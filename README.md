# Chip8 Emulator #

[Chip8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C using the SDL2 library. 

### Compiling and running (Ubuntu 16.04) ###
```
sudo apt install build-essential libsdl2-dev libsdl2-mixer-dev
git clone https://github.com/aroess/chip8-emulator
cd chip8-emulator
make -B
./chip8 roms/BREAKOUT
```

### Key mapping ###

```
|1|2|3|C|  =>  |1|2|3|4|
|4|5|6|D|  =>  |Q|W|E|R|
|7|8|9|E|  =>  |A|S|D|F|
|A|0|B|F|  =>  |Z|X|C|V|
```

```ESC``` = Quit

```p``` = Toggle "halt after every opcode execution"

### Accuracy ###
It is very difficult to actually verify if the games are running correctly or not because there is no reference implementation for Chip8. If a game doesn't work correctly it could be that

* the implementation of the emulator is faulty
* the ROM depends on a different behavior of the emulator
* the ROM itself is buggy

Most of the interpreters I tested didn't get the screen wrapping right and completely bugged out running ```UFO```. The game ```BLITZ``` on the other hand seems to assume that there is no screen wrapping on the y-axis. It will therefore not work with my implementation unless you turn off vertical screen wrapping (see below). I tried my best to confirm that at least every included ROM (excluding ```BLITZ```) works as intended but this is - of course - not guaranteed.

### Screenshot ###
![screenshot](https://raw.githubusercontent.com/aroess/chip8-emulator/master/screenshot.png)


### Options ###

#### Turn debugger on ####
```diff
execute.h:
- define DEBUG 0
+ define DEBUG 1
```

#### Change frequency ####
```diff
main.c:
- unsigned short Hz = 500;
+ unsigned short Hz = *VALUE*;
```

#### Change key mapping ####
```diff
main.c:
- case SDLK_1    : CH8.key[  1] = 1; break;
+ case SDLK_*KEY*: CH8.key[  1] = 1; break;
```

[SDL Keycode Lookup Table](https://wiki.libsdl.org/SDLKeycodeLookup)

#### Turn off screen wrapping on y-axis ####
```diff
execute.c:
- #define GFX_PIXEL_INDEX (64 * ((VY + line) & 0x1F) + ((VX + bit_index) & 0x3F)) 
+ #define GFX_PIXEL_INDEX (64 * (VY + line) + ((VX + bit_index) & 0x3F)) 
```

#### Change display color to green ####
```diff
sdl_func.c
- unsigned char rgba[4] = {255, 255, 255, 255};
+ unsigned char rgba[4] = {255,   0, 255,   0};
```
