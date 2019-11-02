var cubeRotation = 0.0;
let player = new PhysicsObject(0.0,165.0,0.0,0.5,2.0, 0.5);
let verticalVelocity = 0.0

let buffers = null;
let fpsSpan = document.getElementById('fps');
let cameraTheta = 0.0
let cameraPhi = 0.0

let chunkCount = 32
let chunkSize = 16
let blockCount = chunkSize * chunkSize
let selectedBlock = null;
let selection = null;
let currentChunk = 0;

let velocity = 8.0;
let w = false;
let a = false;
let s = false;
let d = false;
let meta = false;
let space = false;
let shift = false;

let fps_update_time_seconds = 3;
let last_fps_update_time = new Date();
let fps_count = 0;

window.addEventListener("keyup", (event) => {

  switch(event.key) {
    case 'ArrowUp':
    case 'w':
    case 'W':
        w = false;
      break;
    case 'ArrowDown':
    case 's':
    case 'A':
        s = false;
        break;
    case 'ArrowLeft':
    case 'a':
    case 'A':
        a = false;
        break;
    case 'ArrowRight':
    case 'd':
    case 'D':
        d = false;
        break;
        case 'ArrowRight':
    case ' ':
        space = false;
        break;
    case 'Shift':
        shift = false;
        break;
    case 'Control':
        meta = false;
        break;
    default:
      console.log("unknown key: " + event.key)
  }
});

window.addEventListener("keydown", (event) => {
  switch(event.key) {
    case 'ArrowUp':
    case 'w':
    case 'W':
        w = true;
      break;
    case 'ArrowDown':
    case 's':
    case 'S':
        s = true;
        break;
    case 'ArrowLeft':
    case 'a':
    case 'A':
        a = true;
        break;
    case 'ArrowRight':
    case 'd':
    case 'D':
        d = true;
        break;
    case ' ':
      space = true;
      break;
    case 'Shift':
      shift = true;
      break;
    case 'Control':
        meta = true;
        break;
    default:
        console.log("unknown key: " + event.key)
  }
});

main();

