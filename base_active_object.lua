local BaseActiveObject = {}
BaseActiveObject.__index = BaseActiveObject

function BaseActiveObject:new(params)
    local self = setmetatable({}, BaseActiveObject)

    self.game = _G.GAME -- Reference to the global game table

    -- Default values, to be overridden by params.dict or params directly
    self.type = "default_object"
    self.pos = {x = params.pos and params.pos.x or 0, y = params.pos and params.pos.y or 0, z = params.pos and params.pos.z or 0}
    self.face = params.face or 1 -- 1 for right, -1 for left
    self.palette = params.palette or nil
    self.parent_obj = params.parent or nil -- Renamed from parent
    self.team = params.team or "neutral"
    self.inputdevice = params.inputdevice or nil
    if self.inputdevice then
        self.inputdevice.active_object = self -- Link back to this object
    end

    self.image_key = nil
    self.image_offset = {x = 0, y = 0, z = 0}
    self.image_size = {w = 0, h = 0, d = 0}
    self.image_mirror = {x = false, y = false}
    self.image_tint = {r = 255, g = 255, b = 255, a = 255}
    self.image_angle = {x = 0, y = 0, z = 0}

    self.scale = 1.0
    self.speed = {x = 0, y = 0}
    self.acceleration = {x = 0, y = 0}
    
    self.current_state_name = params.inicial_state or "Stand" -- Store the name
    self.current_state_data = nil -- Will hold the actual state data
    self.hitstop = 0
    self.frame = {current = 0, duration = 0, current_frame_index = 1} -- current_frame_index is 1-based for Lua tables

    self.dict = params.dict or {}
    if self.dict then
        self.type = self.dict.type or self.type
        self.scale = self.dict.scale or self.scale -- dict.scale overrides param.scale if dict is present
        self.gravity = self.dict.gravity or 0
        -- key_name is added by loadJsonAsset in main.lua
        self.key_name = self.dict.key_name or (params.dict and params.dict.name) or "unknown_object" 
    else
        self.key_name = "unknown_object"
    end
     -- params.scale can still override if dict.scale is not present (though less likely with full dicts)
    if params.scale then self.scale = params.scale end


    -- Set initial state and sprite/box data
    self:change_state(self.current_state_name) -- Call this to initialize from state

    -- If an explicit image_key is passed in params AFTER dict processing, it might override state-derived one.
    -- This is less common if using full dicts but provides a fallback.
    if params.image_key then
        self.image_key = params.image_key
    end
    
    self.fet = "grounded"

    -- Collision Boxes - Initialized by change_state or directly from dict.boxes if no state handling
    self.boxes = {} 
    if params.dict and params.dict.boxes then
        -- In a real scenario, boxes might be structured per state or animation frame
        -- For now, assume params.dict.boxes provides a default set of boxes for the object
        -- The structure in params.dict.boxes should match what self.boxes expects
        -- e.g., params.dict.boxes = { hurtbox = {{x=-10,...}}, hitbox = {{...}} }
        for box_type, box_list in pairs(params.dict.boxes) do
            self.boxes[box_type] = {}
            for _, box_data in ipairs(box_list) do
                table.insert(self.boxes[box_type], {
                    x = box_data.x or 0,
                    y = box_data.y or 0,
                    w = box_data.w or 0,
                    h = box_data.h or 0,
                    type = box_type -- Store the type for potential differentiation
                })
            end
        end
        -- This initial load from dict.boxes might be overridden by change_state if state has specific boxes
        print(string.format("Loaded default boxes from dict.boxes for %s", self.type))
    end
     -- Direct override from params (after dict and state) for maximum flexibility if needed
    if params.boxes then
         self.boxes = params.boxes
    end

    print(string.format("BaseActiveObject created: type=%s, key_name=%s, pos=(%d,%d), face=%d, state=%s", self.type, self.key_name, self.pos.x, self.pos.y, self.face, self.current_state_name))
    return self
end

