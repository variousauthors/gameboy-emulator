zig cc -target wasm32-freestanding -Oz -nostdlib -Wl,--no-entry -o main.wasm src/main.c
