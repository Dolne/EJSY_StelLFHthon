# finals code

This folder is meant to be opened in the Arduino IDE.

TODO: more info on how to run code, simulation etc
TODO: this should also explain stuff about the code

## Simulation

The source files are also used for simulating the project in [Wokwi](https://wokwi.com), so all code must be written to run on both the actual hardware and also the simulation. In particular, note that the [finals.ino](./finals.ino) file will be run as a C++ file in the simulation.

The `SIMULATION` constant is set to `1` when running in the simulator, and `0` for the actual hardware. This can be used for example to define different pins for the simulation and hardware or omit certain things which cannot be simulated.

See [simulation/README.md](./simulation/README.md) for more info.