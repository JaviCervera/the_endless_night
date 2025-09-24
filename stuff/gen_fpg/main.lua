import("src/_class.lua")
import("src/fpg.lua")
import("src/image.lua")
import("src/memblock_reader.lua")
import("src/memblock_writer.lua")
import("src/pal_image.lua")
import("src/pal64.lua")
import("src/pal256.lua")

function Main()
  SetDriver(DRIVER_SOFTWAREFAST)
  OpenScreen(604, 480, 0, 0)

  --FPG:Load("../../HORDES/TEXTURES.FPG")
  local pal64 = Pal64:Load("DIV.PAL")
  local imgs = LoadImages(ReadDir("../textures"))
  local pal_imgs = PalettizeImages(imgs, Pal256:Gen(pal64))
  local fpg = FPG:Create(pal64, pal_imgs)
  fpg:save("../../HORDES/TEXTURES.FPG")

  print("Done.")
end

function ReadDir(path)
  local contents = DirContents(path)
  local dir = {}
  for i = 3, SplitCount(contents, "\n") do
    dir[#dir + 1] = path .. "/" .. SplitIndex(contents, "\n", i)
  end
  return dir
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
