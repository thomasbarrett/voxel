class Chunk {
    constructor(world, gl, chunkSize, chunkX, chunkY) {
        this.world = world;
        this.index = [chunkX, chunkY]
        this.chunkHeight = 256;
        this.blocks = new Uint8Array(chunkSize * chunkSize * 256);
        this.blocks.fill(0);
        this.chunkSize = chunkSize;
        this.physicsObjects = [];
        this.visible = null;
        this.gl = gl;

        // Now create an array of positions for the cube.
        for (let x = 0; x < chunkSize; x++) {
            for (let z = 0; z < chunkSize; z++) {
                let height = 75 + Math.round(5 * noise.simplex2((x + this.index[0] * this.chunkSize)/40, (z + this.index[1] * this.chunkSize)/40));
                for (let y = 0; y < height; y++) {
                    this.setBlock(x, y, z, 1);
                }
            }
        }
    }

    getBuffers() {
        let gl = this.gl;
        this.physicsObjects = [];
        this.visible = this.visibleBlocks();

        const positionBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
        let positions = []
        
        const single_positions = [
            // Front face
            -1.0, -1.0,  1.0,
            1.0, -1.0,  1.0,
            1.0,  1.0,  1.0,
            -1.0,  1.0,  1.0,

            // Back face
            -1.0, -1.0, -1.0,
            -1.0,  1.0, -1.0,
            1.0,  1.0, -1.0,
            1.0, -1.0, -1.0,

            // Top face
            -1.0,  1.0, -1.0,
            -1.0,  1.0,  1.0,
            1.0,  1.0,  1.0,
            1.0,  1.0, -1.0,

            // Bottom face
            -1.0, -1.0, -1.0,
            1.0, -1.0, -1.0,
            1.0, -1.0,  1.0,
            -1.0, -1.0,  1.0,

            // Right face
            1.0, -1.0, -1.0,
            1.0,  1.0, -1.0,
            1.0,  1.0,  1.0,
            1.0, -1.0,  1.0,

            // Left face
            -1.0, -1.0, -1.0,
            -1.0, -1.0,  1.0,
            -1.0,  1.0,  1.0,
            -1.0,  1.0, -1.0,
        ];

        this.visible.forEach(block => {
            let location = [(block[0] + this.index[0] * this.chunkSize) * 2, block[1] * 2, -(block[2] + this.index[1] * this.chunkSize) * 2]   
            this.physicsObjects.push(new PhysicsObject(...location, 1, 1, 1));
            single_positions.forEach((position, index) => {
                positions.push(position + location[index % 3])
            });
        });

        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
        
        const normalBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);

        let vertexNormals = []
        const single_vertexNormals = [
            // Front
            0.0,  0.0,  1.0,
            0.0,  0.0,  1.0,
            0.0,  0.0,  1.0,
            0.0,  0.0,  1.0,

            // Back
            0.0,  0.0, -1.0,
            0.0,  0.0, -1.0,
            0.0,  0.0, -1.0,
            0.0,  0.0, -1.0,

            // Top
            0.0,  1.0,  0.0,
            0.0,  1.0,  0.0,
            0.0,  1.0,  0.0,
            0.0,  1.0,  0.0,

            // Bottom
            0.0, -1.0,  0.0,
            0.0, -1.0,  0.0,
            0.0, -1.0,  0.0,
            0.0, -1.0,  0.0,

            // Right
            1.0,  0.0,  0.0,
            1.0,  0.0,  0.0,
            1.0,  0.0,  0.0,
            1.0,  0.0,  0.0,

            // Left
            -1.0,  0.0,  0.0,
            -1.0,  0.0,  0.0,
            -1.0,  0.0,  0.0,
            -1.0,  0.0,  0.0
        ];

        this.visible.forEach((block, i) => {
            single_vertexNormals.forEach((normal) => {
                vertexNormals.push(normal)
            });
        });
  
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexNormals), gl.STATIC_DRAW);
        const textureCoordBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);

        let textureCoordinates = []
        const single_textureCoordinates = [
            // Front
            0.125,  0.0625,
            0.1875,  0.0625,
            0.1875,  0.0,
            0.125,  0.0,
            // Back
            0.125,  0.0625,
            0.125,  0.0,
            0.1875,  0.0,
            0.1875,  0.0625,

            // Top
            0.0625,  0.0,
            0.0625,  0.0625,
            0.125,  0.0625,
            0.125,  0.0,
            // Bottom
            0.0,  0.0,
            1.0,  0.0,
            1.0,  1.0,
            0.0,  1.0,
            // Right
            0.125,  0.0625,
            0.125,  0.0,
            0.1875,  0.0,
            0.1875,  0.0625,
            
            // Left
            0.125,  0.0625,
            0.1875,  0.0625,
            0.1875,  0.0,
            0.125,  0.0,
        ];

        this.visible.forEach((block, i) => {
            single_textureCoordinates.forEach((coord) => {
                textureCoordinates.push(coord)
            });
        });


        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates), gl.STATIC_DRAW);
        const indexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);

        let indices = []
        const single_indices = [
            0,  1,  2,      0,  2,  3,    // front
            4,  5,  6,      4,  6,  7,    // back
            8,  9,  10,     8,  10, 11,   // top
            12, 13, 14,     12, 14, 15,   // bottom
            16, 17, 18,     16, 18, 19,   // right
            20, 21, 22,     20, 22, 23,   // left
        ];

        this.visible.forEach((block, i) => {
            let offset = 24 * i;
            single_indices.forEach((index) => {
                indices.push(index + offset)
            });
        });

        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);

        return {
            position: positionBuffer,
            normal: normalBuffer,
            textureCoord: textureCoordBuffer,
            indices: indexBuffer,
            blockCount: this.visible.length,
        }

    }

    getIndex(x, y, z) {
        return y * this.chunkSize * this.chunkSize + x * this.chunkSize + z;
    }
    
    getBlock(x, y, z) {
        return this.blocks[this.getIndex(x, y, z)];
    }

    getAdjacentBlocks(x, y, z) {
        let left = x > 0 ? this.getBlock(x - 1, y, z): this.world.getBlock(this.index[0] - 1, this.index[1], this.chunkSize - 1, y, z);
        let right = x < this.chunkSize - 1 ? this.getBlock(x + 1, y, z): this.world.getBlock(this.index[0] + 1, this.index[1], 0, y, z);
        let top = y > 0 ? this.getBlock(x, y - 1, z): null;
        let bottom = y < this.chunkHeight - 1 ? this.getBlock(x, y + 1, z): null;
        let front = z > 0 ? this.getBlock(x, y, z - 1): this.world.getBlock(this.index[0], this.index[1] - 1, x, y, this.chunkSize - 1);;
        let back = z < this.chunkSize - 1 ? this.getBlock(x, y, z + 1): this.world.getBlock(this.index[0], this.index[1] + 1, x, y, 0);;
        return [left, right, top, bottom, front, back].filter((b) => {
            return b != null;
        })
    }

    isAdjacentToAir(x, y, z) {
        return this.getAdjacentBlocks(x, y, z).filter(block => block == 0).length > 0;
    }
    
    setBlockWorld(wx, wy, wz, b) {
        let x = wx - this.index[0] * this.chunkSize;
        let y = wy;
        let z = wz - this.index[1] * this.chunkSize;
        this.setBlock(x, y, z, b);
    }

    setBlock(x, y, z, b) {
        this.blocks[this.getIndex(x, y, z)] = b;
    }

    visibleBlocks() {
        let visible = [];
        for (let x = 0; x < this.chunkSize; x++) {
            for (let y = 0; y < this.chunkHeight; y++) {
                for (let z = 0; z < this.chunkSize; z++) {
                    let block = this.getBlock(x, y, z);
                    if (block != 0 && this.isAdjacentToAir(x, y, z)) {
                        visible.push([x, y, z]);
                    }
                }
            }
        }
        return visible;
    }
}


class World {
    constructor(gl, seed) {
        this.gl = gl;
        this.seed = seed;
        this.viewRadius = 3;
        this.chunkSize = 8;
        this.chunks = [];
        this.centerChunk = [0, 0];
        noise.seed(seed);
        this.populateChunks()
    }

    getChunk(i, j) {
        return this.chunks.filter((chunk) => {
            return chunk.index[0] == i && chunk.index[1] == j;
        })[0];
    }

    getBlock(i, j, x, y, z) {
        let chunk = this.getChunk(i, j);
        if (chunk) {
            return chunk.getBlock(x, y, z);
        } else return -1;
    }

    populateChunks() {
        this.chunks = [];
        let center_x = this.centerChunk[0];
        let center_z = this.centerChunk[1];        
        for (let chunk_x = -this.viewRadius; chunk_x < this.viewRadius; chunk_x++) {
            for (let chunk_z = -this.viewRadius; chunk_z < this.viewRadius; chunk_z++) {
                this.chunks.push(new Chunk(this, this.gl, this.chunkSize, chunk_x + center_x, chunk_z + center_z)) 
            }
        }
    }


}