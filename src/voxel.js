import { KeyboardInput } from './keyboard.js'
import { FPSTracker } from './fps.js'
import { Graphics } from './gpu.js'

const server_address = '10.8.29.204'
const server_port = '3000'

let world = null;
window.pid = 0;

/* WebGL Canvas */
const canvas = document.getElementById('glcanvas');
const gl = canvas.getContext('webgl', {antialias: false});
if (!gl) {
    alert('Unable to initialize WebGL. Your browser or machine may not support it.');
}
gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
gl.enable(gl.BLEND);

const graphics = new Graphics(gl);

/* FPS Tracker UI Element */
const frameCounter = new FPSTracker('fps');

/* Key Event Handler */
const keyboard = new KeyboardInput({
    'forwards': ['ArrowUp', 'w', 'W'],
    'backwards': ['ArrowDown', 's', 'S'],
    'left': ['ArrowLeft', 'a', 'A'],
    'right': ['ArrowRight', 'D', 'd'],
    'up': [' '],
    'placeBlock': ['Control'],
});

window.addEventListener('keydown', (e) => {
    console.log(e.key.charCodeAt(0));
    instance.exports.on_key_press(world, e.key.charCodeAt(0));
});

window.addEventListener('keyup', (e) => {
   
});

function move_handler(e) {
    let dx = e.movementX / canvas.clientWidth;
    let dy = e.movementY / canvas.clientHeight;
    instance.exports.world_move_handler(world, dx, dy);
}

function lockChangeAlert() {
    if (document.pointerLockElement === canvas ||
        document.mozPointerLockElement === canvas) {
        document.addEventListener("mousemove", move_handler, false);
    } else {
        document.removeEventListener("mousemove", move_handler, false);
    }
}

async function load_game_source() {
    const result = await fetch('./wasm/voxel.wasm');
    const bytes = await result.arrayBuffer();
    const { instance } = await WebAssembly.instantiate(bytes, {
            env: {
                sqrt: Math.sqrt,
                abs: Math.abs,
                sin: Math.sin,
                cos: Math.cos,
                tan: Math.tan,
                atan2: Math.atan2,
                get_pid: function() {
                    return window.pid;
                },
                floor: Math.floor,
                print_float: function(num) {
                    console.log(num);
                },
                random: function() {
                    return Math.random();
                },
                update_health: function(health) {
                    document.getElementById('health').innerText = `Health: ${health}`
                },
                is_key_pressed: function(key) {
                    const key_str = String.fromCharCode(key);
                    return keyboard.isPressed(key_str);
                },
                print_char: function (c) {
                    console.log(String.fromCharCode(c));
                },
                fetch: function(self, path) {
                    let memory = instance.exports.memory.buffer;
                    let uint8_view = new Uint8Array(memory, path);
                    let length = uint8_view.indexOf(0);
                    let path_string = String.fromCharCode(...uint8_view.subarray(0, length));
                    fetch(path_string).then(result => {
                        return result.arrayBuffer();
                    }).then(result => {
                        let length = result.byteLength;
                        let src_view = new Uint8Array(result);
                        let pointer = instance.exports.malloc(length);
                        let array_buffer = instance.exports.memory.buffer;
                        let dst_view = new Uint8Array(array_buffer, pointer, result.byteLength);
                        for (let i = 0; i < result.byteLength; i++) {
                            dst_view[i] = src_view[i];
                        }
                        instance.exports.fetch_callback(self, pointer, length);
                        instance.exports.free(pointer);
                    });
                },
                game_over: function() {
                    window.location.replace("game_over");

                },
                send: function(pointer, size) {
                    let memory_buffer = instance.exports.memory.buffer;
                    let body = memory_buffer.slice(pointer, pointer + size);
                    /*
                    fetch(`http://${server_address}:${server_port}/`, {
                        method: 'post',
                        body
                    });*/
                },
                __cxa_atexit: function() {
                    console.log('exit');
                },
                _ZN5FetchD2Ev: function() {},
                mem_doctor: function(lo, hi) {
                    let memory_buffer = instance.exports.memory.buffer;
                    let uint32_view = new Uint32Array(memory_buffer, lo, (hi - lo)/4);
                    let index = 0;
                    let blocks = [];
                    let block_count = 0;
                    while(index < (hi - lo)/4) {
                        let header = uint32_view[index];
                        let free = header >>> 31;
                        let size = header & ((-1) >>> 1);
                        blocks.push({free, size, header: header.toString(2)})
                        index += size / 4;
                        block_count++;
                    }
                    
                    console.log(`${block_count} blocks found`);
                    console.log(blocks);
                },

                create_buffer: graphics.createBuffer.bind(graphics),
                update_vertex_buffer: graphics.updateVertexBuffer.bind(graphics),
                update_normal_buffer: graphics.updateNormalBuffer.bind(graphics),
                update_index_buffer: graphics.updateIndexBuffer.bind(graphics),
                update_texture_buffer: graphics.updateTextureBuffer.bind(graphics),
                update_texture: graphics.updateTexture.bind(graphics),
                delete_buffer: graphics.deleteBuffer.bind(graphics),
                draw_buffer: graphics.drawBuffer.bind(graphics)
            }
        }
    );

    console.log(instance);
    window.instance = instance;
    instance.exports.memory.grow(400);
    instance.exports.mem_init();
    
};

function game_loop() {

    document.addEventListener('pointerlockchange', lockChangeAlert, false);
    document.addEventListener('mozpointerlockchange', lockChangeAlert, false);
    canvas.addEventListener('click', function(event) {
        canvas.requestPointerLock();
        instance.exports.world_click_handler(world);
    });

    
    world = instance.exports.world_init();

    /*
    setInterval(() => {
        fetch(`http://${server_address}:${server_port}/`).then(result => {
            return result.arrayBuffer();
        }).then(result => {
            let src_view = new Uint8Array(result);
            let pointer = instance.exports.malloc(result.byteLength);
            let array_buffer = instance.exports.memory.buffer;
            let dst_view = new Uint8Array(array_buffer, pointer, result.byteLength);
            for (let i = 0; i < result.byteLength; i++) {
                dst_view[i] = src_view[i];
            }
            instance.exports.world_message_handler(world, pointer);
            instance.exports.free(pointer);
        });
    }, 100);
*/

    var then = 0;

    // Draw the scene repeatedly
    function run(now) {
        now *= 0.001;  // convert to seconds
        const deltaTime = now - then;
        then = now;
        window.triangles = 0;
        update(world, deltaTime > 0.1? 0.1: deltaTime);
        gl.clearColor(0.554, 0.746, 0.988, 1.0); 
        gl.clearDepth(1.0); 
        gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LEQUAL);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        const aspect_ratio = gl.canvas.clientWidth / gl.canvas.clientHeight;
        instance.exports.on_animation_frame(world, deltaTime, aspect_ratio);
        frameCounter.increment();
        requestAnimationFrame(run);
    }

    requestAnimationFrame(run);
}

function update(world, dt) {
    instance.exports.world_update(
        world, dt,
        keyboard.active('forwards'),
        keyboard.active('backwards'),
        keyboard.active('left'),
        keyboard.active('right'),
        keyboard.active('up')
    );
}


load_game_source().then(() => {
    game_loop();   
});