import { useEffect, useState } from "react";
import "./App.css";
import { DrawingPane } from "./Draw.js";

var vectorImagePointsGlobal = [[0, 0]];

function App() {
  const [resetProgress, setResetProgess] = useState(0);
  const [drawProgress, setDrawProgess] = useState(0);
  const [originalImage, setOriginalImage] = useState("");
  const [vectorImage, setVectorImage] = useState("");
  const [vectorImagePoints, setVectorImagePoints] = useState("");

  const SERVER_PORT = 8081;
  const socket = new WebSocket("ws://localhost:" + SERVER_PORT);

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
        console.log(message);
        console.log(parseInt(message));
        setResetProgess(parseInt(message));
        break;
      // AD: A - Arduino, D - Draw Progress
      case "AD":
        setDrawProgess(parseInt(message));
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
          var numb = m[0].match(/\d/g);
          numb = numb.join("");
          m[0] = numb;
          var numb = m[1].match(/\d/g);
          numb = numb.join("");
          m[1] = numb;
          vectorImagePointsGlobal.push(m);
        }
        break;
      // IR: I - Image, R - Resource
      case "IR":
        setVectorImagePoints(vectorImagePointsGlobal);
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
        originalImage={originalImage}
        vectorImage={vectorImage}
        imagePoints={vectorImagePoints}
      ></DrawingPane>
    </div>
  );
}

export default App;
