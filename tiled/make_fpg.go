package main

import (
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/jpeg"
	"image/png"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strings"
)

var fpgMagic = [8]byte{'f', 'p', 'g', 0x1a, '\r', '\n', 0, 0}

type sourceImage struct {
	name string
	data *image.NRGBA
}

type palette struct {
	raw [256 * 3]byte
}

func main() {
	_, srcFile, _, _ := runtime.Caller(0)
	scriptDir := filepath.Dir(srcFile)
	palettePath := filepath.Join(scriptDir, "..", "assets", "div.pal")
	outputPath := filepath.Join(scriptDir, "..", "assets", "town.fpg")

	files, err := collectTileFiles(scriptDir)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error finding source tiles: %v\n", err)
		os.Exit(1)
	}

	pal, err := loadPalette(palettePath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error loading %s: %v\n", palettePath, err)
		os.Exit(1)
	}

	images := make([]sourceImage, 0, len(files))
	for i, name := range files {
		src, err := loadImage(filepath.Join(scriptDir, name))
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error loading %s: %v\n", name, err)
			os.Exit(1)
		}

		images = append(images, sourceImage{
			name: name,
			data: src,
		})
		fmt.Printf("  [%2d/%d] %s\n", i+1, len(files), name)
	}

	if err := writeFPG(outputPath, pal, images); err != nil {
		fmt.Fprintf(os.Stderr, "Error writing %s: %v\n", outputPath, err)
		os.Exit(1)
	}

	fmt.Printf("\nSaved %s\n", outputPath)
}

func collectTileFiles(dir string) ([]string, error) {
	entries, err := os.ReadDir(dir)
	if err != nil {
		return nil, err
	}

	var files []string
	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}
		name := entry.Name()
		lower := strings.ToLower(name)
		if lower == "tiles.png" {
			continue
		}
		if strings.HasSuffix(lower, ".jpg") || strings.HasSuffix(lower, ".jpeg") || strings.HasSuffix(lower, ".png") {
			files = append(files, name)
		}
	}

	sort.Strings(files)
	if len(files) == 0 {
		return nil, fmt.Errorf("no .jpg/.png tile images found")
	}
	return files, nil
}

func loadPalette(path string) (palette, error) {
	var pal palette

	f, err := os.Open(path)
	if err != nil {
		return pal, err
	}
	defer f.Close()

	var header [8]byte
	if _, err := f.Read(header[:]); err != nil {
		return pal, err
	}
	if _, err := f.Read(pal.raw[:]); err != nil {
		return pal, err
	}

	return pal, nil
}

func writeFPG(path string, pal palette, images []sourceImage) error {
	f, err := os.Create(path)
	if err != nil {
		return err
	}
	defer f.Close()

	if _, err := f.Write(fpgMagic[:]); err != nil {
		return err
	}
	if _, err := f.Write(pal.raw[:]); err != nil {
		return err
	}
	if _, err := f.Write(make([]byte, 16*36)); err != nil {
		return err
	}

	for i, img := range images {
		code := int32(i + 1)
		pixels := palettize(img.data, pal)
		desc := makeCString(img.name, 32)
		name := makeCString(strings.TrimSuffix(img.name, filepath.Ext(img.name)), 12)
		width := int32(img.data.Bounds().Dx())
		height := int32(img.data.Bounds().Dy())
		length := int32(64 + len(pixels))
		numPoints := int32(0)

		if err := binary.Write(f, binary.LittleEndian, code); err != nil {
			return err
		}
		if err := binary.Write(f, binary.LittleEndian, length); err != nil {
			return err
		}
		if _, err := f.Write(desc); err != nil {
			return err
		}
		if _, err := f.Write(name); err != nil {
			return err
		}
		if err := binary.Write(f, binary.LittleEndian, width); err != nil {
			return err
		}
		if err := binary.Write(f, binary.LittleEndian, height); err != nil {
			return err
		}
		if err := binary.Write(f, binary.LittleEndian, numPoints); err != nil {
			return err
		}
		if _, err := f.Write(pixels); err != nil {
			return err
		}
	}

	return nil
}

func loadImage(path string) (*image.NRGBA, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	var src image.Image
	lower := strings.ToLower(path)
	if strings.HasSuffix(lower, ".jpg") || strings.HasSuffix(lower, ".jpeg") {
		src, err = jpeg.Decode(f)
	} else {
		src, err = png.Decode(f)
	}
	if err != nil {
		return nil, err
	}

	b := src.Bounds()
	nrgba := image.NewNRGBA(image.Rect(0, 0, b.Dx(), b.Dy()))
	draw.Draw(nrgba, nrgba.Bounds(), src, b.Min, draw.Src)
	return nrgba, nil
}

func palettize(src *image.NRGBA, pal palette) []byte {
	w := src.Bounds().Dx()
	h := src.Bounds().Dy()
	pixels := make([]byte, w*h)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			pixels[y*w+x] = closestPaletteIndex(src.NRGBAAt(x, y), pal)
		}
	}
	return pixels
}

func closestPaletteIndex(c color.NRGBA, pal palette) byte {
	if c.A < 128 {
		return 0
	}

	bestIndex := 1
	bestDist := int(^uint(0) >> 1)
	for i := 1; i < 256; i++ {
		r := int(pal.raw[i*3]) * 4
		g := int(pal.raw[i*3+1]) * 4
		b := int(pal.raw[i*3+2]) * 4
		dr := r - int(c.R)
		dg := g - int(c.G)
		db := b - int(c.B)
		dist := dr*dr + dg*dg + db*db
		if dist < bestDist {
			bestDist = dist
			bestIndex = i
		}
	}
	return byte(bestIndex)
}

func makeCString(value string, size int) []byte {
	buf := make([]byte, size)
	copy(buf, []byte(value))
	return buf
}

