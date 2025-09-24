__class = {}
__class.__index = __class

function __class:New()
  local obj = {}
  setmetatable(obj, self)
  return obj
end

function class(parent)
  if parent == nil then parent = __class end
  local c = {}
  setmetatable(c, parent)
  c.__index = c
  return c
end
