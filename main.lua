_G.json = require("lib.json")
local screen_manager = require("screen_manager")
local TitleScreen = require("screens.title_screen")

-- Game Variables
_G.GAME = {
    resolution = {width = 640, height = 400},
    internal_resolution = {width = 640, height = 400},
    frame_rate = 60,
    camera_focus_point = {x = 0, y = 0},
    show_boxes = false, -- Toggle for drawing debug boxes
    show_inputs = false, -- Toggle for drawing input states (already used in game_screen)
    current_fps = 0,
    assets = {
        images = {},
        sounds = {},
        objects = {} -- For JSON object definitions
    }
}

-- Asset Loading Functions (modified to store in _G.GAME.assets)
function _G.loadJsonAsset(key, filePath) -- Category is always "objects" for these
    local fileData, size = love.filesystem.read(filePath)
    if fileData then
        local success, result = pcall(_G.json.decode, fileData)
        if success then
            result.key_name = key -- Store the key used to access this object_dict
            _G.GAME.assets.objects[key] = result
            print(string.format("Loaded JSON '%s' into _G.GAME.assets.objects.%s", filePath, key))
            return result
        else
            print(string.format("Error parsing JSON file '%s': %s", filePath, tostring(result)))
            return nil
        end
    else
        print(string.format("Error reading file '%s'", filePath))
        return nil
    end
end

function _G.loadImageAsset(key, filePath) -- Key is now the full path-like key, e.g., "SF3/Ryu/stand_0"
    local success, image = pcall(love.graphics.newImage, filePath)
    if success then
        _G.GAME.assets.images[key] = image
        print(string.format("Loaded image '%s' into _G.GAME.assets.images.%s", filePath, key))
        return image
    else
        print(string.format("Error loading image '%s': %s", filePath, tostring(image)))
        return nil
    end
end

function _G.loadSoundAsset(key, filePath)
    local success, sound = pcall(love.audio.newSource, filePath, "static")
    if success then
        _G.GAME.assets.sounds[key] = sound
        print(string.format("Loaded sound '%s' into _G.GAME.assets.sounds.%s", filePath, key))
        return sound
    else
        print(string.format("Error loading sound '%s': %s", filePath, tostring(sound)))
        return nil
    end
end

-- Remove old global test variables
-- _G.testJsonData = nil (already implicitly removed by not using _G.loadJson)
-- _G.testImage = nil (already implicitly removed by not using _G.loadImage)
-- _G.testSound = nil (already implicitly removed by not using _G.loadSound)

