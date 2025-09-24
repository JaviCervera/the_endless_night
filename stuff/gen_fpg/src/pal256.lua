Pal256 = class()

function Pal256:Gen(pal64)
  self = Pal256:New()
  self.data = {}
  local mul = 4.047619047619048
  for i = 1, 256 do
    local color = pal64[i]
    self.data[i] = {
      Int(Clamp(pal64:red(i) * mul, 0, 255)),
      Int(Clamp(pal64:green(i) * mul, 0, 255)),
      Int(Clamp(pal64:blue(i) * mul, 0, 255)),
    }
  end
  return self
end

function Pal256:red(index)
  return self.data[index][1]
end

function Pal256:green(index)
  return self.data[index][2]
end

function Pal256:blue(index)
  return self.data[index][3]
end
