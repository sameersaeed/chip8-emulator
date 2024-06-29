# CHIP-8

a basic chip8 emulator written in C++. uses SDL for input/graphics
<br>
**note:** all of the roms in this repository are public domain
<br><br>



### **prerequisite libraries:**

SDL2 installation:
```console
Debian/Ubuntu:  sudo apt-get install libsdl2-dev -y
  
or check out the SDL2 wiki's **Installation** section:
  https://wiki.libsdl.org/SDL2/Installation        
```
<br>

CMake installation:
```console
Debian/Ubuntu:  sudo apt-get install cmake
  
or download directly from their website:
  https://cmake.org/download/
```
<br><br>



### **setup:**
&emsp; to install this repository:
```
git clone "https://github.com/sameersaeed/chip8-interpreter"
```
<br>

&emsp; building (starting from project root):
```console
mkdir build && cd build
cmake ..
make
./chip8 <scale> ../roms/<ROM-name>.ch8
```
<br><br>



### **sample installation + run:**
```console
git clone "https://github.com/sameersaeed/chip8-interpreter" 
cd chip8-interpreter
mkdir build && cd build
cmake .. 
make
./chip8 3 ../roms/Maze.ch8
```
a scale factor of 3 creates a 192 x 96px display (3 * (64 x 32px) = 192 x 96px)
<br><br>



### references:
 - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM                                (opcodes)
 - https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/  (0xDxyn implementation)
 - https://en.wikipedia.org/wiki/CHIP-8                                            (general info)