local MenuItem = {}
MenuItem.__index = MenuItem

function MenuItem:new(params)
    local self = setmetatable({}, MenuItem)

    self.text = params.text or "Menu Item"
    self.x = params.x or 0
    self.y = params.y or 0
    self.action = params.action -- Can be nil

    self.is_selected = false
    self.font = params.font or love.graphics.getFont() -- Use current font or a specific one
    self.selected_color = params.selected_color or {0, 1, 0, 1} -- Green
    self.default_color = params.default_color or {1, 1, 1, 1} -- White
    self.selected_indicator = params.selected_indicator or "> "
    self.deselected_indicator = params.deselected_indicator or "  "
    
    return self
end

function MenuItem:update(dt)
    -- Nothing to update for this simple version
end

function MenuItem:draw()
    local current_color = self.default_color
    local current_indicator = self.deselected_indicator

    if self.is_selected then
        current_color = self.selected_color
        current_indicator = self.selected_indicator
    end

    love.graphics.setColor(current_color[1], current_color[2], current_color[3], current_color[4])
    love.graphics.setFont(self.font)
    love.graphics.print(current_indicator .. self.text, self.x, self.y)
    love.graphics.setColor(1, 1, 1, 1) -- Reset to white
end

function MenuItem:on_select()
    self.is_selected = true
    -- print(self.text .. " selected")
end

function MenuItem:on_deselect()
    self.is_selected = false
    -- print(self.text .. " deselected")
end

function MenuItem:activate()
    if self.action then
        print("Activating action for: " .. self.text)
        self.action()
    else
        print("No action defined for: " .. self.text)
    end
end

return MenuItem
