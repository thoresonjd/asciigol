# asciigol: ASCII Game of Life

An implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway's_Game_of_Life) animated as ASCII in the terminal.

## Usage

To build the program, run
```
make
```
or
```
make asciigol
```

By default, the game will start with a random initial state.

To execute the program in it's default state, simply run
```
./asciigol
```

### Parameters

The asciigol program supports various parameters:

| Parameter   | Description                                             | Default     | Type                             |
|-------------|---------------------------------------------------------|-------------|----------------------------------|
| `width`     | Width of grid                                           | `100`       | Non-negative integer             |
| `height`    | Height of grid                                          | `40`        | Non-negative integer             |
| `delay`     | Delay between frames in milliseconds                    | `500`       | Non-negative integer             |
| `live-char` | Character representing a live cell                      | `#`         | ASCII character                  |
| `dead-char` | Character representing a dead cell                      | ` ` (space) | ASCII character                  |
| `file`      | Custom configuration file                               | NA          | Name of file on system           |
| `wrap`      | Reaching row/column limit will wrap around to other end | `false`     | String literal `true` or `false` |

To execute the program with parameters, the command must be in the following format:
```
./asciigol --<parameter>=<value> ...
```

In the event that duplicate arguments are provided, only the first instance will be accepted while the subsequent ones will be ignored.

In addition to their default values, `--width` and `--height` also have a current maximum of 250 and 100 respectively. Using 0 for either of the dimensions will result in the program falling back to the default values.

### Configuration Files

As alluded to in the aforementioned table, asciigol supports custom, fixed initial states via configuration files provided via the `--file` parameter.

Configuration files adhere to the format as follows:

The first line is the literal `asciigol`.

The second line contains the non-negative integer width and height of the cell grid separated by a comma: `<width>,<height>`. If `--width` and/or `--height` are also specified in the argument list, they will be ignored in favor of the dimensions specified in the file.

The remaining lines contain a series of zeroes and ones, where `0` represents a dead cell and `1` represents a live cell. Of these remaining lines, each line must contain the same number of characters as the specified width, and the number of lines must match the specified height.

Example:
```
asciigol
10,10
1010000000
0110000000
0100000000
0000000000
0000000000
0000000000
0000000000
0000000000
0000000000
0000000000
```

Sample configuration files, including invalid ones, are located in the `config/` directory.

## Demo

Random initial state

https://github.com/user-attachments/assets/e37a5c96-41ec-46ca-a5bf-e3a5557ec02e

One in three cells are initially alive

https://github.com/user-attachments/assets/f81ddbb4-ae50-4695-abf1-fda95ccc1a84
