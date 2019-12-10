CC_WASM = /usr/local/Cellar/llvm/*/bin/clang++
CC = clang++

CFLAGS = -Iinclude -Iinclude/libc  -I/usr/local/Cellar/llvm/9.0.0/include/c++/v1 --target=wasm32 -fno-exceptions -nostdlib -O3 -Wl,--no-entry -Wl,--export-all -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wl,--allow-undefined -Wl,--lto-O3
voxel.wasm: src/voxel/chunk.cpp src/voxel/perlin.cpp src/voxel/cube.cpp src/voxel/player.cpp src/voxel/linalg.cpp src/libc/heap.cpp src/libc/stdlib.cpp src/voxel/physics_object.cpp src/voxel/world.cpp
	$(CC_WASM) $(CFLAGS) -o $@ $^

server: src/server/server.c 
	$(CC) $^ -g -Iinclude -o server

clean:
	rm -f *.wasm
	rm -f server