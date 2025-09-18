# Install from source

## Download the source code

```bash
git clone --recursive git@github.com:takev/hikolang.git
```

## Build the project

```bash
cd hikolang
cmake --preset=x64-windows
cmake --build --preset=x64-windows-dbg
cmake --build --preset=x64-windows-rel
cmake --build --preset=x64-windows-rdi
```

Configuration presets for different platforms are:
 - `x64-windows`: Windows x64
 - `x64-linux`: Linux x64
 - `arm64-macos`: macOS ARM64

For building there are the following suffixes:
 - `-dbg`: Debug build
 - `-rel`: Release build
 - `-rdi`: Release with debug information
