# asciigolgen

A configuration file generator for asciigol that runs in the terminal.

## Usage

To build, run
```
make asciigolgen
```

To execute, run
```
./asciigolgen [arguments]
```

Where the arguments take the form of `--<parameter>=<value>`.

All of the following parameters are required:

| Parameter | Description                            | Type                                                 |
|-----------|----------------------------------------|------------------------------------------------------|
| `width`   | Width of grid                          | Non-negative integer                                 |
| `height`  | Height of grid                         | Non-negative integer                                 |
| `file`    | Name of configuration file to write to | Name of file                                         |
| `cell`    | State in which to initialize all cells | Character literal `0` (dead cell) or `1` (live cell) | 

The asciigolgen program will prompt you with an initial game state in which all cells are dead (`0`) or live (`1`) with the dimensions of `<width>` and `<height>`. The cells of this initial state can be modified through the terminal by moving between them with the arrow keys (up, down, right, left) and replacing them with `1` or `0`. When finished, press `q` to quit, and the final configuration will be written to the file specified by `<filename>`.

## Demo

Creating a glider configuration for asciigol with asciigolgen

https://github.com/user-attachments/assets/31058550-8946-4e85-a9ed-09452d9b1e34
