# lr2_chainload

Proxy `d3d9.dll` for auto-loading libraries during LR2's startup.
Almost all of the relevant code was adapted from emskye90's [infinitas_chainload](https://github.com/emskye96/infinitas_chainload) project.
The proxy DLL template was generated using maluramichael's [dll-proxy-generator](https://github.com/maluramichael/dll-proxy-generator) project.

## Usage

- Copy `d3d9.dll` and `chainload.txt` to LR2's directory.
- Add a line to the chainload.txt file containing a path to a library.
- Launch the game.