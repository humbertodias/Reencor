local InputDevice = {}
InputDevice.__index = InputDevice

InputDevice.MAX_BUFFER_SIZE = 60 -- Store last 60 events (approx 1 second at 60fps)
InputDevice.COMMAND_WINDOW_MS = 300 -- Milliseconds for command sequence parts

function InputDevice:new(params)
    local self = setmetatable({}, InputDevice)

    self.game = params.game or _G.GAME
    self.team = params.team or 1
    self.mode = params.mode or "keyboard"
    self.active_object = params.active_object or nil -- Link to the object it controls

    self.raw_input_state = {up=false, down=false, left=false, right=false, b1=false, b2=false, b3=false, b4=false, b5=false, b6=false}
    self.prev_raw_input_state = {} -- To detect changes
    for k,v in pairs(self.raw_input_state) do self.prev_raw_input_state[k] = v end

    self.input_buffer = {} -- Stores {notation="6", time=love.timer.getTime(), type="directional"} or {notation="p_b1", time=..., type="button_press"}
    self.current_command_input = {} -- Stores recognized commands and current presses for this frame
    self.inter_press = false -- True if a new input was pressed/released this frame

    -- Default key mapping for keyboard mode (player 1)
    if self.team == 1 then
        self.key_mapping = {
            w = "up", s = "down", a = "left", d = "right",
            u = "b1", i = "b2", o = "b3", -- Punches
            j = "b4", k = "b5", l = "b6"  -- Kicks
        }
    elseif self.team == 2 then -- Example for P2, might need different keys
         self.key_mapping = {
            up = "up", down = "down", left = "left", right = "right", -- Arrow keys
            kp7 = "b1", kp8 = "b2", kp9 = "b3",
            kp4 = "b4", kp5 = "b5", kp6 = "b6"
        }
    else
        self.key_mapping = {} -- AI or other modes might not use direct key mapping
    end
    
    -- Command list using numpad notation
    self.command_list = {
        { name = "QCF", sequence = {"2", "3", "6"}, press_type = "button_press_required" },
        { name = "QCB", sequence = {"2", "1", "4"}, press_type = "button_press_required" },
        { name = "DP",  sequence = {"6", "5", "2", "3"}, press_type = "button_press_required" }, -- Simplified DP, true DP is 623
        { name = "RDP", sequence = {"4", "5", "2", "1"}, press_type = "button_press_required" },
        { name = "FORWARD_DASH", sequence = {"6", "5", "6"}, press_type = "directional_only", time_limit_ms = 200 },
        { name = "BACK_DASH",    sequence = {"4", "5", "4"}, press_type = "directional_only", time_limit_ms = 200 },
        -- Add more commands like HCF, HCB, 360s later
    }
    
    print(string.format("InputDevice created for team %d, mode %s", self.team, self.mode))
    return self
end

function InputDevice:get_numpad_notation(raw_state)
    local face = (self.active_object and self.active_object.face == -1) and -1 or 1 -- Default to 1 if no active_object or face
    
    local effective_left = (face == 1 and raw_state.left) or (face == -1 and raw_state.right)
    local effective_right = (face == 1 and raw_state.right) or (face == -1 and raw_state.left)

    if raw_state.up then
        if effective_left then return "7"
        elseif effective_right then return "9"
        else return "8" end
    elseif raw_state.down then
        if effective_left then return "1"
        elseif effective_right then return "3"
        else return "2" end
    else
        if effective_left then return "4"
        elseif effective_right then return "6"
        else return "5" end -- Neutral
    end
end

function InputDevice:update_keyboard()
    local current_raw = {}
    for k,_ in pairs(self.raw_input_state) do current_raw[k] = false end -- Reset

    for love_key, abstract_input in pairs(self.key_mapping) do
        if love.keyboard.isDown(love_key) then
            current_raw[abstract_input] = true
        end
    end
    
    -- Update raw_input_state and check for inter_press
    self.inter_press = false
    for key, value in pairs(current_raw) do
        if self.raw_input_state[key] ~= value then
            self.inter_press = true
            self.raw_input_state[key] = value
            
            local event_time = love.timer.getTime() * 1000 -- milliseconds
            if value == true then -- Key pressed
                if key == "up" or key == "down" or key == "left" or key == "right" then
                    -- For directionals, we add the numpad notation to buffer
                    local numpad_notation = self:get_numpad_notation(self.raw_input_state)
                    self:add_to_buffer({notation = numpad_notation, time = event_time, type = "directional"})
                else -- Button pressed
                    self:add_to_buffer({notation = "p_"..key, time = event_time, type = "button_press"}) -- e.g. p_b1
                end
            else -- Key released
                 if not (key == "up" or key == "down" or key == "left" or key == "right") then
                    self:add_to_buffer({notation = "r_"..key, time = event_time, type = "button_release"}) -- e.g. r_b1
                 end
                 -- We could also add directional releases if needed, e.g. "r_5" when returning to neutral
                 -- For now, only directional presses (as numpad) and button presses/releases go to buffer
            end
        end
    end
end

