local BaseActiveObject = require("base_active_object")
local InputDevice = require("input_device")

local GameScreen = {}
GameScreen.__index = GameScreen

function GameScreen:new(game_ref, screen_manager_ref)
    local instance = setmetatable({}, GameScreen)
    instance.game = game_ref
    instance.screen_manager = screen_manager_ref
    instance.font = love.graphics.newFont(14)
    
    -- Key states for raw LÖVE key checking (still useful for global keys like Esc, or if not using InputDevice for some things)
    instance.key_states = {
        z = "released" -- Example for toggling debug, not handled by InputDevice for character
    }
    instance.tracked_keys = { -- Only for keys NOT handled by player's InputDevice
        ["z"] = "z"
    }

    instance.joysticks = {} -- For LÖVE's direct joystick events if needed elsewhere
    instance.joystick_button_states = {}
    instance.joystick_axis_states = {}

    -- Load Character Data
    local ryu_data = _G.GAME.assets.objects["SF3/Ryu"]
    if not ryu_data then
        print("Error: SF3/Ryu data not found in _G.GAME.assets.objects! Ensure it is loaded in main.lua.")
    end

    -- Create InputDevice for Player 1
    local p1_input_device = InputDevice:new({
        game = game_ref,
        team = 1,
        mode = "keyboard"
        -- active_object will be set by BaseActiveObject
    })

    instance.player1 = BaseActiveObject:new({
        dict = ryu_data,
        pos = {x = game_ref.resolution.width / 4, y = game_ref.resolution.height - 100, z = 0},
        face = 1,
        inicial_state = "Stand",
        team = 1,
        inputdevice = p1_input_device -- Pass the input device here
    })
    -- p1_input_device.active_object = instance.player1 -- This is now done in BaseActiveObject:new

    if not instance.player1.image_key then
        print("Warning: player1.image_key is nil after BaseActiveObject:new(). Using fallback.")
        instance.player1.image_key = "reencor_1" 
        instance.player1.image_size = {w=32, h=32}
        instance.player1.image_offset = {x=16, y=16}
    end

    -- Load Stage Data
    local stage_data = _G.GAME.assets.objects["Reencor/Training"]
    if not stage_data then
        print("Error: Reencor/Training data not found in _G.GAME.assets.objects!")
    end

    instance.stage = BaseActiveObject:new({
        -- game = game_ref,
        dict = stage_data,
        pos = {x = game_ref.resolution.width / 2, y = game_ref.resolution.height / 2, z = -1},
        inicial_state = "Stand" -- Assuming stages also have a "Stand" state for their main image
    })
     -- Ensure stage has a valid image_key
    if not instance.stage.image_key then
        print("Warning: stage.image_key is nil. Using fallback.")
        instance.stage.image_key = "Reencor/TrainingBG" 
        instance.stage.image_size = {w=640, h=400} 
        instance.stage.image_offset = {x=320, y=200} 
    end
    instance.stage.gravity = 0

    return instance
end

function GameScreen:load()
    print("GameScreen loaded")
    for i, joystick in ipairs(love.joystick.getJoysticks()) do
        if joystick then
            self.joysticks[joystick:getID()] = joystick
            self.joystick_button_states[joystick:getID()] = {}
            self.joystick_axis_states[joystick:getID()] = {}
        end
    end
end

function GameScreen:update(dt)
    -- Player 1 update is now driven by its input device, called within player1:update()
    if self.player1 then
        self.player1:update(dt) 
        -- Simple ground check for player1 (could be moved into BaseActiveObject or a physics module)
        if self.player1.pos.y > self.game.resolution.height - 60 then
            self.player1.pos.y = self.game.resolution.height - 60
            self.player1.speed.y = 0
            self.player1.fet = "grounded"
            if self.player1.current_state_name == "Jump" then -- If was jumping, return to Stand
                self.player1:change_state("Stand")
            end
        end
    end

    if self.stage then
        self.stage:update(dt)
    end
end

