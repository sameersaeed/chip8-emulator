# CHIP-8 Interpreter

usage:
```
./chip8 [screen-scale-factor] <path-to-ROM>
```




build instructions:
```
  git clone "https://github.com/sameersaeed/chip8-interpreter"
```
  if you do not have the SDL2 library installed:
```
  Debian/Ubuntu:  sudo apt-get install libsdl2-dev -y
  
  or see the SDL2 wiki:
    https://wiki.libsdl.org/SDL2/Installation        
```

  (within project directory)
```
  mkdir build && cd build
  cmake ..
  make
  ./chip8 [scale] ../roms/<ROM>
```





  sample installation / run:
```
                         ~/#  git clone "https://github.com/sameersaeed/chip8-interpreter" && cd chip8-interpreter
        ~/chip8-interpreter#  mkdir build && cd build
  ~/chip8-interpreter/build#  cmake .. && make
  ~/chip8-interpreter/build#  ./chip8 3 ../roms/Maze.ch8        <-- creates a 3 * (64 x 32px) = 192 x 96px display
```

references:
 - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 - http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf
