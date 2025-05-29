# lr2_chainload

Proxy `d3d9.dll` for auto-loading libraries during LR2's startup.
Almost all of the relevant code was adapted from emskye96's [infinitas_chainload](https://github.com/emskye96/infinitas_chainload) project.
The proxy DLL template was generated using maluramichael's [dll-proxy-generator](https://github.com/maluramichael/dll-proxy-generator) project.

## Usage

- Copy `d3d9.dll` and `chainload.txt` to LR2's directory.
- Add a line to the chainload.txt file containing a path to a library.
- Launch the game.

You can also load a specific `d3d9.dll` file instead of the default system one by specifying the following line in `chainload.txt`:
```
d3d9_overwrite=<path to external d3d9 DLL>
```
This is useful if you are running LR2 through some DirectX wrappers, such as dgVoodoo. Before using this option, make sure that the DLL you want to use is 32-bit.

## Requirements

- [Microsoft Visual C++ Redistributable for Visual Studio 2019 (32-bit)](https://aka.ms/vs/16/release/vc_redist.x86.exe)