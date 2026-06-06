package main

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/jpeg"
	"image/png"
	"math"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strings"
)

const (
	tileW    = 32
	tileH    = 32
	cols     = 8
	rows     = 4
	maxTiles = cols * rows
)

func main() {
	_, srcFile, _, _ := runtime.Caller(0)
	dir := filepath.Dir(srcFile)

	entries, err := os.ReadDir(dir)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading directory: %v\n", err)
		os.Exit(1)
	}

	var files []string
	for _, e := range entries {
		if e.IsDir() {
			continue
		}
		name := e.Name()
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
		fmt.Fprintln(os.Stderr, "No image files found.")
		os.Exit(1)
	}

	if len(files) > maxTiles {
		fmt.Printf("Warning: found %d files; only the first %d will be used.\n", len(files), maxTiles)
		files = files[:maxTiles]
	}

	sheet := image.NewNRGBA(image.Rect(0, 0, cols*tileW, rows*tileH))

	for i, name := range files {
		col := i % cols
		row := i / cols

		src, err := loadImage(filepath.Join(dir, name))
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error loading %s: %v\n", name, err)
			os.Exit(1)
		}

		tile := resizeBilinear(src, tileW, tileH)
		draw.Draw(sheet,
			image.Rect(col*tileW, row*tileH, (col+1)*tileW, (row+1)*tileH),
			tile, image.Point{}, draw.Src)

		fmt.Printf("  [%2d/%d] %s -> tile (%d, %d)\n", i+1, len(files), name, col, row)
	}

	outPath := filepath.Join(dir, "tiles.png")
	f, err := os.Create(outPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error creating tiles.png: %v\n", err)
		os.Exit(1)
	}
	defer f.Close()

	if err := png.Encode(f, sheet); err != nil {
		fmt.Fprintf(os.Stderr, "Error encoding PNG: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("\nSaved %s  (%d tile(s) in a %dx%d sheet)\n", outPath, len(files), cols, rows)
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

	// Convert to NRGBA for uniform pixel access
	b := src.Bounds()
	nrgba := image.NewNRGBA(image.Rect(0, 0, b.Dx(), b.Dy()))
	draw.Draw(nrgba, nrgba.Bounds(), src, b.Min, draw.Src)
	return nrgba, nil
}

func resizeBilinear(src *image.NRGBA, newW, newH int) *image.NRGBA {
	srcW := src.Bounds().Dx()
	srcH := src.Bounds().Dy()
	dst := image.NewNRGBA(image.Rect(0, 0, newW, newH))

	for y := 0; y < newH; y++ {
		for x := 0; x < newW; x++ {
			// Map destination pixel centre to source space
			sx := (float64(x)+0.5)*float64(srcW)/float64(newW) - 0.5
			sy := (float64(y)+0.5)*float64(srcH)/float64(newH) - 0.5

			x0 := int(math.Floor(sx))
			y0 := int(math.Floor(sy))
			fx := sx - float64(x0)
			fy := sy - float64(y0)

			c00 := src.NRGBAAt(clamp(x0, 0, srcW-1), clamp(y0, 0, srcH-1))
			c10 := src.NRGBAAt(clamp(x0+1, 0, srcW-1), clamp(y0, 0, srcH-1))
			c01 := src.NRGBAAt(clamp(x0, 0, srcW-1), clamp(y0+1, 0, srcH-1))
			c11 := src.NRGBAAt(clamp(x0+1, 0, srcW-1), clamp(y0+1, 0, srcH-1))

			dst.SetNRGBA(x, y, color.NRGBA{
				R: uint8(bilerp(float64(c00.R), float64(c10.R), float64(c01.R), float64(c11.R), fx, fy)),
				G: uint8(bilerp(float64(c00.G), float64(c10.G), float64(c01.G), float64(c11.G), fx, fy)),
				B: uint8(bilerp(float64(c00.B), float64(c10.B), float64(c01.B), float64(c11.B), fx, fy)),
				A: uint8(bilerp(float64(c00.A), float64(c10.A), float64(c01.A), float64(c11.A), fx, fy)),
			})
		}
	}
	return dst
}

func bilerp(c00, c10, c01, c11, fx, fy float64) float64 {
	return (c00*(1-fx)+c10*fx)*(1-fy) + (c01*(1-fx)+c11*fx)*fy
}

func clamp(v, lo, hi int) int {
	if v < lo {
		return lo
	}
	if v > hi {
		return hi
	}
	return v
}
