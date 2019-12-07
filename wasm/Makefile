CC_WASM = /usr/local/Cellar/llvm/*/bin/clang
CC = clang

CFLAGS = -Iinclude -Iinclude/libc --target=wasm32 -nostdlib -nostdinc -O3 -Wl,--no-entry -Wl,--export-all -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wl,--allow-undefined -Wl,--lto-O3
voxel.wasm: src/voxel/chunk.c src/voxel/perlin.c src/voxel/cube.c src/voxel/player.c src/voxel/linalg.c src/libc/heap.c src/libc/stdlib.c src/voxel/physics_object.c src/voxel/world.c
	$(CC_WASM) $(CFLAGS) -o $@ $^

server: src/server/server.c 
	$(CC) $^ -g -Iinclude -o server

clean:
	rm -f *.wasm
	rm -f server