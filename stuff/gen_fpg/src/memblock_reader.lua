MemblockReader = class()

function MemblockReader:Load(filename)
  self = MemblockReader:New()
  self.memblock = LoadMemblock(filename)
  self.offset = 0
  return self
end

function MemblockReader:free()
  FreeMemblock(self.memblock)
  self.memblock = nil
  self.offset = 0
end

function MemblockReader:readByte()
  local v = PeekByte(self.memblock, self.offset)
  self.offset = self.offset + 1
  return v
end

function MemblockReader:readShort()
  local v = PeekShort(self.memblock, self.offset)
  self.offset = self.offset + 2
  return v
end

function MemblockReader:readInt()
  local v = PeekInt(self.memblock, self.offset)
  self.offset = self.offset + 4
  return v
end

function MemblockReader:readFixedString(size)
  local str = ""
  for i = 1, size do
    local code = self:readByte()
    if code ~= 0 then str = str .. Char(code) end
  end
  return str
end

function MemblockReader:eof()
  return self.offset >= MemblockSize(self.memblock)
end
