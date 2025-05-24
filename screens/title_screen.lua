local TitleScreen = {}
TitleScreen.__index = TitleScreen

function TitleScreen:new(game_ref, screen_manager_ref)
    local instance = setmetatable({}, TitleScreen)
    instance.game = game_ref -- Reference to the main GAME table
    instance.screen_manager = screen_manager_ref -- Reference to the screen manager
    instance.font = love.graphics.newFont(30) -- Basic font
    return instance
end

function TitleScreen:load()
    -- Optional: Load assets specific to this screen
    print("TitleScreen loaded")
end

function TitleScreen:update(dt)
    -- Optional: Update logic for the title screen (e.g., animations)
end

function TitleScreen:draw()
    love.graphics.setFont(self.font)
    love.graphics.printf("Title Screen", 0, self.game.resolution.height / 2 - 30, self.game.resolution.width, "center")
    love.graphics.printf("Press Enter to Start", 0, self.game.resolution.height / 2 + 10, self.game.resolution.width, "center")
end

function TitleScreen:keypressed(key, scancode, isrepeat)
    if key == "return" or key == "kpenter" then
        print("Enter key pressed on TitleScreen, switching to ModeSelectionScreen")
        local ModeSelectionScreen = require("screens.mode_selection_screen")
        self.screen_manager.switch(ModeSelectionScreen) -- Pass game and screen_manager refs
    end
end

function TitleScreen:unload()
    print("TitleScreen unloaded")
end

return TitleScreen