function BaseActiveObject:change_state(new_state_name)
    if not self.dict or not self.dict.states or not self.dict.states[new_state_name] then
        print(string.format("Warning: State '%s' not found for object '%s'. Remaining in '%s'.", new_state_name, self.key_name, self.current_state_name))
        -- Attempt to set a fallback image if current_state_data is also nil
        if not self.current_state_data and self.dict and self.dict.image_key then
             self.image_key = self.dict.image_key -- Fallback to a global image_key in dict
             print(string.format("Using fallback image_key: %s", self.image_key))
        elseif not self.current_state_data then
            print(string.format("No valid state and no fallback image_key for %s.", self.key_name))
        end
        return
    end

    self.current_state_name = new_state_name
    self.current_state_data = self.dict.states[new_state_name]
    self.frame.current = 0
    self.frame.current_frame_index = 1 -- Reset to first frame

    -- Update sprite, offset, size, and boxes from the new state's first frame
    self:update_frame_data()

    print(string.format("Object '%s' changed to state '%s'", self.key_name, self.current_state_name))
end

function BaseActiveObject:update_frame_data()
    if not self.current_state_data or not self.current_state_data.framedata then
        -- print(string.format("Warning: No framedata for state '%s' in object '%s'", self.current_state_name, self.key_name))
        if self.dict and self.dict.image_key then -- Try to use a global image_key as fallback
            self.image_key = self.dict.image_key
            if self.dict.image_offset then self.image_offset = self.dict.image_offset end
            if self.dict.image_size then self.image_size = self.dict.image_size end
            -- print(string.format("Using fallback image data for %s from dict root", self.key_name))
        end
        return
    end

    local frame_entry = self.current_state_data.framedata[self.frame.current_frame_index]
    if not frame_entry then
        print(string.format("Warning: Frame index %d not found in framedata for state '%s', object '%s'. Resetting to frame 1.", self.frame.current_frame_index, self.current_state_name, self.key_name))
        self.frame.current_frame_index = 1
        frame_entry = self.current_state_data.framedata[self.frame.current_frame_index]
        if not frame_entry then
            print(string.format("Error: Still no frame_entry at index 1 for state '%s', object '%s'. No image will be set.", self.current_state_name, self.key_name))
            return
        end
    end

    if frame_entry.sprite then
        -- Construct image key using key_name (e.g., "SF3/Ryu") and sprite name (e.g., "stand_0")
        self.image_key = self.key_name .. "/" .. frame_entry.sprite
    else
        -- print(string.format("Warning: No sprite defined for frame %d in state '%s', object '%s'", self.frame.current_frame_index, self.current_state_name, self.key_name))
    end
    
    self.image_offset = frame_entry.offset or self.image_offset
    self.image_size = frame_entry.size or self.image_size
    
    self.current_frame_duration = tonumber(frame_entry.dur) or 1 -- Store current frame's duration in frames (from JSON)
    self.frame_timer = 0 -- Reset frame timer for the new frame

    -- Update collision boxes if defined for this frame or state
    -- For now, let's assume boxes are defined at the state level or globally in dict.boxes
    -- If frame_entry.boxes exists, it would override state/global boxes.
    if frame_entry.boxes then
        self.boxes = frame_entry.boxes 
    elseif self.current_state_data and self.current_state_data.boxes then
        self.boxes = self.current_state_data.boxes
    elseif self.dict and self.dict.boxes then
        self.boxes = self.dict.boxes
    else
        self.boxes = {} 
    end
end


