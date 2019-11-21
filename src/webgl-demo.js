import { KeyboardInput } from './keyboard.js'
import { FPSTracker } from './fps.js'

let cameraTheta = 0.0
let cameraPhi = 0.0
let velocity = 8.0;
let world = null;
let selectedBlock = null;
let selection = null;
let currentChunk = 0;

const canvas = document.getElementById('glcanvas');
const frameCounter = new FPSTracker('fps');
const keyboard = new KeyboardInput({
  'forwards': ['ArrowUp', 'w', 'W'],
  'backwards': ['ArrowDown', 's', 'S'],
  'left': ['ArrowLeft', 'a', 'A'],
  'right': ['ArrowRight', 'D', 'd'],
  'up': [' '],
  'placeBlock': ['Control'],
});

/**
 * 
 * @param {'normal'|'drunk'} mode 
 */
function create_projection_matrix(player, cameraPhi, cameraTheta, aspect, mode) {
  let coords = [
    instance.exports.aabb3_position_x(player),
    instance.exports.aabb3_position_y(player),
    instance.exports.aabb3_position_z(player)
  ];
    const fov_angle_degrees = mode == 'normal' ? 45: 120;
    const fieldOfView = fov_angle_degrees * Math.PI / 180;   // in radians
    const zNear = 0.1;
    const zFar = 5000.0;
    const projectionMatrix = mat4.create();

    // note: glmatrix.js always has the first argument
    // as the destination to receive the result.
    mat4.perspective(projectionMatrix,
                    fieldOfView,
                    aspect,
                    zNear,
                    zFar);

    mat4.rotateX(projectionMatrix, projectionMatrix, cameraPhi)
    mat4.rotateY(projectionMatrix, projectionMatrix, cameraTheta)
    mat4.translate(projectionMatrix, projectionMatrix, [-coords[0], - coords[1], -coords[2]]);
    return projectionMatrix;
}

(async function () {
  const { instance } = await WebAssembly.instantiateStreaming(
    fetch("./wasm/chunk.wasm"), {
      env: {
        sqrt: Math.sqrt,
        abs: Math.abs,
        sin: Math.sin,
        cos: Math.cos,
        print_float: function(num) {
          console.log(num);
        }
      }
    }
  );
  window.instance = instance;
  
  canvas.addEventListener('click', function(event) {
    canvas.requestPointerLock();
    if (selection != 0 && keyboard.active('placeBlock')) {
        let coords = [
            instance.exports.aabb3_position_x(selection) / 2,
            instance.exports.aabb3_position_y(selection) / 2,
            instance.exports.aabb3_position_z(selection) / 2
        ];
        let chunkX = Math.floor(coords[0] / world.chunkSize);
        let chunkZ = Math.floor(coords[2] / world.chunkSize);
    
        let chunk = world.chunks.filter(chunk => {
          return chunk.index[0] == chunkX && chunk.index[1] == chunkZ;
        });
            
        if (chunk.length > 0) {
            switch (selection.side) {
            case 'top': chunk[0].setBlockWorld(coords[0], coords[1] + 1, -coords[2], Math.floor(Math.random() * 16)); break;
            case 'bottom': chunk[0].setBlockWorld(coords[0], coords[1] - 1, -coords[2], Math.floor(Math.random() * 16)); break;
            case 'front': chunk[0].setBlockWorld(coords[0], coords[1], -coords[2] - 1, Math.floor(Math.random() * 16)); break;
            case 'back': chunk[0].setBlockWorld(coords[0], coords[1], -coords[2] + 1, Math.floor(Math.random() * 16)); break;
            case 'left': chunk[0].setBlockWorld(coords[0] - 1, coords[1], -coords[2], Math.floor(Math.random() * 16)); break;
            case 'right': chunk[0].setBlockWorld(coords[0] + 1, coords[1], -coords[2], Math.floor(Math.random() * 16)); break;
            }
        }
    } else if (selection != 0) {
         let coords = [
            instance.exports.aabb3_position_x(selection) / 2,
            instance.exports.aabb3_position_y(selection) / 2,
            instance.exports.aabb3_position_z(selection) / 2
        ];
        let chunkX = Math.floor(coords[0] / world.chunkSize);
        let chunkZ = Math.floor(coords[2] / world.chunkSize);
    
        let chunk = world.chunks.filter(chunk => {
          return chunk.index[0] == chunkX && chunk.index[1] == chunkZ;
        });
            
        if (chunk.length > 0) {
            chunk[0].setBlockWorld(coords[0], coords[1], coords[2], 0);
        }
    }
   
  });

  let updatePosition = function(e) {
    cameraTheta += e.movementX / canvas.clientWidth * Math.PI;
    cameraPhi += e.movementY / canvas.clientHeight * Math.PI;
  }

  let lockChangeAlert = function () {
    if (document.pointerLockElement === canvas ||
        document.mozPointerLockElement === canvas) {
      document.addEventListener("mousemove", updatePosition, false);
    } else {
      document.removeEventListener("mousemove", updatePosition, false);
    }
  }

  document.addEventListener('pointerlockchange', lockChangeAlert, false);
  document.addEventListener('mozpointerlockchange', lockChangeAlert, false);
  const gl = canvas.getContext('webgl', {antialias: false});
  if (!gl) {
    alert('Unable to initialize WebGL. Your browser or machine may not support it.');
    return;
  }

  let programInfo = getProgramInfo(gl);
  world = new World(gl, Math.random());

  var then = 0;

  // Draw the scene repeatedly
  function render(now) {
    now *= 0.001;  // convert to seconds
    const deltaTime = now - then;
    then = now;

    drawScene(gl, programInfo, world, deltaTime);
    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
})()


function drawScene(gl, programInfo, world, deltaTime) {

    /*
     *  1. Selection
     *  Compute the intersection of a ray emitted from the cursor with every
     *  nearby element in the world. Then, find the nearest object and set the global 
     *  'selected' and 'selectedBlock' objects to the object with which the ray collided.
     *  This allows the player to select blocks!
     */
    function CollisionRay(theta, phi) {
      this.x = Math.sin(Math.PI-theta) * Math.cos(phi)
      this.y = -Math.sin(phi)
      this.z = Math.cos(Math.PI-theta) * Math.cos(phi)
    }
    selection = world.computeRayIntersection(world.player, new CollisionRay(cameraTheta, cameraPhi));

    let bottom = instance.exports.world_update(
      world.worldBuffer, -cameraTheta, deltaTime,
      keyboard.active('forwards'),
      keyboard.active('backwards'),
      keyboard.active('left'),
      keyboard.active('right'),
      keyboard.active('up')
    );
  
    let coords = [
      instance.exports.aabb3_position_x(world.player) / 2,
      instance.exports.aabb3_position_y(world.player) / 2,
      instance.exports.aabb3_position_z(world.player) / 2
    ];

    let chunkX = Math.floor(coords[0] / world.chunkSize);
    let chunkZ = Math.floor(coords[2] / world.chunkSize);
    currentChunk = [chunkX, chunkZ]

    if (currentChunk[0] != world.centerChunk[0] || currentChunk[1] != world.centerChunk[1]) {
        world.centerChunk = currentChunk;
        world.populateChunks();
    }

    let projectionMatrix = create_projection_matrix(world.player, cameraPhi, cameraTheta, gl.canvas.clientWidth / gl.canvas.clientHeight, 'normal') 
    world.render(programInfo, projectionMatrix);

    // Notify our FPS tracker that another frame was rendered
    frameCounter.increment();
}
