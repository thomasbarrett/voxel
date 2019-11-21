
function chunk_get_buffers(chunkBuffer, gl) {

    instance.exports.chunk_update_buffers(chunkBuffer);
    
    let visible_block_length = instance.exports.chunk_visible_block_count(chunkBuffer);

    const positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    let vertex_offset = instance.exports.chunk_get_vertex_buffer(chunkBuffer);
    let vertex_length = visible_block_length * 24 * 3;
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(instance.exports.memory.buffer, vertex_offset, vertex_length), gl.STATIC_DRAW);
    
    const normalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);
    let normal_offset = instance.exports.chunk_get_normal_buffer(chunkBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(instance.exports.memory.buffer, normal_offset, vertex_length), gl.STATIC_DRAW);

    const textureCoordBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);
    let texture_offset = instance.exports.chunk_get_texture_buffer(chunkBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(instance.exports.memory.buffer, texture_offset, vertex_length / 3 * 2), gl.STATIC_DRAW);

    const indexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    let index_offset = instance.exports.chunk_get_index_buffer(chunkBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(instance.exports.memory.buffer, index_offset, vertex_length * 36 / 24 / 3), gl.STATIC_DRAW);

    this.cached_buffers = {
        position: positionBuffer,
        normal: normalBuffer,
        textureCoord: textureCoordBuffer,
        indices: indexBuffer,
        blockCount: visible_block_length,
    }

    return this.cached_buffers;

}


class World {
    constructor(gl, seed) {
        this.gl = gl;
        this.worldBuffer = instance.exports.world_init();
        this.player = instance.exports.world_get_player(this.worldBuffer);
        this.texture = loadTexture(gl, './textures/blocks.png');
        noise.seed(seed);
    }

    render(programInfo, projectionMatrix) {
        let gl = this.gl;
        gl.clearColor(0.554, 0.746, 0.988, 1.0); 
        gl.clearDepth(1.0); 
        gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LEQUAL);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        for (let i = 0; i < instance.exports.world_get_chunk_count(this.worldBuffer); i++) {
            let chunk = instance.exports.world_get_chunk_by_index(this.worldBuffer, i);
            let buffers =  chunk_get_buffers(chunk, gl); 

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
    
       }
    }

}