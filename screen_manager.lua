local ScreenManager = {}
ScreenManager.stack = {}
ScreenManager.game_reference = nil -- To hold reference to GAME table from main.lua

-- Private function to call a method on the current screen if it exists
local function call_current_screen_method(method_name, ...)
    local current_screen_instance = ScreenManager.stack[#ScreenManager.stack]
    if current_screen_instance and current_screen_instance[method_name] then
        return current_screen_instance[method_name](current_screen_instance, ...)
    end
end

function ScreenManager.load(initial_screen_module, game_ref, ...)
    ScreenManager.game_reference = game_ref -- Store the game reference
    if initial_screen_module then
        local screen_instance = initial_screen_module:new(ScreenManager.game_reference, ...)
        table.insert(ScreenManager.stack, screen_instance)
        if screen_instance.load then
            screen_instance:load()
        end
    else
        print("ScreenManager.load: Error - initial_screen_module is nil")
    end
end

function ScreenManager.push(screen_module, ...)
    if screen_module then
        local screen_instance = screen_module:new(ScreenManager.game_reference, ...)
        table.insert(ScreenManager.stack, screen_instance)
        if screen_instance.load then
            screen_instance:load()
        end
    else
        print("ScreenManager.push: Error - screen_module is nil")
    end
end

function ScreenManager.pop()
    if #ScreenManager.stack > 0 then
        local popped_screen = table.remove(ScreenManager.stack)
        if popped_screen and popped_screen.unload then -- Optional unload method
            popped_screen:unload()
        end
        -- If stack becomes empty, consider what to do. Quit or load a default screen?
        -- For now, it will just be empty. love.update/draw will do nothing.
        if #ScreenManager.stack == 0 then
            print("ScreenManager.pop: Stack is now empty.")
        end
        return popped_screen
    else
        print("ScreenManager.pop: Error - Stack is already empty.")
        return nil
    end
end

function ScreenManager.switch(screen_module, ...)
    -- Pop all existing screens
    while #ScreenManager.stack > 0 do
        local popped_screen = table.remove(ScreenManager.stack)
        if popped_screen and popped_screen.unload then
            popped_screen:unload()
        end
    end
    -- Push the new screen
    if screen_module then
        ScreenManager.push(screen_module, ...) -- game_reference is already passed by push
    else
        print("ScreenManager.switch: Error - screen_module is nil")
    end
end

function ScreenManager.update(dt)
    call_current_screen_method("update", dt)
end

function ScreenManager.draw()
    call_current_screen_method("draw")
end

function ScreenManager.keypressed(key, scancode, isrepeat)
    call_current_screen_method("keypressed", key, scancode, isrepeat)
end

function ScreenManager.mousepressed(x, y, button, istouch, presses)
    call_current_screen_method("mousepressed", x, y, button, istouch, presses)
end

function ScreenManager.mousereleased(x, y, button, istouch, presses)
    call_current_screen_method("mousereleased", x, y, button, istouch, presses)
end

function ScreenManager.keyreleased(key) -- Added missing keyreleased
    call_current_screen_method("keyreleased", key)
end

function ScreenManager.gamepadpressed(joystick, button)
    call_current_screen_method("gamepadpressed", joystick, button)
end

function ScreenManager.gamepadreleased(joystick, button)
    call_current_screen_method("gamepadreleased", joystick, button)
end

function ScreenManager.gamepadaxis(joystick, axis, value)
    call_current_screen_method("gamepadaxis", joystick, axis, value)
end

function ScreenManager.joystickadded(joystick)
    call_current_screen_method("joystickadded", joystick)
end

function ScreenManager.joystickremoved(joystick)
    call_current_screen_method("joystickremoved", joystick)
end

-- Add other input forwarding functions as needed (e.g., textinput, wheelmoved etc.)

return ScreenManager
