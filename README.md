# Sorting Visualizer

A C++ sorting algorithm visualizer using SDL2.

## Features
- Visualizes Bubble, Selection, Insertion, Merge, and Quick Sort
- Color highlights for comparisons, swaps, and sorted elements
- User controls for algorithm, speed, shuffle, and pause

## Controls
- `SPACE` : Start/Pause sorting
- `R`     : Reset (sorted array)
- `S`     : Shuffle (randomize array)
- `LEFT/RIGHT` : Previous/Next algorithm
- `UP/DOWN` : Increase/Decrease speed
- `P`     : Pause/Resume
- `ESC`   : Quit

## Build Instructions

### Prerequisites
- C++ compiler (e.g., g++/MinGW)
- [SDL2 development libraries](https://www.libsdl.org/download-2.0.php)

### Windows (MinGW Example)
1. Download and extract SDL2 (e.g., to `C:/SDL2`)
2. Build:
   ```sh
   g++ -IC:/SDL2/include -LC:/SDL2/lib -lSDL2 SortingVisualizer.cpp -o SortingVisualizer.exe
   ```
3. Copy `SDL2.dll` from `C:/SDL2/lib` or `C:/SDL2/bin` to your project folder
4. Run `SortingVisualizer.exe`

### Linux
1. Install SDL2 (`sudo apt install libsdl2-dev`)
2. Build:
   ```sh
   g++ -lSDL2 SortingVisualizer.cpp -o SortingVisualizer
   ```
3. Run with `./SortingVisualizer`

### macOS
1. Install SDL2 (`brew install sdl2`)
2. Build:
   ```sh
   g++ -F/Library/Frameworks -framework SDL2 SortingVisualizer.cpp -o SortingVisualizer
   ```
3. Run with `./SortingVisualizer`

## License
MIT
