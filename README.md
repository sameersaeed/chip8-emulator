# CHIP-8 Interpreter

an interpreter for the CHIP-8 programming language written in C++. uses SDL for graphics
<br><br>



### **usage:**
```console
./chip8 [screen-scale-factor] <path-to-ROM>
```
<br><br>



### **setup:**
```
  git clone "https://github.com/sameersaeed/chip8-interpreter"
```
<br>

&emsp; if you do not have the SDL2 library installed:
```console
  Debian/Ubuntu:  sudo apt-get install libsdl2-dev -y
  
  or see the SDL2 wiki:
    https://wiki.libsdl.org/SDL2/Installation        
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
                         ~/#  git clone "https://github.com/sameersaeed/chip8-interpreter" && cd chip8-interpreter
        ~/chip8-interpreter#  mkdir build && cd build
  ~/chip8-interpreter/build#  cmake .. && make
  ~/chip8-interpreter/build#  ./chip8 3 ../roms/Maze.ch8        <-- creates a 3 * (64 x 32px) = 192 x 96px display
```
<br><br>



### references:
 - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 - http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf
