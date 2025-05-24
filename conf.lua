function love.conf(t)
    t.window.title = "LÖVE 2D Game"
    t.window.width = 640
    t.window.height = 400

    t.modules.graphics = true
    t.modules.audio = true
    t.modules.filesystem = true
    t.modules.event = true
    t.modules.timer = true
end
