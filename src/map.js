class Chunk {
    constructor(gl, chunkSize, chunk_index_x, chunk_index_y) {

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

        // Now create an array of positions for the cube.
        for (let i = 0; i < chunkSize; i++) {
            for (let j = 0; j < chunkSize; j++) {
            let x = i + chunk_index_x * chunkSize
            let y = j + chunk_index_y * chunkSize
            var value = noise.simplex2(x / 50, y / 50);
            let location = [2 * x, (-4.0 + Math.round(10 * value)) * 2, -y * 2]
            single_positions.forEach((position, index) => {
                positions.push(position + location[index % 3])
            });
            }
        }
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

        for (let i = 0; i < chunkSize; i++) {
            for (let j = 0; j < chunkSize; j++) {
            single_vertexNormals.forEach((normal) => {
                vertexNormals.push(normal)
            });
            }
        }

        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexNormals), gl.STATIC_DRAW);
        const textureCoordBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);

        let textureCoordinates = []
        const single_textureCoordinates = [
            // Front
            0.0,  0.0,
            1.0,  0.0,
            1.0,  1.0,
            0.0,  1.0,
            // Back
            0.0,  0.0,
            1.0,  0.0,
            1.0,  1.0,
            0.0,  1.0,
            // Top
            0.0,  0.0,
            1.0,  0.0,
            1.0,  1.0,
            0.0,  1.0,
            // Bottom
            0.0,  0.0,
            1.0,  0.0,
            1.0,  1.0,
            0.0,  1.0,
            // Right
            0.0,  0.0,
            1.0,  0.0,
            1.0,  1.0,
            0.0,  1.0,
            // Left
            0.0,  0.0,
            1.0,  0.0,
            1.0,  1.0,
            0.0,  1.0,
        ];

        for (let i = 0; i < chunkSize; i++) {
            for (let j = 0; j < chunkSize; j++) {
                single_textureCoordinates.forEach((coord) => {
                    textureCoordinates.push(coord)
                });
            }
        }

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

        for (let i = 0; i < chunkSize; i++) {
            for (let j = 0; j < chunkSize; j++) {
            
            let offset = 24 * (i * chunkSize + j)
                single_indices.forEach((index) => {
                    indices.push(index + offset)
                });
            }
        }

        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);

        this.position = positionBuffer
        this.normal = normalBuffer
        this.textureCoord = textureCoordBuffer
        this.indices = indexBuffer
    }
}

class World {
    constructor(gl, seed) {
        this.gl = gl;
        this.seed = seed;
        this.viewRadius = 4;
        this.chunkSize = 16;
        this.chunks = [];
        this.centerChunk = [0, 0];
        noise.seed(seed);
        this.populateChunks()
    }

    populateChunks() {
        this.chunks = [];
        let position_x = this.centerChunk[0];
        let position_z = this.centerChunk[1];
        for (let chunk_x = -this.viewRadius; chunk_x < this.viewRadius; chunk_x++) {
            for (let chunk_z = -this.viewRadius; chunk_z < this.viewRadius; chunk_z++) {
                this.chunks.push(new Chunk(this.gl, this.chunkSize, chunk_x + position_x, chunk_z + position_z)) 
            }
        }
    }

}