package main

import (
	"encoding/binary"
	"encoding/xml"
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"strings"
)

type tmxMap struct {
	Width  int      `xml:"width,attr"`
	Height int      `xml:"height,attr"`
	Layers []tmxLayer `xml:"layer"`
}

type tmxLayer struct {
	Name string  `xml:"name,attr"`
	Data tmxData `xml:"data"`
}

type tmxData struct {
	Encoding string `xml:"encoding,attr"`
	Text     string `xml:",chardata"`
}

func parseCSVLayer(text string) []int {
	var values []int
	for _, token := range strings.FieldsFunc(text, func(r rune) bool {
		return r == ',' || r == '\n' || r == '\r'
	}) {
		token = strings.TrimSpace(token)
		if token == "" {
			continue
		}
		var v int
		fmt.Sscanf(token, "%d", &v)
		values = append(values, v)
	}
	return values
}

func main() {
	_, srcFile, _, _ := runtime.Caller(0)
	scriptDir := filepath.Dir(srcFile)

	tmxPath := filepath.Join(scriptDir, "town.tmx")
	outPath := filepath.Join(scriptDir, "..", "assets", "town.tma")

	data, err := os.ReadFile(tmxPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error reading %s: %v\n", tmxPath, err)
		os.Exit(1)
	}

	var m tmxMap
	if err := xml.Unmarshal(data, &m); err != nil {
		fmt.Fprintf(os.Stderr, "Error parsing TMX: %v\n", err)
		os.Exit(1)
	}

	size := m.Width * m.Height
	layers := map[string][]int{
		"Floor":    make([]int, size),
		"Walls":    make([]int, size),
		"Entities": make([]int, size),
	}

	for _, layer := range m.Layers {
		if layer.Data.Encoding != "csv" {
			fmt.Fprintf(os.Stderr, "Warning: layer %q is not CSV-encoded, skipping\n", layer.Name)
			continue
		}
		if _, ok := layers[layer.Name]; ok {
			layers[layer.Name] = parseCSVLayer(layer.Data.Text)
		}
	}

	f, err := os.Create(outPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error creating output file: %v\n", err)
		os.Exit(1)
	}
	defer f.Close()

	binary.Write(f, binary.LittleEndian, uint32(m.Width))
	binary.Write(f, binary.LittleEndian, uint32(m.Height))

	for _, name := range []string{"Floor", "Walls", "Entities"} {
		data := layers[name]
		for row := m.Height - 1; row >= 0; row-- {
			for col := 0; col < m.Width; col++ {
				f.Write([]byte{byte(data[row*m.Width+col])})
			}
		}
	}

	fmt.Printf("Exported %dx%d map to %s\n", m.Width, m.Height, outPath)
}
