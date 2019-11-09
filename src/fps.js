class FPSTracker {
    constructor(id) {
        this.element = document.getElementById(id);
        this.update_duration = 3000;
        this.fps_count = 0;
        setInterval(() => {
            this.element.innerText = 'FPS: ' + (this.fps_count * 1000 / this.update_duration).toFixed(1);
            this.fps_count = 0;
        },  this.update_duration);
    }

    increment() {
        this.fps_count++;
    }
}

export { FPSTracker };