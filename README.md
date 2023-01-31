# CHIP-8 Interpreter

an interpreter for the CHIP-8 programming language written in C++. uses SDL for input/graphics
<br><br>
CHIP-8 was first introduced in the 1970s. many famous clasic games like Tetris, Pac-Man, Pong, etc. were ported to it
<br><br>
this is an interpreter and not an emulator because the CHIP-8 isn't a physical machine being emulated, but rather a virutal machine running CHIP-8 programs
<br><br>



### **usage:**
```console
./chip8 [screen-scale-factor] <path-to-ROM>
```
<br><br>



### **prerequisite libraries:**

SDL2
<br>
&emsp; installation:
```console
  Debian/Ubuntu:  sudo apt-get install libsdl2-dev -y
  
  or check out the SDL2 wiki's Installation section:
    https://wiki.libsdl.org/SDL2/Installation        
```
<br>

CMake
<br>
&emsp; installation:
```console
  Debian/Ubuntu:  sudo apt-get install cmake
  
  or download directly from their website:
    https://cmake.org/download/
```
<br>

### **setup:**
&emsp; to install the repository 
```
  git clone "https://github.com/sameersaeed/chip8-interpreter"
```
<br>

&emsp; building (starting from project root)
```console
  mkdir build && cd build
  cmake ..
  make
  ./chip8 [scale] ../roms/<ROM>
```
<br><br>



### **sample installation / run:**
```console
                          ...#  git clone "https://github.com/sameersaeed/chip8-interpreter" && cd chip8-interpreter
        .../chip8-interpreter#  mkdir build && cd build
  .../chip8-interpreter/build#  cmake .. 
  .../chip8-interpreter/build#  make
  .../chip8-interpreter/build#  ./chip8 3 ../roms/Maze.ch8        <-- creates a 3 * (64 x 32px) = 192 x 96px display
```
<br><br>



### references:
 - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 - http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf
