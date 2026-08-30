# Terminal Snake

A simple Snake game written in C for Linux terminals.

## Controls

| Key | Action |
|-----|--------|
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `P` | Pause / Resume |
| `Q` | Quit |

> **Note:** Use `Q` to quit the game. `Ctrl+C` is currently not handled explicitly.

## Features

- Terminal-based gameplay
- Non-canonical keyboard input
- Real-time keyboard input using `select()`
- Snake collision detection
- Random food generation
- Dynamic snake growth
- Game-over screen
- Score tracking

## Implementation

The snake is currently implemented using a **dynamic circular array**.

The game uses POSIX/Linux facilities such as:

- `termios`
- `select()`
- `read()`
- ANSI escape sequences
- `malloc()` / `realloc()`

## Future Improvements

- Add signal handling for `Ctrl+C` and other termination signals
- Improve terminal cleanup when the program terminates unexpectedly
- Add a high-score system
- Improve game UI
- Experiment with a linked-list implementation of the snake
- Improve portability across different terminal emulators
