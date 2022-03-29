const { EventEmitter } = require("events");
const { Server } = require("ws");
const fs = require('fs');
// const https = require('https');
const http = require('http');
// const spawn = require('child_process').spawn;
// const ls = spawn('python', ['process.py']);
const express = require("express");
const cors = require("cors");

let server = express();

server.use(cors());

server = server.listen(process.env.PORT || 3000, () => console.log(`Listening on ${process.env.PORT || 3000}`));

const ws = new Server({ server });
const bus = new EventEmitter();

const subscribers = [];
let publisher = null;
// hundle ip camera
// http.get('http://192.168.1.3:8080/photo.jpg', (resp) => {
//     let data = [];
//     // A chunk of data has been received.
//     resp.on('data', (chunk) => {
//         data.push(chunk)
//     });
//     // The whole response has been received. Print out the result.
//     resp.on('end', () => {
//         fs.writeFileSync("image.jpg", Buffer.concat(data));
//         console.log("[IMAGE]");
//     });
// }).on("error", (err) => {
//     console.log("Error: " + err.message);
// });

// ls.stdout.on('data', (predictedValue) => {
//     subscribers.forEach((sub) => {
//         sub.send(JSON.stringify(currentData), { binary: false });
//     });
// });

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
    // require("fs").writeFile("out.png", base64Data, 'base64');
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




// ls.stderr.on('data', (data) => {
//     console.error(`stderr: ${data}`);
// });
// ls.on('close', (code) => {
//     console.log("child process exited with code ${code}");
// });