//
// Start here
//
function main() {
  const canvas = document.querySelector('#glcanvas');

  canvas.requestPointerLock = canvas.requestPointerLock ||
                            canvas.mozRequestPointerLock;

  document.exitPointerLock = document.exitPointerLock ||
                            document.mozExitPointerLock;

  canvas.addEventListener('click', function(event) {
    canvas.requestPointerLock();
    if (selectedBlock && meta) {
        let coords = selectedBlock.worldCoordinates();
        console.log(coords);
        let chunkX = Math.floor(coords[0] / buffers.chunkSize);
        let chunkZ = Math.floor(-coords[2] / buffers.chunkSize);
    
        let chunk = buffers.chunks.filter(chunk => {
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
    } else if (selectedBlock) {
        let coords = selectedBlock.worldCoordinates();
        let chunkX = Math.floor(coords[0] / buffers.chunkSize);
        let chunkZ = Math.floor(-coords[2] / buffers.chunkSize);
    
        let chunk = buffers.chunks.filter(chunk => {
          return chunk.index[0] == chunkX && chunk.index[1] == chunkZ;
        });
            
        if (chunk.length > 0) {
            chunk[0].setBlockWorld(coords[0], coords[1], -coords[2], 0);
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
  buffers = new World(gl, Math.random());
  const texture = loadTexture(gl, './textures/blocks.png');

  var then = 0;

  // Draw the scene repeatedly
  function render(now) {
    now *= 0.001;  // convert to seconds
    const deltaTime = now - then;
    then = now;

    drawScene(gl, programInfo, buffers, texture, deltaTime);
    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}


function drawScene(gl, programInfo, world, texture, deltaTime) {
    gl.clearColor(0.554, 0.746, 0.988, 1.0);  // Clear to black, fully opaque
    gl.clearDepth(1.0);                 // Clear everything
    gl.enable(gl.DEPTH_TEST);           // Enable depth testing
    gl.depthFunc(gl.LEQUAL);            // Near things obscure far things

    // Clear the canvas before we start drawing on it.
    let collisionRay = new CollisionRay(cameraTheta, cameraPhi);
    let collisions = [];

    let collision = new CollisionSet(false, false, false, false, false, false);
    world.chunks.forEach(chunk => {
        chunk.physicsObjects.forEach((physicsObject) => {
            if (distance(player.worldCoordinates(), physicsObject.worldCoordinates()) < 10) {
                let x = physicsObject.x - player.x
                let y = physicsObject.y - player.y
                let z = physicsObject.z - player.z
                let valid_times = []

                let left = x - physicsObject.a;
                let right = x + physicsObject.a;
                let top = y + physicsObject.b;
                let bottom = y - physicsObject.b;
                let front = z + physicsObject.c;
                let back = z - physicsObject.c;

                let left_t = left / collisionRay.x;
                let right_t = right / collisionRay.x;
                let top_t = top / collisionRay.y;
                let bottom_t = bottom / collisionRay.y;
                let front_t = front / collisionRay.z;
                let back_t = back / collisionRay.z;

                let left_p = multiply(collisionRay, left_t);
                let left_i = left_t > -0.5
                            && left_p.y > bottom 
                            && left_p.y < top
                            && left_p.z > back
                            && left_p.z < front;
                if (left_i) valid_times.push(left_t);
                
                let right_p = multiply(collisionRay, right_t);
                let right_i = right_t > -0.5
                            && right_p.y > bottom 
                            && right_p.y < top
                            && right_p.z > back
                            && right_p.z < front;
                if (right_i) valid_times.push(right_t);

                let top_p = multiply(collisionRay, top_t);
                let top_i = top_t > -0.5
                            && top_p.x > left 
                            && top_p.x < right
                            && top_p.z > back
                            && top_p.z < front;
                if (top_i) valid_times.push(top_t);

                let bottom_p = multiply(collisionRay, bottom_t);
                let bottom_i = bottom_t > -0.5
                            && bottom_p.x > left 
                            && bottom_p.x < right
                            && bottom_p.z > back
                            && bottom_p.z < front;
                if (bottom_i) valid_times.push(bottom_t);


                let front_p = multiply(collisionRay, front_t);
                let front_i = front_t > -0.5
                            && front_p.x > left 
                            && front_p.x < right
                            && front_p.y > bottom
                            && front_p.y < top;
                if (front_i) valid_times.push(front_t);

                let back_p = multiply(collisionRay, back_t);
                let back_i = back_t > -0.5
                            && back_p.x > left 
                            && back_p.x < right
                            && back_p.y > bottom
                            && back_p.y < top;
                if (back_i) valid_times.push(back_t);

                
                let min_time = Math.min(...valid_times);
                let side = null;
                switch(min_time) {
                    case top_t: side = 'top'; break;
                    case bottom_t: side = 'bottom'; break;
                    case front_t: side = 'front'; break;
                    case back_t: side = 'back'; break;
                    case left_t: side = 'left'; break;
                    case right_t: side = 'right'; break;
                }

                if (valid_times.length > 0) {
                    collisions.push({
                        physicsObject,
                        t: min_time,
                        side: side
                    })
                }   
            }         
        });

        chunk.physicsObjects.forEach((physicsObject, index) => {
            if (physicsObject.collidesWith(player)) {
                let c = physicsObject.collision(player);
                player.x += c[0];
                player.y += c[1];
                if (c[1] > 0) {
                    collision.bottom = true;
                }
                player.z += c[2];
                if (c[1]) {
                    verticalVelocity = 0.0
                }
            }
       });
    });

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    

    // Create a perspective matrix, a special matrix that is
    // used to simulate the distortion of perspective in a camera.
    // Our field of view is 45 degrees, with a width/height
    // ratio that matches the display size of the canvas
    // and we only want to see objects between 0.1 units
    // and 100 units away from the camera.

    const fieldOfView = 45 * Math.PI / 180;   // in radians
    const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
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
    mat4.translate(projectionMatrix, projectionMatrix, [-player.x, -player.y, -player.z])

    let velocityVector = vec3.create();
    let velocityVectorLeft = vec3.create();
    vec3.rotateY(velocityVector, [0,0, velocity], [0,0,0], -cameraTheta);
    vec3.rotateY(velocityVectorLeft, velocityVector, [0,0,0], Math.PI / 2.0);
        
    let chunkX = Math.floor(player.x / 2.0 / buffers.chunkSize);
    let chunkZ = Math.floor(player.z / 2.0/ buffers.chunkSize);
    currentChunk = [chunkX, -chunkZ]

    world.chunks.forEach(chunk => {

        let buffers = chunk.getBuffers();
        // Set the drawing position to the "identity" point, which is
        // the center of the scene.
        const modelViewMatrix = mat4.create();

        const normalMatrix = mat4.create();
        mat4.invert(normalMatrix, modelViewMatrix);
        mat4.transpose(normalMatrix, normalMatrix);

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
        gl.uniformMatrix4fv(programInfo.uniformLocations.modelViewMatrix, false, modelViewMatrix);
        gl.uniformMatrix4fv(programInfo.uniformLocations.normalMatrix, false, normalMatrix);

        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.uniform1i(programInfo.uniformLocations.uSampler, 0);

        gl.drawElements(gl.TRIANGLES, 36 * buffers.blockCount, gl.UNSIGNED_SHORT, 0)

   });

    collisions.sort(function(a, b) {
        return a.t - b.t;
    });

    if (collisions.length > 0) {
        selection = collisions[0];
        selectedBlock = collisions[0].physicsObject;
    } else {
        selectedBlock = null;
        selection = null;
    }
  

   
    // Update the rotation for the next draw
    if (w) {
        player.x -= deltaTime * velocityVector[0];
        player.z -= deltaTime * velocityVector[2];
      }
      if (s) {
        player.x += deltaTime * velocityVector[0];
        player.z += deltaTime * velocityVector[2];
      }
      if (a) {
        player.x -= deltaTime * velocityVectorLeft[0];
        player.z -= deltaTime * velocityVectorLeft[2];
      }
      if (d) {
        player.x += deltaTime * velocityVectorLeft[0];
        player.z += deltaTime * velocityVectorLeft[2];
      }
      if (space) {
        if (shift) {
            player.y -= deltaTime * 24.0;
        } else if (collision.bottom) {
            verticalVelocity = -12.0
        }
      }
  
    
    if (currentChunk[0] != world.centerChunk[0] || currentChunk[1] != world.centerChunk[1]) {
        world.centerChunk = currentChunk;
        world.populateChunks();
    }
   
    if (!collision.bottom) {
        player.y -= deltaTime * verticalVelocity;
        verticalVelocity += 20.0 * deltaTime;
    } else if (verticalVelocity > 0) {
        verticalVelocity = 0.0
    }

    let current_time = new Date();
    let elapsed_time = current_time - last_fps_update_time;
    if (elapsed_time > fps_update_time_seconds * 1000) {
        fpsSpan.innerText = 'FPS: ' + fps_count / fps_update_time_seconds;
        fps_count = 0;
        last_fps_update_time = current_time;
    } else {
        fps_count++;
    }
}
