# Introduction

This project implements functionality to read stats, front sprites, Trainers' party/PC boxes and other various properties either from the ROM or from a save file of Gen 1 and Gen 2 Pokémon games.

I'm happy to accept pull requests if the community wants to do them.

# Features
- Decode front sprites from Gen 1 and Gen 2 Pokémon games
- Read Trainers' party and PC boxes from Gen 1 and 2
- Inject Pokémon from past distribution events into your Gen 1/Gen 2 game save
- Inject other pokémon into your Gen 1/Gen 2 game save

# Limitations
- Right now, this library only supports the international versions of the games.

# Dependencies
This library only depends on libc. I specifically wanted to avoid the bloat of libstdc++ or any other library. This choice is to keep this library small and portable.

The library was developed on Linux. I haven't tried building it on Windows or WSL yet, so let me know how that goes :)

# Build

To build it, you can do this:

\#Build with 12 threads (Change the thread number to what you want):
make -j 12

# Goal
This project was built as a stepping stone for the next project I want to do: To experiment with the N64 transfer pak with libdragon. In that project, I want to be able to inject Distribution pokémon into a cartridge using the N64 transfer pak. That's why the initial feature set is what it is.

I also hope that if someone else wants to work on something similar, that he/she finds the functionality they want or inspiration to implement it themselves. I tried to document the resources I used during development in the code.

# Future potential improvements
- Add support for gen 3
- Possibly other additions if the community wants them?

