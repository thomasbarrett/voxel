CC = clang
CFLAGS = -Iinclude --target=wasm32 -nostdlib -nostdinc -O3 -Wl,--no-entry -Wl,--export-all -Wno-implicit-function-declaration -Wno-incompatible-library-redeclaration -Wl,--allow-undefined -Wl,--lto-O3
chunk.wasm: chunk.c src/vector.c heap.c stdlib.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.wasm