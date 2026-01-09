FPG = class()

function FPG:Create(pal64, pal_imgs)
  self = FPG:New()
  self.pal64 = pal64
  self.imgs = pal_imgs
  return self
end

-- This is actually not functional. Basically used now to dump some data about the FPG being loaded
function FPG:Load(filename)
  local reader = MemblockReader:Load(filename)
  
  -- Header
  local id = string.char(reader:readByte()) .. string.char(reader:readByte()) .. string.char(reader:readByte())
  local hex = reader:readInt()
  local ver = reader:readByte()
  print(id)
  print(hex)
  print(ver)
  Exit(0)

  -- Palette
  local pal64 = Pal64:Create()
  for i = 1, 256 do
    pal64:setRed(i, reader:readByte())
    pal64:setGreen(i, reader:readByte())
    pal64:setBlue(i, reader:readByte())
  end

  -- Color ranges
  for i = 1, 16 do
    for j = 1, 36 do
      pal64:setRange(i, j, reader:readByte())
    end
  end
  self.pal64 = pal64

  while not reader:eof() do
    print("Reading new map...")
    local code = reader:readInt()
    local length = reader:readInt() -- 64 + width * height
    local desc = reader:readFixedString(32)
    local name = reader:readFixedString(12)
    local width = reader:readInt()
    local height = reader:readInt()
    local num_points = reader:readInt()
    print(code)
    print(length)
    print(desc)
    print(name)
    print(width)
    print(height)
    print(num_points)
    for p = 1, num_points do
      local px = reader:readShort()
      local py = reader:readShort()
      print(px .. " " .. py)
    end
    for y = 1, height do
      for x = 1, width do
        reader:readByte()
      end
    end
    print("")
  end

  reader:free()
end

function FPG:save(filename)
  local writer = MemblockWriter:Create(self:_memblockSize())
  
  -- Header
  writer:writeByte(string.byte("f"))
  writer:writeByte(string.byte("p"))
  writer:writeByte(string.byte("g"))
  writer:writeInt(658714)
  writer:writeByte(0)

  -- Palette
  for i = 1, 256 do
    writer:writeByte(self.pal64:red(i))
    writer:writeByte(self.pal64:green(i))
    writer:writeByte(self.pal64:blue(i))
  end

  -- Color ranges
  for i = 1, 16 do
    for j = 1, 36 do
      writer:writeByte(self.pal64:range(i, j))
    end
  end

  -- Maps
  for i, img in ipairs(self.imgs) do
    writer:writeInt(i) -- Code
    writer:writeInt(64 + img:width() * img:height()) -- Length
    writer:writeFixedString(string.sub(img:filename(), -31), 31) -- Description
    writer:writeByte(0) -- Zero terminator for description
    writer:writeFixedString(string.sub(img:filename(), -12), 12) -- Filename
    writer:writeInt(img:width()) -- Width
    writer:writeInt(img:height()) -- Height
    writer:writeInt(0) -- Num control points

    -- Pixels
    for y = 1, img:height() do
      for x = 1, img:width() do
        writer:writeByte(img:index(x, y) - 1)
      end
    end
  end

  SaveMemblock(writer.memblock, filename)
  writer:free()
end

function FPG:_memblockSize()
  local size = 1352
  for i = 1, #self.imgs do
    size = size + self:_mapSize(i)
  end
  return size
end

function FPG:_mapSize(img_index)
  return 64 + (self.imgs[img_index]:width() * self.imgs[img_index]:height())
end
