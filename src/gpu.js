class GraphicsBuffer {

    constructor(gl) {
        this.gl = gl;
        this.vertexBuffer = gl.createBuffer();
        this.indexBuffer = gl.createBuffer();
        this.normalBuffer = gl.createBuffer();
        this.texture = 0;
        this.textureBuffer = gl.createBuffer();

    }

    updateVertexBuffer(vertex_data_view) {
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.vertexBuffer);
        this.gl.bufferData(this.gl.ARRAY_BUFFER, vertex_data_view, this.gl.STATIC_DRAW);
    }

    updateIndexBuffer(index_data_view) {
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, this.indexBuffer);
        this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, index_data_view, this.gl.STATIC_DRAW);    
    }

    updateTexture(texture_index) {
        this.texture = texture_index;
    }

    updateNormalBuffer(normal_data_view) {
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.normalBuffer);
        this.gl.bufferData(this.gl.ARRAY_BUFFER, normal_data_view, this.gl.STATIC_DRAW);
    }

    updateTextureBuffer(texture_data_view) {
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.textureBuffer);
        this.gl.bufferData(this.gl.ARRAY_BUFFER, texture_data_view, this.gl.STATIC_DRAW);
    }

    release() {
        this.gl.deleteBuffer(this.vertexBuffer);
        this.gl.deleteBuffer(this.indexBuffer);
        this.gl.deleteBuffer(this.normalBuffer);
        this.gl.deleteBuffer(this.textureBuffer);
    }
}

class Graphics {

    constructor(gl) {
        this.gl = gl;
        this.program_info = getProgramInfo(gl)
        this.buffers = [];
        this.textures = [
            loadTexture(gl, './textures/blocks.png'),
            loadTexture(gl, './textures/mobs.png')
        ];
    }

    createBuffer() {
        return this.buffers.push(new GraphicsBuffer(this.gl)) - 1;
    }

    updateVertexBuffer(index, vertices, n_vertices) {
        const wasm_memory = instance.exports.memory.buffer;
        const vertex_data_view = new Float32Array(wasm_memory, vertices, 3 * n_vertices);
        this.buffers[index].updateVertexBuffer(vertex_data_view);
    }

    updateIndexBuffer(index, faces, n_faces){
        const wasm_memory = instance.exports.memory.buffer;
        const index_data_view = new Uint16Array(wasm_memory, faces, 3 * n_faces);
        this.buffers[index].n_faces = n_faces;
        this.buffers[index].updateIndexBuffer(index_data_view);
    }

    updateTexture(index, texture_index) {
        this.buffers[index].texture = texture_index;
    }

    updateNormalBuffer(index, normals, n_normals) {
        const wasm_memory = instance.exports.memory.buffer;
        const normal_data_view = new Float32Array(wasm_memory, normals, 3 * n_normals);
        this.buffers[index].updateNormalBuffer(normal_data_view);
    }

    updateTextureBuffer(index, texture_coords, n_texture_coords) {
        const wasm_memory = instance.exports.memory.buffer;
        const texture_data_view = new Float32Array(wasm_memory, texture_coords, 2 * n_texture_coords);
        this.buffers[index].updateTextureBuffer(texture_data_view);
    }

    deleteBuffer(index) {
        if (index != -1) {
            this.buffers[index].release();
            this.buffers[index] = null;
        }
    }

    drawBuffer(index, model_view_matrix, projection_matrix) {
        const gl = this.gl;
        const wasm_memory = instance.exports.memory.buffer;
        const model_view_matrix_view =  new Float32Array(wasm_memory, model_view_matrix, 16);
        const projection_matrix_view =  new Float32Array(wasm_memory, projection_matrix, 16);
        
        gl.bindBuffer(gl.ARRAY_BUFFER, this.buffers[index].vertexBuffer);
        gl.vertexAttribPointer(this.program_info.attribLocations.vertexPosition, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(this.program_info.attribLocations.vertexPosition);


        gl.bindBuffer(gl.ARRAY_BUFFER, this.buffers[index].textureBuffer);
        gl.vertexAttribPointer(this.program_info.attribLocations.textureCoord, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(this.program_info.attribLocations.textureCoord);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.buffers[index].normalBuffer);
        gl.vertexAttribPointer(this.program_info.attribLocations.vertexNormal, 3, gl.FLOAT,  false, 0, 0);
        gl.enableVertexAttribArray(this.program_info.attribLocations.vertexNormal);

        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.buffers[index].indexBuffer);
        gl.useProgram(this.program_info.program);
        gl.uniformMatrix4fv(this.program_info.uniformLocations.modelViewMatrix, false, model_view_matrix_view);
        gl.uniformMatrix4fv(this.program_info.uniformLocations.projectionMatrix, false, projection_matrix_view);

        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, this.textures[this.buffers[index].texture]);
        gl.uniform1i(this.program_info.uniformLocations.uSampler, 0);

        gl.drawElements(gl.TRIANGLES, 3 * this.buffers[index].n_faces, gl.UNSIGNED_SHORT, 0)
    }
}

export { Graphics };