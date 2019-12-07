import { KeyboardInput } from './keyboard.js'
import { FPSTracker } from './fps.js'

const server_address = '10.8.29.204'
const server_port = '3000'

let world = null;
window.pid = 0;

/* WebGL Canvas */
const canvas = document.getElementById('glcanvas');

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
    const { instance } = await WebAssembly.instantiateStreaming(
        fetch("./wasm/voxel.wasm"), {
            env: {
                sqrt: Math.sqrt,
                abs: Math.abs,
                sin: Math.sin,
                cos: Math.cos,
                tan: Math.tan,
                get_pid: function() {
                    return window.pid;
                },
                floor: Math.floor,
                print_float: function(num) {
                    console.log(num);
                },
                send: function(pointer, size) {
                    let memory_buffer = instance.exports.memory.buffer;
                    let body = memory_buffer.slice(pointer, pointer + size);
                    fetch(`http://${server_address}:${server_port}/`, {
                        method: 'post',
                        body
                    });
                },
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
                }
            }
        }
    );

    window.instance = instance;
    instance.exports.memory.grow(200);
    instance.exports.mem_init();
    
};

function game_loop() {

    document.addEventListener('pointerlockchange', lockChangeAlert, false);
    document.addEventListener('mozpointerlockchange', lockChangeAlert, false);
    canvas.addEventListener('click', function(event) {
        canvas.requestPointerLock();
        instance.exports.world_click_handler(world);
    });

    const gl = canvas.getContext('webgl', {antialias: false});
    if (!gl) {
        alert('Unable to initialize WebGL. Your browser or machine may not support it.');
        return;
    }
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    gl.enable(gl.BLEND);

    let programInfo = getProgramInfo(gl);
    world = instance.exports.world_init();

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

    let texture = loadTexture(gl, './textures/blocks.png');
    var then = 0;

    // Draw the scene repeatedly
    function run(now) {
        now *= 0.001;  // convert to seconds
        const deltaTime = now - then;
        then = now;
        update(world, deltaTime > 0.1? 0.1: deltaTime);
        render(gl, programInfo, world, texture);
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

function render(gl, programInfo, world, texture) {
    let aspect_ratio = gl.canvas.clientWidth / gl.canvas.clientHeight;
    world_render(gl, world, texture, programInfo, aspect_ratio);
    frameCounter.increment();
}

load_game_source().then(() => {
    game_loop();   
});