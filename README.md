# C Hash Map

## Compiling and running
Make sure you have meson and ninja installed. They're an alternative build system to makefiles.
```bash
brew install meson
brew install ninja
```
## To actually run

```bash
meson setup build # to generate build directory
ninja -c build # to compile
./build/hash_map # to run
```
