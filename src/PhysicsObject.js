class CollisionSet {
    constructor(top, bottom, front, back, left, right) {
        this.top = top;
        this.bottom = bottom;
        this.front = front;
        this.back = back;
        this.left = left;
        this.right = right;
    }

    union(set) {
        return new CollisionSet(
            this.top || set.top,
            this.bottom || set.bottom,
            this.front || set.front,
            this.back || set.back,
            this.left || set.left,
            this.right || set.right
        )
    }
}

class PhysicsObject {
    constructor(x, y, z, a, b, c) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.a = a;
        this.b = b;
        this.c = c;
    }
    
    worldCoordinates() {
        return [this.x / 2, this.y / 2, this.z / 2];
    }

    isWithinRadius(obj, r) {
        return Math.sqrt(Math.pow(this.x - obj.x, 2) + Math.pow(this.y - obj.y, 2) + Math.pow(this.z - obj.z, 2)) < r;
    }

    collidesWith(obj) {
        return Math.abs(this.x - obj.x) < this.a + obj.a
            && Math.abs(this.y - obj.y) < this.b + obj.b
            && Math.abs(this.z - obj.z) < this.c + obj.c;
    }

    collision(obj) {
        let x = 0.0;
        let y = 0.0;
        let z = 0.0;

        if (this.x > obj.x && this.x - obj.x < this.a + obj.a) {
            x = (this.x - obj.x) - (this.a + obj.a);
        } else if (obj.x - this.x < this.a + obj.a) {
            x = (this.a + obj.a) - (obj.x - this.x);
        }
        
        if (this.y > obj.y && this.y - obj.y < this.b + obj.b) {
            y = (this.y - obj.y) - (this.b + obj.b);
        } else if (obj.y - this.y < this.b + obj.b) {
            y = (this.b + obj.b) - (obj.y - this.y);
        }

        if (this.z > obj.z && this.z - obj.z < this.c + obj.c) {
            z = (this.z - obj.z) - (this.c + obj.c) ;
        } else if (obj.z - this.z < this.c + obj.c) {
            z = (this.c + obj.c) - (obj.z - this.z);
        }

        let max = Math.min(Math.abs(x), Math.abs(y), Math.abs(z));
        if (max == Math.abs(x)) {
            return [x, 0, 0];
        } else if (max == Math.abs(y)) {
            return [0, y, 0];
        } else if (max == Math.abs(z)) {
            return [0, 0, z];
        }
    } 

    
}

class CollisionRay {
    constructor(theta, phi) {
        this.x = Math.sin(Math.PI-theta) * Math.cos(phi)
        this.y = -Math.sin(phi)
        this.z = Math.cos(Math.PI-theta) * Math.cos(phi)
    }
}

function magnitude(v) {
    return Math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
}

function multiply(v, c) {
    return {
        x: v.x * c,
        y: v.y * c,
        z: v.z * c,
    }
}

function dot(v1, v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

function normalize(v) {
    return multiply(v, 1/magnitude(v));
}

function inside(p, box) {
    return p.x <= box.x + box.a && p.x >= box.x - box.a
        && p.y <= box.y + box.b && p.y >= box.y - box.b;
}