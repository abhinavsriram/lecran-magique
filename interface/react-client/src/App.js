// custom CSS styling file
import "./App.css";
// component with all interactive components to draw on Etch A Sketch
import { EtchPanel } from "./EtchPanel.js";
// react components
import { useEffect, useState } from "react";

// array of all coordinates representing image to be plotted
var imagePointsGlobal = [[0, 0]];
// port on which web socket server has been opened
const SERVER_PORT = 8081;
// variable that represents the opened web socket
const socket = new WebSocket("ws://localhost:" + SERVER_PORT);

var testResponseTracker = ""; // tracks responses sent by Arduino for testing

function App() {
  // STATE VARIABLES FOR BASE IMPLEMENTATION
  const [resetComplete, setResetComplete] = useState(false); // bool to track whether reset process is complete
  const [drawProgress, setDrawProgess] = useState(-1); // int to track number of lines plotted
  const [stopDrawing, setStopDrawing] = useState(false); // bool to track whether ISR was tripped to stop drawing
  const [watchdogTripped, setWatchdogTripped] = useState(false); // bool to track whether Watchdog was tripped

  // STATE VARIABLES FOR CAPSTONE
  const [originalImage, setOriginalImage] = useState(""); // original image file path
  const [vectorImage, setVectorImage] = useState(""); // vector image file path
  const [imagePoints, setImagePoints] = useState([]); // array tracking coordinates to be plotted for vector image

  function setupWebSocketConnection() {
    socket.onopen = openSocket; // perform when socket is opened
    socket.onmessage = showData; // perform when message is received on socket
  }

  function openSocket() {
    socket.send("Hello Arduino!");
  }

  // reading from Server
  function showData(data) {
    var [instruction, ...message] = String(data.data).split(" ");
    switch (instruction) {
      // AR: A - Arduino, R - Reset Status
      case "AR":
        setResetComplete(true);
        testResponseTracker = "AR";
        break;
      // AD: A - Arduino, D - Draw Progress
      case "AD":
        setDrawProgess(parseInt(message));
        testResponseTracker = String(data.data);
        break;
      // AS: A - Arduino, S - Stop Motors
      case "AS":
        setStopDrawing(true);
        testResponseTracker = "AS";
        break;
      // AW: A - Arduino, W - Watchdog Tripped
      case "AW":
        setWatchdogTripped(true);
        testResponseTracker = "AW";
        break;
      // testing Scenario 1 from the Sequence Diagrams
      case "TEST_SCENARIO_1":
        switch (parseInt(message)) {
          case 1:
            // transition 1-2
            socket.send("SR");
            break;
          case 2:
            // transition 2-3
            socket.send("");
            break;
          case 3:
            // transition 3-4
            if (testResponseTracker === "AR") {
              socket.send("SD 2");
            } else {
              socket.send("TEST_SCENARIO_1 Failed In Transition 3-4");
            }
            break;
          case 4:
            // transition 4-5
            socket.send("SF 0 0");
            break;
          case 5:
            // transition 5-6
            socket.send("");
            break;
          case 6:
            // transition 6-5
            if (testResponseTracker.replaceAll(/\s/g, "") == "AD0") {
              socket.send("SL 1 1");
            } else {
              socket.send("TEST_SCENARIO_1 Failed In Transition 6-5");
            }
            break;
          case 7:
            // transition 5-1
            if (testResponseTracker.replaceAll(/\s/g, "") == "AD1") {
              socket.send("TEST_SCENARIO_1 Passes");
            } else {
              socket.send("TEST_SCENARIO_1 Failed In Transition 5-1");
            }
            break;
        }
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
        const originalImagePath = "/images/" + message + ".png";
        setOriginalImage(originalImagePath);
        const vectorImagePath = "/images/" + message + ".svg";
        setVectorImage(vectorImagePath);
        break;
      default:
        break;
    }
  }

  // set up web socket connection upon first web page load
  useEffect(() => {
    setupWebSocketConnection();
  }, []);

  return (
    <div className="App">
      <EtchPanel
        socket={socket}
        resetComplete={resetComplete}
        drawProgress={drawProgress}
        stopDrawing={stopDrawing}
        watchdogTripped={watchdogTripped}
        originalImage={originalImage}
        vectorImage={vectorImage}
        imagePoints={imagePoints}
      ></EtchPanel>
    </div>
  );
}

export default App;
