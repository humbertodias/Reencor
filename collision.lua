local Collision = {}

--- Checks for Axis-Aligned Bounding Box (AABB) collision between two boxes.
-- @param box1 A table with x, y, w, h representing its absolute screen coordinates and dimensions.
-- @param box2 A table with x, y, w, h representing its absolute screen coordinates and dimensions.
-- @return true if the boxes are overlapping, false otherwise.
function Collision.check_aabb(box1, box2)
    if not box1 or not box2 then
        -- print("Collision.check_aabb: Warning - one or both boxes are nil.")
        return false
    end
    
    -- Ensure all required fields are present
    if not (box1.x and box1.y and box1.w and box1.h and
            box2.x and box2.y and box2.w and box2.h) then
        -- print("Collision.check_aabb: Warning - one or both boxes are missing x, y, w, or h properties.")
        return false
    end

    return box1.x < box2.x + box2.w and
           box1.x + box1.w > box2.x and
           box1.y < box2.y + box2.h and
           box1.y + box1.h > box2.y
end

return Collision