function love.load()
    if love.conf and love.conf.window then
        _G.GAME.resolution.width = love.window.getWidth()
        _G.GAME.resolution.height = love.window.getHeight()
    end
    love.window.setMode(_G.GAME.resolution.width, _G.GAME.resolution.height)
    love.timer.setFPSLimit(_G.GAME.frame_rate)

    -- Load assets into the new structure
    print("--- Loading Assets ---")
    -- Character JSONs
    _G.loadJsonAsset("SF3/Ryu", "Assets/objects/SF3/Ryu.json")
    _G.loadJsonAsset("SF3/Ken", "Assets/objects/SF3/Ken.json") 
    -- Stage JSONs
    _G.loadJsonAsset("Reencor/Training", "Assets/objects/Reencor/Training.json")

    -- Manually load a few required sprites based on common initial states for testing
    -- Ryu's stand sprite (assuming it's named stand_0 or similar in Ryu.json)
    -- If Ryu.json states.Stand.framedata[1].sprite = "stand_0"
    -- and Ryu.json key_name = "SF3/Ryu"
    -- then the image key will be "SF3/Ryu/stand_0"
    _G.loadImageAsset("SF3/Ryu/stand_0", "Assets/images/SF3/Ryu/stand_0.png") 
    _G.loadImageAsset("SF3/Ken/stand_0", "Assets/images/SF3/Ken/stand_0.png") -- Example for Ken

    -- Stage background image (assuming Training.json refers to a sprite)
    -- If Training.json states.Stand.framedata[1].sprite = "TrainingBG" (hypothetical)
    -- and Training.json key_name = "Reencor/Training"
    -- then image key = "Reencor/Training/TrainingBG"
    -- However, stages might just have a direct image_key in their dict, e.g. "Reencor/TrainingBG"
    -- For now, let's assume the stage background is a simple image:
    _G.loadImageAsset("Reencor/TrainingBG", "Assets/images/Reencor/TrainingBG.png") -- Adjusted key

    -- General fallback/test image (original test image)
    _G.loadImageAsset("reencor_1", "Assets/images/reencor/1.png") 
    
    _G.loadSoundAsset("reencor_note", "Assets/sounds/reencor/nota.wav")
    print("--- Asset Loading Complete ---")

    -- Initialize Screen Manager
    screen_manager.load(TitleScreen, _G.GAME, screen_manager)

    -- Print connected joysticks
    print("\nConnected Joysticks (Initial Check in main.lua):")
    local joysticks = love.joystick.getJoysticks()
    if #joysticks == 0 then
        print("No joysticks initially detected.")
    else
        for i, joystick in ipairs(joysticks) do
            if joystick then
                print(string.format("  ID: %d, Name: %s, Buttons: %d, Axes: %d",
                                    joystick:getID(), joystick:getName(), joystick:getButtonCount(), joystick:getAxes()))
            else
                print(string.format("  Joystick %d is nil (potentially disconnected during startup?)", i))
            end
        end
    end
end

function love.update(dt)
    _G.GAME.current_fps = love.timer.getFPS()
    screen_manager.update(dt)
end

function love.draw()
    love.graphics.clear(0.1, 0.1, 0.1, 1) -- Dark grey background

    screen_manager.draw()

    -- Draw FPS counter
    love.graphics.setColor(0, 1, 0, 1) -- Green
    love.graphics.print("FPS: " .. _G.GAME.current_fps, 10, 10)
    love.graphics.setColor(1, 1, 1, 1) -- Reset color to white
end

function love.keypressed(key, scancode, isrepeat)
    screen_manager.keypressed(key, scancode, isrepeat)
end

function love.mousepressed(x, y, button, istouch, presses)
    screen_manager.mousepressed(x, y, button, istouch, presses)
end

function love.mousereleased(x, y, button, istouch, presses)
    screen_manager.mousereleased(x, y, button, istouch, presses)
end

function love.keyreleased(key) -- Added missing keyreleased
    screen_manager.keyreleased(key)
end

function love.gamepadpressed(joystick, button)
    screen_manager.gamepadpressed(joystick, button)
end

function love.gamepadreleased(joystick, button)
    screen_manager.gamepadreleased(joystick, button)
end

function love.gamepadaxis(joystick, axis, value)
    screen_manager.gamepadaxis(joystick, axis, value)
end

function love.joystickadded(joystick)
    print(string.format("Joystick ADDED globally: ID=%d, Name=%s", joystick:getID(), joystick:getName()))
    screen_manager.joystickadded(joystick)
end

function love.joystickremoved(joystick)
    print(string.format("Joystick REMOVED globally: ID=%d, Name=%s", joystick:getID(), joystick:getName()))
    screen_manager.joystickremoved(joystick)
end


-- Add other LÖVE callbacks as needed and forward them to the screen manager
-- love.wheelmoved(x,y)
-- love.focus(f)
-- love.quit()
-- love.resize(w,h)
-- love.visible(v)
-- love.mousemoved(x,y,dx,dy,istouch)
-- love.textinput(t)
-- love.directoryropped(path)
-- love.filedropped(file)
-- love.touchpressed(id, x, y, dx, dy, pressure)
-- love.touchreleased(id, x, y, dx, dy, pressure)
-- love.touchmoved(id, x, y, dx, dy, pressure)
-- love.lowmemory()
-- love.threaderror(thread, errorstr)
