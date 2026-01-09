-- Each pixel is an ARGB integer
Image = class()

function Image:Load(filename)
  self = Image:New()
  self.fname = StripDir(filename)
  self.rows = {}
  local img = LoadImage(filename)
  local colors = LoadImageColors(img)
  for y = 0, img.height - 1 do
    row = {}
    for x = 0, img.width - 1 do
      local color = Color()
      color.r = PeekByte(colors, (y * img.width + x) * 4)
      color.g = PeekByte(colors, (y * img.width + x) * 4 + 1)
      color.b = PeekByte(colors, (y * img.width + x) * 4 + 2)
      color.a = PeekByte(colors, (y * img.width + x) * 4 + 3)
      row[x + 1] = color
    end
    self.rows[#self.rows + 1] = row
  end
  UnloadImage(img)
  return self
end

function Image:filename()
  return self.fname
end

function Image:width()
  if #self.rows > 0 then
    return #self.rows[1]
  else
    return 0
  end
end

function Image:height()
  return #self.rows
end

function Image:color(x, y)
  return self.rows[y][x]
end
