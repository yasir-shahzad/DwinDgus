# DwinDgus

An Arduino library for interfacing with DWIN DGUS serial displays.

## Features
- Send and receive data to/from DWIN DGUS serial displays
- Page switching, value setting, and display control
- Supports Arduino and ESP32 boards

## Installation
1. Download or clone this repository.
2. Copy the `DwinDgus` folder to your Arduino `libraries` directory.

## Usage Example
```cpp
#include <DwinDgus.h>

void setup() {
  rtscheck.RTS_Init();
}

void loop() {
  RTSUpdate();
  // Add your code to interact with the display
}
```

## Author
Yasir Shahzad (<yasirshahzad918@gmail.com>)

## License
MIT License