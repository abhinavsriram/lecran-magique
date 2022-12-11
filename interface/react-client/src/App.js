import { useEffect, useState } from "react";
import "./App.css";
import { DrawingPane } from "./Draw.js";

var imagePointsGlobal = [[0, 0]];
const SERVER_PORT = 8081;
const socket = new WebSocket("ws://localhost:" + SERVER_PORT);

function App() {
  const [resetProgress, setResetProgess] = useState(0);
  const [drawProgress, setDrawProgess] = useState(0);
  const [stopDrawing, setStopDrawing] = useState(false);
  const [watchdogTripped, setWatchdogTripped] = useState(false);
  const [originalImage, setOriginalImage] = useState("");
  const [vectorImage, setVectorImage] = useState("");
  const [imagePoints, setImagePoints] = useState("");

  function setupWebSocketConnection() {
    socket.onopen = openSocket; // perform when socket is opened
    socket.onmessage = showData; // perform when message is received on socket
  }

  function openSocket() {
    // socket.send("Hello Arduino!");
  }

  // reading from Server
  function showData(data) {
    var [instruction, ...message] = String(data.data).split(" ");
    switch (instruction) {
      // AR: A - Arduino, R - Reset Progress
      case "AR":
        setResetProgess(parseInt(message));
        break;
      // AD: A - Arduino, D - Draw Progress
      case "AD":
        setDrawProgess(parseInt(message));
        break;
      // AD: A - Arduino, S - Stop Motors
      case "AS":
        setStopDrawing(true);
        break;
      // AD: A - Arduino, W - Watchdog Tripped
      case "AW":
        setWatchdogTripped(true);
        break;
      // IP: I - Image, P - Points
      case "IP":
        message = message.join(" ");
        message = message.replaceAll("[(", "");
        message = message.replaceAll(")]", "");
        message = message.split("), (");
        for (var i = 0; i < message.length; i++) {
          var m = message[i];
          m = m.split(", ");
          if (m[0] && m[0].match(/\d/g)) {
            m[0] = m[0].match(/\d/g).join("");
          }
          if (m[1] && m[1].match(/\d/g)) {
            m[1] = m[1].match(/\d/g).join("");
          }
          imagePointsGlobal.push(m);
        }
        break;
      // IR: I - Image, R - Resource
      case "IR":
        setImagePoints(imagePointsGlobal);
        const originalImagePath = "/images/" + message + ".jpeg";
        setOriginalImage(originalImagePath);
        const vectorImagePath = "/images/" + message + ".svg";
        setVectorImage(vectorImagePath);
        break;
      default:
        break;
    }
  }

  useEffect(() => {
    setupWebSocketConnection();
  }, []);

  return (
    <div className="App">
      <DrawingPane
        socket={socket}
        resetProgress={resetProgress}
        drawProgress={drawProgress}
        stopDrawing={stopDrawing}
        watchdogTripped={watchdogTripped}
        originalImage={originalImage}
        vectorImage={vectorImage}
        imagePoints={imagePoints}
      ></DrawingPane>
    </div>
  );
}

export default App;