function BaseActiveObject:update(dt)
    if self.inputdevice then
        self.inputdevice:update()
        self:handle_input_commands(self.inputdevice.current_command_input)
    end

    if self.hitstop > 0 then
        self.hitstop = self.hitstop - dt * 60 
        if self.hitstop < 0 then self.hitstop = 0 end
        return 
    end
    
    -- Movement and physics logic
    if not (self.current_state_data and self.current_state_data.ignore_physics) then
        if not (self.current_state_data and self.current_state_data.no_move_x) then
            self.speed.x = self.speed.x + (self.acceleration.x * self.face * dt)
            self.pos.x = self.pos.x + (self.speed.x * self.face * dt)
        end
        if not (self.current_state_data and self.current_state_data.no_move_y) then
            self.speed.y = self.speed.y + (self.acceleration.y * dt)
            self.pos.y = self.pos.y + (self.speed.y * dt)
        end

        if self.fet == "airborne" and self.gravity and self.gravity ~= 0 then
            if not (self.current_state_data and self.current_state_data.ignore_gravity) then
                 self.speed.y = self.speed.y + self.gravity * dt
            end
        end
    end

    -- Frame animation logic
    self.frame_timer = (self.frame_timer or 0) + dt * 60 -- Increment frame timer (simulating frames)

    if self.current_frame_duration and self.frame_timer >= self.current_frame_duration then
        self.frame_timer = self.frame_timer - self.current_frame_duration -- Subtract duration for accuracy
        self.current_frame_index = self.current_frame_index + 1
        
        local current_animation_frames = (self.current_state_data and self.current_state_data.framedata) or {}
        if self.current_frame_index > #current_animation_frames then
            local loop_target = self.current_state_data and self.current_state_data.loop
            if type(loop_target) == "number" then
                self.current_frame_index = loop_target -- Loop to specific frame index (1-based)
            elseif type(loop_target) == "string" then
                self:change_state(loop_target) -- Loop to a different state
                return -- change_state will handle update_frame_data and execute_frame_functions
            elseif loop_target == true or -- Boolean true means loop current animation
                   (self.current_state_name == "Stand" or 
                    self.current_state_name == "F_Walk" or self.current_state_name == "B_Walk" or
                    string.find(self.current_state_name, "Idle") ) then
                self.current_frame_index = 1 
            else 
                -- Default: if not looping and animation ends, go to "Stand"
                -- This is a simplification; JSON should ideally define next_state
                if self.current_state_data and self.current_state_data.next_state then
                    self:change_state(self.current_state_data.next_state)
                else
                    print(string.format("Animation for state '%s' ended. No loop or next_state. Defaulting to 'Stand'.", self.current_state_name))
                    self:change_state("Stand")
                end
                return -- change_state handles the rest
            end
        end
        self:update_frame_data()
        self:execute_frame_functions() -- Execute functions for the new frame
    end
end

function BaseActiveObject:handle_input_commands(commands)
    local walk_speed = (self.dict and self.dict.walk_speed) or 100
    local jump_speed = (self.dict and self.dict.jump_speed) or 400
    local dash_speed = (self.dict and self.dict.dash_speed) or 250

    local command_executed = false 

    -- Do not process new commands if in a state that should complete (e.g. attack)
    if self.current_state_data and self.current_state_data.no_interrupt then
        return
    end

    for _, cmd_name in ipairs(commands) do
        if cmd_name == "QCF" then
            local punch_pressed = false
            for _, btn_cmd in ipairs(commands) do if btn_cmd == "p_b1" or btn_cmd == "p_b2" or btn_cmd == "p_b3" then punch_pressed = true break end end
            if punch_pressed then
                print(self.key_name .. " performs Hadouken (QCF + punch)!")
                self:change_state("Hadouken_LP") 
                command_executed = true
                break
            end
        elseif cmd_name == "DP" then
            local punch_pressed = false
            for _, btn_cmd in ipairs(commands) do if btn_cmd == "p_b1" or btn_cmd == "p_b2" or btn_cmd == "p_b3" then punch_pressed = true break end end
            if punch_pressed then
                print(self.key_name .. " performs Shoryuken (DP + punch)!")
                self:change_state("Shoryuken_LP") 
                command_executed = true
                break
            end
        elseif cmd_name == "FORWARD_DASH" and self.fet == "grounded" then
            print(self.key_name .. " performs FORWARD_DASH!")
            self:change_state("F_Dash")
            command_executed = true
            break
        elseif cmd_name == "BACK_DASH" and self.fet == "grounded" then
            print(self.key_name .. " performs BACK_DASH!")
            self:change_state("B_Dash")
            command_executed = true
            break
        end
    end

    if command_executed then return end 

    if not (self.current_state_data and self.current_state_data.no_move_x) and
       not (self.current_state_data and self.current_state_data.no_move_y) then
        
        local new_speed_x = 0
        local direction_pressed = false
        local current_numpad_dir = nil
        for _, cmd_input in ipairs(commands) do
            if cmd_input == "6" then 
                new_speed_x = walk_speed
                self.face = 1
                direction_pressed = true
                current_numpad_dir = "6"
            elseif cmd_input == "4" then 
                new_speed_x = -walk_speed 
                self.face = -1 
                direction_pressed = true
                current_numpad_dir = "4"
            elseif cmd_input == "p_b1" and self.fet == "grounded" then 
                print(self.key_name .. " performs Light Punch (p_b1)!")
                self:change_state("LPunch") 
                command_executed = true; break
            elseif cmd_input == "p_b4" and self.fet == "grounded" then 
                print(self.key_name .. " performs Light Kick (p_b4)!")
                self:change_state("LKick")
                command_executed = true; break
            end
             -- Check for up directions for jump, can be combined with 4 or 6
            if (cmd_input == "7" or cmd_input == "8" or cmd_input == "9") and self.fet == "grounded" then
                print(self.key_name .. " Jumps! (Input: "..cmd_input..")")
                self.speed.y = -jump_speed
                self.fet = "airborne"
                if cmd_input == "7" then self.face = -1; self:change_state("B_Jump")
                elseif cmd_input == "9" then self.face = 1; self:change_state("F_Jump")
                else self:change_state("N_Jump") end
                command_executed = true; break
            end
        end
        
        if command_executed then return end

        self.speed.x = new_speed_x
        if self.fet == "grounded" then
            if direction_pressed then
                if self.current_state_name ~= "F_Walk" and self.current_state_name ~= "B_Walk" then
                    if current_numpad_dir == "6" then self:change_state("F_Walk")
                    elseif current_numpad_dir == "4" then self:change_state("B_Walk")
                    end
                end
            elseif not direction_pressed and (self.current_state_name == "F_Walk" or self.current_state_name == "B_Walk") then
                 self:change_state("Stand")
            end
        end
    end
