MemblockWriter = class()

function MemblockWriter:Create(size)
  self = MemblockWriter:New()
  self.memblock = CreateMemblock(size)
  self.offset = 0
  return self
end

function MemblockWriter:free()
  FreeMemblock(self.memblock)
  self.memblock = nil
  self.offset = 0
end

function MemblockWriter:writeByte(v)
  PokeByte(self.memblock, self.offset, v)
  self.offset = self.offset + 1
end

function MemblockWriter:writeShort(v)
  PokeShort(self.memblock, self.offset, v)
  self.offset = self.offset + 2
end

function MemblockWriter:writeInt(v)
  PokeInt(self.memblock, self.offset, v)
  self.offset = self.offset + 4
end

function MemblockWriter:writeFixedString(str, size)
  local max = math.min(str:len(), size)
  for i = 1, max do
    self:writeByte(string.byte(string.sub(str, i, i)))
  end
  for i = str:len() + 1, size do
    self:writeByte(0)
  end
end

function MemblockWriter:eof()
  return self.offset >= MemblockSize(self.memblock)
end
