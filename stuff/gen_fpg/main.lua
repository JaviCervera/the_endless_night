-- This tool is meant to be executed with https://github.com/JaviCervera/hal
-- Usage: Run "hal" from this dir (make sure that hal is compiled and _build dir added to PATH)

import("src/_class.lua")
import("src/fpg.lua")
import("src/image.lua")
import("src/memblock_reader.lua")
import("src/memblock_writer.lua")
import("src/pal_image.lua")
import("src/pal64.lua")
import("src/pal256.lua")
import("src/util.lua")

function Main()
  InitWindow(640, 480, "")

  --FPG:Load("../../ENDLESS/TEXTURES.FPG")
  local pal64 = Pal64:Load("DIV.PAL")
  local imgs = LoadImages(ReadDir("../textures"))
  local pal_imgs = PalettizeImages(imgs, Pal256:Gen(pal64))
  local fpg = FPG:Create(pal64, pal_imgs)
  fpg:save("../../ENDLESS/TEXTURES.FPG")

  print("Done.")
end

function ReadDir(path)
  local contents = Split(DirContents(path), "\n")
  table.sort(contents)
  for i, v in ipairs(contents) do
    contents[i] = path .. "/" .. v
  end
  return contents
end

function LoadImages(filenames)
  local imgs = {}
  for i, filename in ipairs(filenames) do
    print("Loading image " .. i .. ": '" .. filename .. "' ...")
    imgs[i] = Image:Load(filename)
  end
  return imgs
end

function PalettizeImages(imgs, pal256)
  local pal_imgs = {}
  for i, img in ipairs(imgs) do
    print("Palettizing " .. i .. ": '" .. img:filename() .. "' ...")
    pal_imgs[i] = PalImage:Create(img, pal256)
  end
  return pal_imgs
end

Main()
