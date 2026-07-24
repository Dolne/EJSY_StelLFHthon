# simulation

This folder houses the code for simulating the project using [Wokwi](https://wokwi.com).

The [source files](./src/) are symlinks of the source files in the parent [finals](../) directory.

The [PlatformIO IDE](https://platformio.org/platformio-ide) is used for development. For it to work properly, this folder must be opened as a workspace in VS Code.

To run the code, run `pio run` and select "Start Simulation" in the Wokwi extension Quick Actions (or "Wokwi: Start Simulator" from the command palette).

When running the simulation, it is important to set the `-DSIMULATION=1` PlatformIO build flag.
This is set by default in [platformio.ini](./platformio.ini).

If using PlatformIO to run the project on the actual hardware instead of using the Arduino IDE, set `-DSIMULATION=0`.

This can be set by changing [platformio.ini](./platformio.ini) or overriden with an environment variable.

```sh
PLATFORMIO_BUILD_FLAGS="-DSIMULATION=0" pio run
```