function InputDevice:add_to_buffer(input_event)
    table.insert(self.input_buffer, input_event)
    if #self.input_buffer > InputDevice.MAX_BUFFER_SIZE then
        table.remove(self.input_buffer, 1)
    end
    -- print(string.format("Buffer (%d): %s %s (%.2f)", #self.input_buffer, input_event.type, input_event.notation, input_event.time))
end

function InputDevice:check_commands()
    local recognized_this_frame = {}
    local current_time = love.timer.getTime() * 1000

    -- 1. Add currently held/just pressed raw inputs (numpad and buttons)
    local current_numpad = self:get_numpad_notation(self.raw_input_state)
    if current_numpad ~= "5" then -- if not neutral, add it.
        -- Check if this numpad state is new or just held (to avoid spamming commands)
        local last_event = self.input_buffer[#self.input_buffer]
        if last_event and last_event.type == "directional" and last_event.notation == current_numpad and (current_time - last_event.time < 50) then -- Arbitrary 50ms for "just pressed"
             table.insert(recognized_this_frame, current_numpad)
        elseif current_numpad ~= (self.prev_raw_input_state and self:get_numpad_notation(self.prev_raw_input_state) or "5") then
            -- if it's different from previous frame's numpad, also treat as new
            table.insert(recognized_this_frame, current_numpad)
        end
    end

    for btn, is_pressed in pairs(self.raw_input_state) do
        if not (btn == "up" or btn == "down" or btn == "left" or btn == "right") and is_pressed then
            local last_event = self.input_buffer[#self.input_buffer]
            if last_event and last_event.type == "button_press" and last_event.notation == "p_"..btn and (current_time - last_event.time < 50) then
                table.insert(recognized_this_frame, "p_"..btn)
            end
        end
    end
    
    -- 2. Check for command sequences
    for _, cmd in ipairs(self.command_list) do
        local sequence_idx = #cmd.sequence
        local last_event_time = current_time
        local matched_count = 0
        local first_event_time_of_match = current_time
        local button_press_for_command = nil

        for i = #self.input_buffer, 1, -1 do
            local event = self.input_buffer[i]
            local time_limit = cmd.time_limit_ms or InputDevice.COMMAND_WINDOW_MS

            if current_time - event.time > time_limit * #cmd.sequence then -- Optimization: too old
                break
            end

            if event.type == "directional" and event.notation == cmd.sequence[sequence_idx] then
                if sequence_idx == #cmd.sequence then -- first element of sequence we are matching (from end)
                    last_event_time = event.time
                elseif (last_event_time - event.time) > time_limit then
                    break -- Too much time between inputs in the sequence
                end
                
                first_event_time_of_match = event.time
                last_event_time = event.time
                sequence_idx = sequence_idx - 1
                matched_count = matched_count + 1
            elseif cmd.press_type == "button_press_required" and event.type == "button_press" and sequence_idx == 0 then
                 -- If sequence is fully matched (idx is 0), check for button press within window
                 if (event.time - first_event_time_of_match) < time_limit then -- Button pressed shortly after sequence
                    button_press_for_command = event.notation -- e.g. "p_b1"
                    matched_count = matched_count + 1 -- Count the button as part of the match
                    break -- Found button for this command
                 end
            end

            if sequence_idx == 0 then break end -- Full directional sequence matched
        end

        if sequence_idx == 0 then -- Directional part matched
            if cmd.press_type == "directional_only" then
                table.insert(recognized_this_frame, cmd.name)
                -- print("Recognized directional command:", cmd.name)
            elseif cmd.press_type == "button_press_required" then
                if button_press_for_command then
                     table.insert(recognized_this_frame, cmd.name)
                     table.insert(recognized_this_frame, button_press_for_command) -- also add the button that triggered it
                     -- print("Recognized button command:", cmd.name, "with", button_press_for_command)
                else
                    -- Check if a button is currently pressed that wasn't part of buffer yet (e.g. very simultaneous press)
                    for btn_key, is_down in pairs(self.raw_input_state) do
                        if not (btn_key == "up" or btn_key == "down" or btn_key == "left" or btn_key == "right") and is_down then
                             -- Is this button press "new"? Compare to prev_raw_input_state
                            if not self.prev_raw_input_state[btn_key] then
                                table.insert(recognized_this_frame, cmd.name)
                                table.insert(recognized_this_frame, "p_"..btn_key)
                                -- print("Recognized button command (current press):", cmd.name, "with p_", btn_key)
                                break -- Found a button
                            end
                        end
                    end
                end
            end
        end
    end
    self.current_command_input = recognized_this_frame
end


function InputDevice:update()
    -- Store previous raw state for change detection
    for k,v in pairs(self.raw_input_state) do self.prev_raw_input_state[k] = v end
    
    self.current_command_input = {} -- Clear previous frame's commands
    -- self.inter_press = false -- This will be set by update_keyboard if changes occur

    if self.mode == "keyboard" then
        self:update_keyboard()
    elseif self.mode == "joystick" then
        -- self:update_joystick() -- To be implemented
    elseif self.mode == "AI" then
        -- self:update_ai() -- To be implemented
    end

    -- After raw input is updated and inter_press is potentially set,
    -- and input_buffer is potentially updated (all handled in update_keyboard for now):
    self:check_commands() -- This populates self.current_command_input

    -- For debugging:
    -- if #self.current_command_input > 0 then
    --    print("Frame Commands: ", table.concat(self.current_command_input, ", "))
    -- end
end

return InputDevice
