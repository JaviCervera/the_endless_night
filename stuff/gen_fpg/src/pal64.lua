Pal64 = class()

function Pal64:Create()
  self = Pal64:New()
  self.data = {}
  for i = 1, 256 do
    self.data[i] = {0, 0, 0}
  end
  self.ranges = {}
  for i = 1, 16 do
    self.ranges[i] = {}
    for j = 1, 36 do
      -- TODO This is not valid!!!
      self.ranges[i][j] = 0
    end
  end
  return self
end

function Pal64:Load(filename)
  self = Pal64:Create()

  local r = MemblockReader:Load(filename)
  
  -- Header
  local id = string.char(r:readByte()) .. string.char(r:readByte()) .. string.char(r:readByte())
  local hex = r:readInt()
  local ver = r:readByte()

  for i = 1, 256 do
    self:setRed(i, r:readByte())
    self:setGreen(i, r:readByte())
    self:setBlue(i, r:readByte())
  end

  -- Color ranges
  for i = 1, 16 do
    for j = 1, 36 do
      self:setRange(i, j, r:readByte())
    end
  end

  r:free()

  return self
end

function Pal64:red(index)
  return self.data[index][1]
end

function Pal64:green(index)
  return self.data[index][2]
end

function Pal64:blue(index)
  return self.data[index][3]
end

function Pal64:range(range_index, color_index)
  return self.ranges[range_index][color_index]
end

function Pal64:setRed(index, r)
  self.data[index][1] = math.floor(Clamp(r, 0, 63))
end

function Pal64:setGreen(index, g)
  self.data[index][2] = math.floor(Clamp(g, 0, 63))
end

function Pal64:setBlue(index, b)
  self.data[index][3] = math.floor(Clamp(b, 0, 63))
end

function Pal64:setRange(range_index, color_index, color_value)
  self.ranges[range_index][color_index] = color_value
end
