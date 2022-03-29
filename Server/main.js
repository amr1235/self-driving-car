const { EventEmitter } = require("events");
const { Server } = require("ws");
const spawn = require('child_process').spawn;
const ls = spawn('python', ['car_cv.py']);
const express = require("express");
const cors = require("cors");
const { dir } = require("console");

let server = express();

server.use(cors());

server = server.listen(process.env.PORT || 3000, () => console.log(`Listening on ${process.env.PORT || 3000}`));

const ws = new Server({ server });
const bus = new EventEmitter();

const subscribers = [];
let publisher = null;

ls.stdout.on('data', (dir) => {
    let newDir = "";
    if (dir === "F") {
        newDir = "0";
    } else if (dir === "L") {
        newDir = "-45";
    } else {
        newDir = "45";
    }
    publisher.send(newDir, { binary: false });
});

bus.on("update", (data) => {
    if (data.toString()) {
        // let records = JSON.parse(data.toString());
        // console.log("heartRate : " + records["heartrate"] + " | " + "strengths : " + records["Strengths"]);
        // ls.stdin.write(records["Strengths"].toString() + "\n", () => { });
        // currentData.HeartRate = records["heartrate"];
    }
});

bus.on("command", (cmd) => {
    if (publisher) {
        publisher.send(cmd, { binary: false });
    }
});
bus.on("frame", (frame) => {
    var base64Data = frame.toString().replace(/^data:image\/png;base64,/, "");
    ls.stdin.write(base64Data + "\n", () => { });
});
ws.on("connection", (socket, req) => {
    switch (req.url) {
        case "/sensor":
            publisher = socket;
            socket.on("message", (msg) => {
                bus.emit("update", msg);
            });
            break;
        case "/client":
            subscribers.push(socket);
            socket.on("message", (cmd) => {
                bus.emit("command", cmd);
            });
            break
        case "/camera":
            socket.on("message", (cmd) => {
                bus.emit("frame", cmd);
            });
            break
        default:
            break;
    }
});

ls.stderr.on('data', (data) => {
    console.error(`stderr: ${data}`);
});
ls.on('close', (code) => {
    console.log("child process exited with code ${code}");
});
