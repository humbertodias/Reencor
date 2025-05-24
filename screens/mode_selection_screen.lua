local MenuItem = require("menu_item")

local ModeSelectionScreen = {}
ModeSelectionScreen.__index = ModeSelectionScreen

function ModeSelectionScreen:new(game_ref, screen_manager_ref)
    local instance = setmetatable({}, ModeSelectionScreen)
    instance.game = game_ref
    instance.screen_manager = screen_manager_ref
    
    instance.menu_items = {}
    instance.current_selection_index = 1
    instance.font = love.graphics.newFont(24) -- Specific font for this screen

    local modes = {
        {text = "Single Player", action = function() 
            print("Selected Single Player - Not implemented yet") 
            -- Example: self.screen_manager:switch(require("screens.single_player_setup_screen"), self.game, self.screen_manager)
        end},
        {text = "Multi Player", action = function() 
            print("Selected Multi Player - Not implemented yet") 
        end},
        {text = "Training", action = function() 
            print("Selected Training - Switching to GameScreen")
            instance.screen_manager:switch(require("screens.game_screen"), instance.game, instance.screen_manager) 
        end},
        {text = "Options", action = function() 
            print("Selected Options - Not implemented yet") 
        end},
        {text = "Back to Title", action = function()
            print("Selected Back to Title - Switching to TitleScreen")
            instance.screen_manager:switch(require("screens.title_screen"), instance.game, instance.screen_manager)
        end}
    }

    local start_y = instance.game.resolution.height / 2 - (#modes * 30) / 2
    for i, mode_data in ipairs(modes) do
        local item = MenuItem:new({
            text = mode_data.text,
            x = instance.game.resolution.width / 2 - 100,
            y = start_y + (i - 1) * 30,
            action = mode_data.action,
            font = instance.font
        })
        table.insert(instance.menu_items, item)
    end

    if #instance.menu_items > 0 then
        instance.menu_items[instance.current_selection_index]:on_select()
    end
    
    print("ModeSelectionScreen created")
    return instance
end

function ModeSelectionScreen:load()
    print("ModeSelectionScreen loaded")
    -- Reset selection if needed, or if items could change dynamically
    if #self.menu_items > 0 then
        if self.current_selection_index > #self.menu_items then
            self.current_selection_index = 1
        end
        for i, item in ipairs(self.menu_items) do
            item:on_deselect()
        end
        self.menu_items[self.current_selection_index]:on_select()
    end
end

function ModeSelectionScreen:update(dt)
    -- No specific update logic for menu items themselves in this version
end

function ModeSelectionScreen:draw()
    love.graphics.setFont(self.font) -- Set a default larger font for the title
    love.graphics.printf("Select Mode", 0, 50, self.game.resolution.width, "center")

    for _, item in ipairs(self.menu_items) do
        item:draw()
    end
end

function ModeSelectionScreen:keypressed(key, scancode, isrepeat)
    if #self.menu_items == 0 then return end

    local old_selection_index = self.current_selection_index

    if key == "up" then
        self.current_selection_index = self.current_selection_index - 1
        if self.current_selection_index < 1 then
            self.current_selection_index = #self.menu_items
        end
    elseif key == "down" then
        self.current_selection_index = self.current_selection_index + 1
        if self.current_selection_index > #self.menu_items then
            self.current_selection_index = 1
        end
    elseif key == "return" or key == "kpenter" or key == "z" then -- 'z' as an alternative confirm
        self.menu_items[self.current_selection_index]:activate()
        return -- Action might change screen, so exit early
    elseif key == "escape" then -- Go back to Title screen
        self.screen_manager:switch(require("screens.title_screen"), self.game, self.screen_manager)
        return
    end

    if old_selection_index ~= self.current_selection_index then
        self.menu_items[old_selection_index]:on_deselect()
        self.menu_items[self.current_selection_index]:on_select()
    end
end

function ModeSelectionScreen:unload()
    print("ModeSelectionScreen unloaded")
    -- Deselect all items when screen is left, for a clean state if returned to.
    for _, item in ipairs(self.menu_items) do
        item:on_deselect()
    end
end

return ModeSelectionScreen
