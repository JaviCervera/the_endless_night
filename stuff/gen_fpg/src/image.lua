-- Each pixel is an ARGB integer
Image = class()

function Image:Load(filename)
  self = Image:New()
  self.fname = StripDir(filename)
  self.rows = {}
  local pixmap = LoadPixmap(filename)
  for y = 1, PixmapHeight(pixmap) do
    row = {}
    for x = 1, PixmapWidth(pixmap) do
      row[x] = ReadPixel(pixmap, x - 1, y - 1)
    end
    self.rows[#self.rows + 1] = row
  end
  FreePixmap(pixmap)
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
