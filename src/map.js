class Chunk {
    constructor(world, gl, chunkSize, chunkX, chunkY) {
        this.world = world;
        this.index = [chunkX, chunkY]
        this.block_update = true;
        this.chunkHeight = 256;
        this.chunkBuffer = instance.exports.chunk_init(world.worldBuffer, chunkX, chunkY, 0);
        this.chunkSize = chunkSize;
        this.visible = null;
        this.gl = gl;
    }

    getBuffers() {
        if (this.block_update == false) {
            return this.cached_buffers;
        }
        this.block_update = false;
        let gl = this.gl;
        this.physicsObjects = [];
        this.visible = this.visibleBlocks();

        instance.exports.chunk_update_buffers(this.chunkBuffer);

        const positionBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
        let vertex_offset = instance.exports.chunk_get_vertex_buffer(this.chunkBuffer);
        let vertex_length = instance.exports.chunk_visible_block_count(this.chunkBuffer) * 24 * 3;
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(instance.exports.memory.buffer, vertex_offset, vertex_length), gl.STATIC_DRAW);
        
        const normalBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);
        let normal_offset = instance.exports.chunk_get_normal_buffer(this.chunkBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(instance.exports.memory.buffer, normal_offset, vertex_length), gl.STATIC_DRAW);

        const textureCoordBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);

        let textureCoordinates = []
        const single_textureCoordinates = [
            // Front
            0.0,  0.0625,
            0.0,  0.0,
            0.0625,  0.0,
            0.0625,  0.0625,

            // Back
            0.0,  0.0625,
            0.0,  0.0,
            0.0635,  0.0,
            0.0625,  0.0625,

            // Top
            0.0,  0.0625,
            0.0,  0.0,
            0.0625,  0.0,
            0.0625,  0.0625,

            // Bottom
            0.0,  0.0625,
            0.0,  0.0,
            0.0625,  0.0,
            0.0625,  0.0625,
            
            // Right
            0.0,  0.0625,
            0.0,  0.0,
            0.0625,  0.0,
            0.0625,  0.0625,
            
            // Left
            0.0,  0.0625,
            0.0,  0.0,
            0.0625,  0.0,
            0.0625,  0.0625,
        ];

        
        this.visible.forEach((block) => {
           let q1 = Math.floor(Math.random() * 16);
           let q2 = Math.floor(Math.random() * 16);

            single_textureCoordinates.forEach((coord, i) => {  
                let b = this.getBlock(...block); 
                switch(b) {
                    case 1: 
                    if (i >= 16 && i < 24) {
                        if (i % 2 == 0) {
                            textureCoordinates.push(coord + 0.0625 * 1)
                        } else {
                            textureCoordinates.push(coord)
                        }
                    } else {
                        if (i % 2 == 0) {
                            textureCoordinates.push(coord + 0.0625 * 3)
                        } else {
                            textureCoordinates.push(coord)
                        }
                    }
                    break;
                    case 2: 
                    if (i % 2 == 0) {
                        textureCoordinates.push(coord)
                    } else {
                        textureCoordinates.push(coord)
                    }
                    break;
                    case 3: 
                    if (i % 2 == 0) {
                        textureCoordinates.push(coord)
                    } else {
                        textureCoordinates.push(coord + 0.0625)
                    }
                    break;
                    case 4: 
                    if (i % 2 == 0) {
                        textureCoordinates.push(coord)
                    } else {
                        textureCoordinates.push(coord + 0.0625)
                    }
                    break;
                    case 5: 
                    if (i % 2 == 0) {
                        textureCoordinates.push(coord)
                    } else {
                        textureCoordinates.push(coord + 0.0625)
                    }
                    break;
                    default:
      
                        if (i % 2 == 0) {
                            textureCoordinates.push(coord + q1 * 0.0625)
                        } else {
                            textureCoordinates.push(coord + q2 * 0.0625)
                        }
                }
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

        this.cached_buffers = {
            position: positionBuffer,
            normal: normalBuffer,
            textureCoord: textureCoordBuffer,
            indices: indexBuffer,
            blockCount: this.visible.length,
        }

        return this.cached_buffers;

    }

    getIndex(x, y, z) {
        return y * this.chunkSize * this.chunkSize + x * this.chunkSize + z;
    }
    
    getBlock(x, y, z) {
        return instance.exports.chunk_get(this.chunkBuffer, x, y, z);
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
        //return this.getAdjacentBlocks(x, y, z).filter(block => block == 0).length > 0;
        return instance.exports.chunk_is_block_visible(this.chunkBuffer, x, y, z);
    }
    
    setBlockWorld(wx, wy, wz, b) {
        let x = wx - this.index[0] * this.chunkSize;
        let y = wy;
        let z = wz - this.index[1] * this.chunkSize;
        this.setBlock(x, y, z, b);
    }

    setBlock(x, y, z, b) {

        if (x == 0) {
            let adj = this.world.getChunk(this.index[0] - 1, this.index[1]);
            if (adj) adj.block_update = true;
        }
        
        if (z == 0) {
            let adj = this.world.getChunk(this.index[0], this.index[1] - 1);
            if (adj) adj.block_update = true;

        }
        
        if (x == this.chunkSize - 1) {
            let adj = this.world.getChunk(this.index[0] + 1, this.index[1]);
            if (adj) adj.block_update = true;

        }
        
        if (z == this.chunkSize - 1) {
            let adj = this.world.getChunk(this.index[0], this.index[1] + 1);
            if (adj) adj.block_update = true;
        }

        this.block_update = true;
        return instance.exports.chunk_set(this.chunkBuffer, x, y, z, b);
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
        this.worldBuffer = instance.exports.world_init();
        this.player = instance.exports.world_get_player(this.worldBuffer);
        this.texture = loadTexture(gl, './textures/blocks.png');
        this.viewRadius = 2;
        this.chunkSize = 16;
        this.chunks = [];
        this.savedChunks = [];
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

    render(programInfo, projectionMatrix) {
        let gl = this.gl;
        gl.clearColor(0.554, 0.746, 0.988, 1.0); 
        gl.clearDepth(1.0); 
        gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LEQUAL);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        this.chunks.forEach(chunk => {
            let buffers = chunk.getBuffers();
    
            // Tell WebGL how to pull out the positions from the position
            // buffer into the vertexPosition attribute
            gl.bindBuffer(gl.ARRAY_BUFFER, buffers.position);
            gl.vertexAttribPointer(programInfo.attribLocations.vertexPosition, 3, gl.FLOAT, false, 0, 0);
            gl.enableVertexAttribArray(programInfo.attribLocations.vertexPosition);
    
            // Tell WebGL how to pull out the texture coordinates from
            // the texture coordinate buffer into the textureCoord attribute.
            gl.bindBuffer(gl.ARRAY_BUFFER, buffers.textureCoord);
            gl.vertexAttribPointer(programInfo.attribLocations.textureCoord, 2, gl.FLOAT, false, 0, 0);
            gl.enableVertexAttribArray(programInfo.attribLocations.textureCoord);
    
            // Tell WebGL how to pull out the normals from
            // the normal buffer into the vertexNormal attribute.
            gl.bindBuffer(gl.ARRAY_BUFFER, buffers.normal);
            gl.vertexAttribPointer(programInfo.attribLocations.vertexNormal, 3, gl.FLOAT,  false, 0, 0);
            gl.enableVertexAttribArray(programInfo.attribLocations.vertexNormal);
    
            // Tell WebGL which indices to use to index the vertices
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.indices);
            gl.useProgram(programInfo.program);
            gl.uniformMatrix4fv(programInfo.uniformLocations.projectionMatrix, false, projectionMatrix);
    
            gl.activeTexture(gl.TEXTURE0);
            gl.bindTexture(gl.TEXTURE_2D, this.texture);
            gl.uniform1i(programInfo.uniformLocations.uSampler, 0);
    
            gl.drawElements(gl.TRIANGLES, 36 * buffers.blockCount, gl.UNSIGNED_SHORT, 0)
    
       });
    }


    computeRayIntersection(playerBuffer, collisionRay) {
        let player = {
            x: instance.exports.aabb3_position_x(playerBuffer),
            y: instance.exports.aabb3_position_y(playerBuffer),
            z: instance.exports.aabb3_position_z(playerBuffer)
          };
          
        return instance.exports.world_ray_intersect(
            player.x, player.y, player.z,
            collisionRay.x, collisionRay.y, collisionRay.z,
            this.worldBuffer,
        );
    }
    
    populateChunks() {
        this.chunks = [];
        let center_x = this.centerChunk[0];
        let center_z = this.centerChunk[1]; 

        for (let chunk_x = -this.viewRadius; chunk_x < this.viewRadius; chunk_x++) {
            for (let chunk_z = -this.viewRadius; chunk_z < this.viewRadius; chunk_z++) {
               
                let saved = this.savedChunks.filter(c => {
                    return c.index[0] == chunk_x + center_x && c.index[1] == chunk_z + center_z;
                });
                
                if (saved.length > 0) {
                    this.chunks.push(saved[0])
                } else {
                    let new_chunk = new Chunk(this, this.gl, this.chunkSize, chunk_x + center_x, chunk_z + center_z);
                    instance.exports.world_set_chunk(this.worldBuffer, chunk_x + center_x, chunk_z + center_z, new_chunk.chunkBuffer);
                    this.chunks.push(new_chunk);
                    this.savedChunks.push(new_chunk);
                }

            }
        }
    }


}