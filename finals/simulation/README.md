# simulation

This folder houses the code for simulating the project using [Wokwi](https://wokwi.com).

The [source files](./src/) are symlinks of the source files in the [finals/main](../main/) directory. If any new files are created, they must be symlinked into the [src](./src/) folder.

## Development

The [PlatformIO IDE](https://platformio.org/platformio-ide) is used for development. For it to work properly, this folder must be opened as a workspace in VS Code.

To run the code, run `pio run` and select "Start Simulation" in the Wokwi extension Quick Actions (or "Wokwi: Start Simulator" from the command palette).

When running the simulation, it is important to set the `-DSIMULATION=1` PlatformIO build flag.
This is set by default in [platformio.ini](./platformio.ini).

If using PlatformIO to run the project on the actual hardware instead of using the Arduino IDE, set `-DSIMULATION=0`.

This can be set by changing [platformio.ini](./platformio.ini) or overriden with an environment variable.

```sh
PLATFORMIO_BUILD_FLAGS="-DSIMULATION=0" pio run
```

## Key differences

There are some differences between the simulation and the actual hardware. This is what necessitates the `-DSIMULATION` build flag.

- No support for I2C expander (MCP23017)
  - As such there are some differences in pin assignment
- No support for audio player (YX5300)
  - The audio player is still "initialized" but not used in the game runner
- Gamemaster buttons are "normally open" in the simulation but "normally closed" in the actual
- The vibration motor is "simulated" with an LED

Additionally to note that the [main/main.ino](../main/main.ino) file is symlinked as [main.cpp](./src/main.cpp), and as such it must also be fully valid C++ code.

## Circuit

The simulated circuit is defined in [diagram.json](./diagram.json), and can be edited in the [Wokwi](https://wokwi.com) online editor for free, or with the Text Editor in VS Code.