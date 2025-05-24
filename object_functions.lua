local ObjectFunctions = {}

-- This table will store functions that can be called from frame_entry.func
ObjectFunctions.registry = {}

--- Sets the speed of the object.
-- @param obj The BaseActiveObject instance.
-- @param vx The speed on the x-axis.
-- @param vy The speed on the y-axis.
function ObjectFunctions.registry.set_speed(obj, vx, vy)
    if obj then
        obj.speed.x = tonumber(vx) or obj.speed.x
        obj.speed.y = tonumber(vy) or obj.speed.y
        print(string.format("Object '%s': set_speed(%.2f, %.2f)", obj.key_name or obj.type, obj.speed.x, obj.speed.y))
    end
end

--- Sets the acceleration of the object.
-- @param obj The BaseActiveObject instance.
-- @param ax The acceleration on the x-axis.
-- @param ay The acceleration on the y-axis.
function ObjectFunctions.registry.set_accel(obj, ax, ay)
    if obj then
        obj.acceleration.x = tonumber(ax) or obj.acceleration.x
        obj.acceleration.y = tonumber(ay) or obj.acceleration.y
        print(string.format("Object '%s': set_accel(%.2f, %.2f)", obj.key_name or obj.type, obj.acceleration.x, obj.acceleration.y))
    end
end

--- Changes the state of the object.
-- @param obj The BaseActiveObject instance.
-- @param new_state_name The name of the new state.
function ObjectFunctions.registry.change_state(obj, new_state_name)
    if obj and new_state_name then
        print(string.format("Object '%s': func attempting change_state to '%s'", obj.key_name or obj.type, new_state_name))
        obj:change_state(new_state_name)
    end
end

--- Creates a projectile (placeholder).
-- @param obj The BaseActiveObject instance.
-- @param projectile_key_name The key name of the projectile to create.
function ObjectFunctions.registry.create_projectile(obj, projectile_key_name)
    if obj and projectile_key_name then
        print(string.format("Object '%s': Creating projectile: %s", obj.key_name or obj.type, projectile_key_name))
        -- In a real game, this would instantiate a new BaseActiveObject or a specialized Projectile object.
        -- Example: local proj = BaseActiveObject:new({ dict = _G.GAME.assets.objects[projectile_key_name], pos = {x=obj.pos.x + (obj.face * 30), y=obj.pos.y}, face = obj.face, team = obj.team })
        -- _G.GAME.current_screen_instance:add_object(proj) -- Assuming GameScreen has an add_object method
    end
end

--- Plays a sound (placeholder).
-- @param obj The BaseActiveObject instance.
-- @param sound_key The key of the sound to play from _G.GAME.assets.sounds.
function ObjectFunctions.registry.play_sound(obj, sound_key)
    if obj and sound_key then
        print(string.format("Object '%s': Playing sound: %s", obj.key_name or obj.type, sound_key))
        if _G.GAME.assets.sounds[sound_key] then
            love.audio.play(_G.GAME.assets.sounds[sound_key])
        else
            print(string.format("Warning: Sound key '%s' not found in assets.", sound_key))
        end
    end
end


--- Parses a function string like "func_name(arg1, arg2, ...)"
-- @param func_str The string to parse.
-- @return func_name (string), args (table of strings) or nil if parse fails.
local function parse_func_string(func_str)
    local func_name, args_str = string.match(func_str, "([%w_]+)%s*%((.*)%)")
    if not func_name then
        func_name = string.match(func_str, "([%w_]+)") -- No args
        if not func_name then return nil end
        return func_name, {}
    end

    local args = {}
    if args_str and args_str ~= "" then
        for arg in string.gmatch(args_str, "([^,]+)") do
            local clean_arg = string.match(string.gsub(arg, "^%s*(.-)%s*$", "%1"), "^['\"]?(.-)['\"]?$")
            table.insert(args, clean_arg)
        end
    end
    return func_name, args
end

--- Executes functions defined in a frame_entry.
-- @param obj The BaseActiveObject instance.
-- @param frame_entry The frame data which might contain a 'func' field.
function ObjectFunctions.execute(obj, frame_entry)
    if not frame_entry or not frame_entry.func then
        return
    end

    local funcs_to_call = {}
    if type(frame_entry.func) == "string" then
        table.insert(funcs_to_call, frame_entry.func)
    elseif type(frame_entry.func) == "table" then
        funcs_to_call = frame_entry.func
    end

    for _, func_call_str in ipairs(funcs_to_call) do
        local func_name, args = parse_func_string(func_call_str)
        if func_name then
            local func_to_run = ObjectFunctions.registry[func_name]
            if func_to_run then
                -- print(string.format("Executing func: %s with args: %s", func_name, table.concat(args, ", ")))
                func_to_run(obj, unpack(args))
            else
                print(string.format("Warning: Function '%s' not found in ObjectFunctions.registry.", func_name))
            end
        else
            print(string.format("Warning: Could not parse function call string: '%s'", func_call_str))
        end
    end
end

return ObjectFunctions