function GameScreen:draw()
    love.graphics.setFont(self.font)
    love.graphics.printf("Game Screen", 0, 10, self.game.resolution.width, "center")
    love.graphics.printf("Press Esc to Return to Title. P1 uses U,I,O,J,K,L,W,A,S,D. Z to toggle boxes.", 0, 30, self.game.resolution.width, "center")

    if self.stage then self.stage:draw() end
    if self.player1 then self.player1:draw() end

    -- UI and Debug info drawing
    local y_offset = self.game.resolution.height - 180 -- Adjusted starting y
    
    -- Display Player 1 InputDevice buffer and commands
    if self.player1 and self.player1.inputdevice then
        local id = self.player1.inputdevice
        love.graphics.print("P1 Input Buffer:", 10, y_offset)
        y_offset = y_offset + 15
        for i = math.max(1, #id.input_buffer - 4), #id.input_buffer do -- Last 5 events
            local event = id.input_buffer[i]
            love.graphics.print(string.format("%s - %.0fms", event.notation, event.time % 100000), 10, y_offset)
            y_offset = y_offset + 15
        end
        y_offset = y_offset + 5
        love.graphics.print("P1 Commands: " .. table.concat(id.current_command_input, ", "), 10, y_offset)
        y_offset = y_offset + 20
    end
    
    -- Raw key states (for non-InputDevice keys like 'z')
    love.graphics.print("Global Key States:", 10, y_offset)
    y_offset = y_offset + 15
    for key, state in pairs(self.key_states) do
        love.graphics.print(string.format("%s: %s", key, state), 10, y_offset)
        y_offset = y_offset + 15
    end

    if self.player1 then
        y_offset = 60 -- Reset for P1 debug on right side
        love.graphics.print("Player 1 Debug:", self.game.resolution.width - 220, y_offset)
        y_offset = y_offset + 15
        love.graphics.print(string.format("Pos: (%.1f, %.1f)", self.player1.pos.x, self.player1.pos.y), self.game.resolution.width - 220, y_offset)
        y_offset = y_offset + 15
        love.graphics.print(string.format("Speed: (%.1f, %.1f)", self.player1.speed.x, self.player1.speed.y), self.game.resolution.width - 220, y_offset)
        y_offset = y_offset + 15
        love.graphics.print(string.format("State: %s (fet: %s)", self.player1.current_state_name, self.player1.fet or "N/A"), self.game.resolution.width - 220, y_offset)
        y_offset = y_offset + 15
        love.graphics.print(string.format("ImageKey: %s", self.player1.image_key or "nil"), self.game.resolution.width - 220, y_offset)
    end
end

function GameScreen:keypressed(key, scancode, isrepeat)
    -- Global key presses (like Esc, or debug toggles not meant for character input)
    if key == "escape" then
        local TitleScreen = require("screens.title_screen")
        self.screen_manager.switch(TitleScreen)
    end

    local tracked_name = self.tracked_keys[key]
    if tracked_name then
        self.key_states[tracked_name] = "pressed"
        -- Control face direction based on horizontal movement keys
        -- This is now handled by InputDevice and BaseActiveObject:handle_input_commands for player1
        -- if tracked_name == "left" and self.player1 then
        --     self.player1.face = -1
        -- elseif tracked_name == "right" and self.player1 then
        --     self.player1.face = 1
        -- end
    end

    -- Player 1's InputDevice will pick up its relevant keys via love.keyboard.isDown() in its own update cycle.
    -- So, no need to explicitly pass key presses to it from here for keyboard mode.
    -- However, for joystick events, those callbacks (gamepadpressed etc.) would need to update
    -- the InputDevice's raw_input_state if it's in joystick mode.

    if key == "z" then -- Toggle show_boxes for the test object
        self.game.show_boxes = not self.game.show_boxes
        print("Toggled game.show_boxes to: " .. tostring(self.game.show_boxes))
        end
    end

    if key == "z" then -- Toggle show_boxes for the test object
        self.game.show_boxes = not self.game.show_boxes
        print("Toggled game.show_boxes to: " .. tostring(self.game.show_boxes))
    end
end

function GameScreen:keyreleased(key)
    local tracked_name = self.tracked_keys[key]
    if tracked_name then
        self.key_states[tracked_name] = "released"
    end
end

function GameScreen:gamepadpressed(joystick, button)
    local tracked_name = self.tracked_keys[key]
    if tracked_name then
        self.key_states[tracked_name] = "released"
    end
end

function GameScreen:gamepadpressed(joystick, button)
    local joy_id = joystick:getID()
    print(string.format("Gamepad pressed: Joystick ID %d, Button %s", joy_id, tostring(button)))
    if not self.joystick_button_states[joy_id] then self.joystick_button_states[joy_id] = {} end
    self.joystick_button_states[joy_id][tostring(button)] = "pressed"
    if not self.joysticks[joy_id] then self.joysticks[joy_id] = joystick end -- Ensure joystick is tracked
end

function GameScreen:gamepadreleased(joystick, button)
    local joy_id = joystick:getID()
    print(string.format("Gamepad released: Joystick ID %d, Button %s", joy_id, tostring(button)))
    if not self.joystick_button_states[joy_id] then self.joystick_button_states[joy_id] = {} end
    self.joystick_button_states[joy_id][tostring(button)] = "released"
end

function GameScreen:gamepadaxis(joystick, axis, value)
    local joy_id = joystick:getID()
    -- LÖVE 11.0+ returns axis name directly, older versions might need mapping
    local axis_name = joystick:getGamepadAxis(axis) or "Axis "..tostring(axis) 
    print(string.format("Gamepad axis: Joystick ID %d, Axis %s, Value %.2f", joy_id, axis_name, value))
    if not self.joystick_axis_states[joy_id] then self.joystick_axis_states[joy_id] = {} end
    self.joystick_axis_states[joy_id][axis_name] = value
    if not self.joysticks[joy_id] then self.joysticks[joy_id] = joystick end -- Ensure joystick is tracked
end

function GameScreen:joystickadded(joystick)
    local joy_id = joystick:getID()
    print(string.format("Joystick ADDED in GameScreen: ID=%d, Name=%s", joy_id, joystick:getName()))
    self.joysticks[joy_id] = joystick
    self.joystick_button_states[joy_id] = {}
    self.joystick_axis_states[joy_id] = {}
    -- Initialize axis states, as they might not emit an event until moved
    for i=1, joystick:getAxes() do
        local axis_name = joystick:getGamepadAxis(i) or "Axis "..tostring(i)
        self.joystick_axis_states[joy_id][axis_name] = joystick:getAxis(i) or 0.0
    end
end

function GameScreen:joystickremoved(joystick)
    local joy_id = joystick:getID()
    print(string.format("Joystick REMOVED in GameScreen: ID=%d, Name=%s", joy_id, joystick:getName()))
    self.joysticks[joy_id] = nil
    self.joystick_button_states[joy_id] = nil
    self.joystick_axis_states[joy_id] = nil
end


function GameScreen:unload()
    print("GameScreen unloaded")
end

return GameScreen
