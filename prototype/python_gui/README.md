# python_gui

This is the virtual prototype for the visual element of the game.

## Setting up the python environment

If you have not done so, create a python virtual environment first.

All commands are run from inside the `python_gui` folder.

```bash
python3 -m venv .venv
```

Then, install all the dependencies.

```bash
python3 -m pip install -r requirements.txt
```

## Running the GUI

Run the app.py file to launch the GUI

```bash
python3 app.py
```

To close the app, press Alt+F4 / Cmd+Q from within the app, or Ctrl+C from within the terminal.

## MQTT communication

The GUI receives message instructing it on the position to spin each slot/wheel to and as well the selected slot.

The GUI sends a message indicating if it is still moving the wheels.

### `display/selected`

This topic is to tell the GUI which slot to select.

Value `0` means no slot selected.

Values `1-4` are used to select each of the 4 slots.

When a slot is selected, an arrow will be lit up above the slot.

### `display/slots/+`

These topics are to tell the GUI what shape to spin each wheel to. There are only 4 topics which are used:

- `display/slots/1`
- `display/slots/2`
- `display/slots/3`
- `display/slots/4`

Each topic corresponds to one slot, using the same index 1-4 as `display/selected`.

Value `0` means a blank, and is used to "disable" the slot.

Values `1-8` refer to the shapes to display.

Value `9` will display the slot number (1-4).

> [!NOTE]
> THe number of shapes and what each of the shapes are is still subject to change.

Currently, the 8 shapes are:

1. small blue triangle
2. large blue triangle
3. small orange triangle
4. large orange triangle
5. small blue square
6. large blue square
7. small orange square
8. large orange square

### `display/lights`

This topic is used to show or hide the RGB light strip.

Value `1` will show the lights while value `0` will hide the lights.

The following subtopics allow more control over how the lights look:

**`display/lights/colours`**

This sets the colours to animate between.

The colours are defined in HSL format. H is from 0–360, while the S and L are from 0–100 inclusive.
If the values are outside the range, it will be rolled over/under for H and clamped for the other values.
This can be useful for controlling the direction of animation of H, as for example, H going from 300 to 60 will be different from -60 to 60 (even though the start and end colours are the same).

H must be set, while S defaults to 100, and L defaults to 50.

The format of the value is progress-colour pairs separated by commas,
with the progress being from `0%` to `100%` and the colour in HSL format represented by 1 to 3 integers separated by a space.

The default value is `0% 0, 100% 360` which will cycle through all the colours at full saturation.

Examples:
- `0% 0, 50% 0, 50% 240, 100% 240` will switch distinctly between red and blue
- `0% 0 100 100, 50% 0, 100% 0 100 100` will animate from white to red to white

If there is no value for 0%, the colour at 0% will be the same as the colour at the smallest progress given. Similarly for 100% except that it will take the colour at the largest progress given. There must be a minimum of 1 progress/colour given.

**`display/lights/travel`**

This controls the left-right movement of the colours.

It is a float value which specifies the difference in animation progress between the first and last segment of the lights.

A positive value will cause movement to the right.

The default value is `0.5`

**`display/lights/period`** - the period of the animation in milliseconds (default `1200`)

**`display/lights/segments`** - the number of segments that the light is split into (default `16`)

Setting `travel` to `0` or `segments` to `1` can be used to show a solid colour.

Refer to [lights_demo.py](./lights_demo.py) for some examples.

### `display/status`

This topic is set by the GUI to indicate if the wheels are spinning.

Value `0` (false) means that there are no wheels spinning.

Value `1` (true) means that there is at least 1 wheel spinning.

This can be used to know when the GUI is done displaying the shapes.