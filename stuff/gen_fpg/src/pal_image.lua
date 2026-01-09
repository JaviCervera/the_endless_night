-- Each pixel is a byte with the index (1...256)
PalImage = class()

function PalImage:Create(img, pal256)
  self = PalImage:New()
  self.fname = img:filename()
  self.rows = {}
  for y = 1, img:height() do
    self.rows[y] = {}
    for x = 1, img:width() do
      local color = img:color(x, y)
      self.rows[y][x] = PalImage:FindIndex(color.r, color.g, color.b, pal256)
    end
  end
  return self
end

function PalImage:FindIndex(r, g, b, pal256)
  local min_distance = math.huge
  local closest_index = 0
  for i = 1, 256 do
    local distance =
      (pal256:red(i) - r)^2 +
      (pal256:green(i) - g)^2 +
      (pal256:blue(i) - b)^2
    if distance < min_distance then
      min_distance = distance
      closest_index = i
    end
  end
  return closest_index
end

function PalImage:filename()
  return self.fname
end

function PalImage:width()
  if #self.rows > 0 then
    return #self.rows[1]
  else
    return 0
  end
end

function PalImage:height()
  return #self.rows
end

function PalImage:index(x, y)
  return self.rows[y][x]
end
