# monte-bot

## About
Monte is a StarCraft II Bot built to play Terran.
Its architecture is based off of parts of CommandCenter and Suvorov/MulleMech I liked.

## Building
### Windows
```bat
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
:: For the debug build.
$ cmake --build . --config Debug --parallel

:: Launch the bot with the specified path to a SC2 map, e.g.
$ bin\Debug\Monte.exe Maps/EverDream506.SC2Map
```

### Linux
```bash
# Create build directory.
$ mkdir build && cd build

# Generate a Makefile.
# Use 'cmake -DCMAKE_BUILD_TYPE=Debug ../' if debuginfo is needed
# Debug build also contains additional debug features and chat commands support.
$ cmake ../

# Build the project.
$ VERBOSE=1 cmake --build . --parallel

# Launch the bot with the specified absolute path to a SC2 map, e.g.
$ ./bin/Monte "/Users/josht/.../EverDream506.SC2Map"
```

## Thanks
- ladder code, CMakelists and build instructions are from [Suvorov](https://github.com/alkurbatov/suvorov-bot/)
- the procedure for finding expansions is from [MulleMech](https://github.com/ludlyl/MulleMech/blob/master/src/core/Map.cpp)