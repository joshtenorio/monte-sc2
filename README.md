# monte-bot

## About
Monte is a StarCraft II Terran rule-based bot.
Its architecture is based off of parts of CommandCenter I liked.

Monte currently supports the following features:
- Supports building for the [SC2 AI Arena ladder](https://aiarena.net/)
- Building addons
- worker distribution, including long-distance mining
- calling MULES
- Rudimentary scouting to find enemy bases
- Getting infantry weapon/armor upgrades

![for the badge](./worker-rush-not-included.svg)

## Building
### Windows
```bat
:: Clone the project.
$ git clone --recursive git@github.com:joshtenorio/monte-bot.git
$ cd monte-bot

:: Create build directory.
$ mkdir build
$ cd build

:: Create Visual Studio project files.
:: For Visual Studio 2019.
$ cmake ../ -G "Visual Studio 16 2019"
:: For Visual Studio 2017.
$ cmake ../ -G "Visual Studio 15 2017 Win64"

:: Build the project.
$ cmake --build . --parallel

:: Launch the bot with the specified path to a SC2 map, e.g.
$ bin\Debug\Monte.exe Maps/EverDream506.SC2Map
```

### Linux
```bash
# Clone the project.
$ git clone --recursive git@github.com:joshtenorio/monte-bot.git && cd monte-bot

# Create build directory.
$ mkdir build && cd build

# Generate a Makefile.
# Use 'cmake -DCMAKE_BUILD_TYPE=Debug ../' if debuginfo is needed
# Debug build also contains additional debug features and chat commands support.
# or use 'cmake -DBUILD_FOR_LADDER=ON -DSC2_VERSION=4.10.0 ../' to build for aiarena ladder
$ cmake ../

# Build the project.
$ VERBOSE=1 cmake --build . --parallel

# Launch the bot with the specified absolute path to a SC2 map, e.g.
$ ./bin/Monte "/Users/josht/.../EverDream506.SC2Map"
```

## Thanks
- ladder code, CMakelists and build instructions are from [BlankBot](https://github.com/cpp-sc2/blank-bot)
- the procedure for finding expansions is from [MulleMech](https://github.com/ludlyl/MulleMech/blob/master/src/core/Map.cpp)

Both of the above projects use the MIT license.