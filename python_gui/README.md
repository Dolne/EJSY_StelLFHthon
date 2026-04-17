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

Values `1-9` refer to the shapes to display.

> [!NOTE]
> THe number of shapes and what each of the shapes are is still subject to change.

Currently, the 9 shapes are:

1. large blue square
2. large blue diamond
3. large blue triangle
4. small blue square
5. small blue diamond
6. small blue triangle
7. large orange square
8. large orange diamond
9. large orange triangle

### `display/status`

This topic is set by the GUI to indicate if the wheels are spinning.

Value `0` (false) means that there are no wheels spinning.

Value `1` (true) means that there is at least 1 wheel spinning.

This can be used to know when the GUI is done displaying the shapes.