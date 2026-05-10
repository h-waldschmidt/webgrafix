# Webgrafix

WebGPU application scaffold built around Dawn and Slang.

```sh
git submodule update --init
```

## Native build

Dependencies are fetched automatically by CMake.

```sh
cmake --preset native-debug
cmake --build --preset native-debug
```

## Web build

The browser build also uses fetched Dawn and Slang sources, but must be configured through Emscripten.

```sh
emcmake cmake --preset web-debug
cmake --build --preset web-debug
```

If you want Release builds, switch the preset name to `native-release` or `web-release`.