# Introduction

This project implements functionality to read stats, front sprites, Trainers' party/PC boxes and other various properties either from the ROM or from a save file of Gen 1 and Gen 2 Pokémon games.
It also has support and data to inject the Distribution event pokémon into gen 1 and gen 2 pokémon save files.

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

# Usage
You can add the library to your project and use either the Gen1GameReader or Gen2GameReader alongside an instance of the BufferBasedRomReader and BufferBasedSaveManager.

There are also a few standalone example applications that are being built while building this project.

The most interesting one of those for most will probably be the addDistributionPoke application:

It gives you a menu in the terminal to select a distribution event pokémon into your save file.

# Goal
This project was built as a stepping stone for [PokeMe64](https://github.com/risingPhil/PokeMe64): This project injects Distribution pokémon into a cartridge using the N64 transfer pak. That's why the initial feature set of libpokemegb is what it is.

I also hope that if someone else wants to work on something similar, that he/she finds the functionality they want or inspiration to implement it themselves. I tried to document the resources I used during development in the code.

# Future potential improvements
- Add support for gen 3
- Possibly other additions if the community wants them?

