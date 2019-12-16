class KeyboardInput {
    constructor(bindings) {
        this.keys = {};
        this.bindings = bindings;
        this.subscribers = {};

        window.addEventListener('keydown', (e) => {
            this.keys[event.key] = true;
        });

        window.addEventListener('keyup', (e) => {
            this.keys[event.key] = false;
        });
    }

    active(event) {
        let combos = this.bindings[event];
        return combos.some((keys) => keys.split('-').every((key) => this.keys[key]));
    }

    isPressed(key) {
        return this.keys[key] == true;
    }
}

export { KeyboardInput };