end

function BaseActiveObject:execute_frame_functions()
    if not self.current_state_data or not self.current_state_data.framedata or
       self.current_frame_index > #self.current_state_data.framedata then
        return -- No valid frame data
    end

    local frame_entry = self.current_state_data.framedata[self.current_frame_index]
    
    -- Require object_functions here to avoid circular dependency if it's also using BaseActiveObject (though not in this case)
    -- Or, ensure object_functions.lua is loaded before this module if it's a global or passed in.
    -- For simplicity, let's assume _G.ObjectFunctions is available if we go that route, or require it.
    local ObjectFunctions = require("object_functions")
    ObjectFunctions.execute(self, frame_entry)
end


function BaseActiveObject:change_state(new_state_name)
    if not self.dict or not self.dict.states or not self.dict.states[new_state_name] then
        print(string.format("Warning: State '%s' not found for object '%s'. Remaining in '%s'.", new_state_name, self.key_name, self.current_state_name))
        if not self.current_state_data and self.dict and self.dict.image_key then
             self.image_key = self.dict.image_key 
             print(string.format("Using fallback image_key: %s", self.image_key))
        elseif not self.current_state_data then
            print(string.format("No valid state and no fallback image_key for %s.", self.key_name))
        end
        return
    end

    print(string.format("Object '%s': Attempting change_state from '%s' to '%s'", self.key_name, self.current_state_name or "nil", new_state_name))
    self.current_state_name = new_state_name
    self.current_state_data = self.dict.states[new_state_name]
    self.current_frame_index = 1 
    self.frame_timer = 0 

    self:update_frame_data()      -- Load data for the first frame
    self:execute_frame_functions() -- Execute any functions on the first frame

    print(string.format("Object '%s' changed to state '%s', frame 1", self.key_name, self.current_state_name))
end


