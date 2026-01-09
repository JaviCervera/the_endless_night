function Clamp(number, min, max)
  return math.min(math.max(number, min), max)
end

function Split(str, sep)
  sep = sep or "%s"
  local result = {}
  for s in string.gmatch(str, "([^" .. sep .. "]+)") do
    table.insert(result, s)
  end
  return result
end

function StripDir(path)
  return path:match("([^/\\]+)$")
end