function BaseActiveObject:draw()
    if not self.image_key then
        -- love.graphics.print("No image_key for object: " .. self.type, self.pos.x, self.pos.y)
        return
    end

    -- Assuming _G.GAME.assets.images is populated
    if not _G.GAME.assets or not _G.GAME.assets.images or not _G.GAME.assets.images[self.image_key] then
        love.graphics.setColor(1,0,0,1) -- Red color for error
        love.graphics.print("Missing image: " .. self.image_key, self.pos.x, self.pos.y)
        love.graphics.setColor(1,1,1,1) -- Reset color
        return
    end

    local img = _G.GAME.assets.images[self.image_key]
    local x, y = self.pos.x, self.pos.y
    local r = self.image_angle.z -- LÖVE 2D uses radians for rotation around Z axis
    
    local sx, sy = self.scale, self.scale
    if self.face == -1 then
        sx = -self.scale -- Flip horizontally
    end
    if self.image_mirror.x then sx = -sx end -- Additional mirroring option
    if self.image_mirror.y then sy = -sy end


    -- Offset handling: LÖVE's offset is where the "center" of transformations is.
    -- If image is flipped, the origin of drawing shifts.
    -- For simple sprite sheet cells, offset is often width/2, height/2 of the cell.
    -- The Python code's self.image_offset seems to be from the top-left.
    local ox, oy = self.image_offset.x, self.image_offset.y
    if self.face == -1 then
        -- When scaled by -1, the image is drawn from its right edge.
        -- To keep the logical (0,0) of the sprite at the same world position,
        -- the offset needs to be adjusted by the image width.
        -- This assumes image_offset is from top-left of original sprite.
        -- And that self.image_size.w is the width of the visual part.
        -- If image_size is not set, this might be tricky.
        if self.image_size and self.image_size.w > 0 then
            ox = self.image_size.w - ox
        else
            -- Fallback: if no image_size, use half image width if available
            -- ox = img:getWidth() - ox -- This might be too simple if image_offset was not 0
            -- Or, more commonly, offset is defined from the center of the sprite for rotation/scaling
            -- Let's assume image_offset is defined from top-left for now.
        end
    end


    local tint_r = self.image_tint.r / 255
    local tint_g = self.image_tint.g / 255
    local tint_b = self.image_tint.b / 255
    local tint_a = self.image_tint.a / 255

    love.graphics.setColor(tint_r, tint_g, tint_b, tint_a)
    love.graphics.draw(img, x, y, r, sx, sy, ox, oy)
    love.graphics.setColor(1, 1, 1, 1) -- Reset color to white

    -- Debug: draw position and origin
    -- love.graphics.setColor(1,0,0,1)
    -- love.graphics.circle("fill", x, y, 3)
    -- love.graphics.setColor(0,1,0,1)
    -- love.graphics.circle("fill", x - ox*sx + (sx < 0 and self.image_size.w * sx or 0) , y - oy*sy, 2) -- Approximate draw origin
    -- love.graphics.setColor(1,1,1,1)

    if _G.GAME.show_boxes then
        -- Draw Hurtboxes (Blue)
        if self.boxes.hurtbox then
            love.graphics.setColor(0, 0, 1, 0.7) -- Blue, semi-transparent
            for _, box_def in ipairs(self.boxes.hurtbox) do
                local box_abs_x = self.pos.x + (self.face == 1 and box_def.x or (-box_def.x - box_def.w))
                local box_abs_y = self.pos.y + box_def.y
                love.graphics.rectangle("line", box_abs_x, box_abs_y, box_def.w, box_def.h)
            end
        end

        -- Draw Hitboxes (Red)
        if self.boxes.hitbox then
            love.graphics.setColor(1, 0, 0, 0.7) -- Red, semi-transparent
            for _, box_def in ipairs(self.boxes.hitbox) do
                local box_abs_x = self.pos.x + (self.face == 1 and box_def.x or (-box_def.x - box_def.w))
                local box_abs_y = self.pos.y + box_def.y
                love.graphics.rectangle("line", box_abs_x, box_abs_y, box_def.w, box_def.h)
            end
        end
        
        -- Draw a box around the image itself (using image_size and offset) for visual comparison
        -- This is the old box drawing logic, kept for reference or specific debug.
        -- love.graphics.setColor(0,1,0,0.5) -- Green, semi-transparent
        -- local img_w = (self.image_size and self.image_size.w or (img and img:getWidth() or 0)) * self.scale
        -- local img_h = (self.image_size and self.image_size.h or (img and img:getHeight() or 0)) * self.scale
        -- local img_box_x = x - ox * sx -- sx already includes self.scale and face
        -- local img_box_y = y - oy * sy -- sy already includes self.scale
        -- if self.face == -1 then
        --      img_box_x = x - ( (self.image_size and self.image_size.w or (img and img:getWidth() or 0)) - ox) * sx
        -- end
        -- love.graphics.rectangle("line", img_box_x, img_box_y, img_w, img_h)

        love.graphics.setColor(1,1,1,1) -- Reset color
    end
end

return BaseActiveObject
