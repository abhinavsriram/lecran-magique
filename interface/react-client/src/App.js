import { useEffect, useState } from "react";
import "./App.css";
import { DrawingPane } from "./Draw.js";

function App() {
  const [resetProgress, setResetProgess] = useState(0);
  const [drawProgress, setDrawProgess] = useState(0);

  const SERVER_PORT = 8081;
  const socket = new WebSocket("ws://localhost:" + SERVER_PORT);

  function setupWebSocketConnection() {
    socket.onopen = openSocket; // perform when socket is opened
    socket.onmessage = showData; // perform when message is received on socket
  }

  function openSocket() {
    // socket.send("Hello Arduino!");
  }

  function showData(message) {
    let parsedMessage = JSON.parse(message.data); // messages are JSON, parse
    // setResetProgess(parseInt(parsedMessage) * 5); // for testing
    switch (parsedMessage[0]) {
      // AR: A - Arduino, R - Reset Progress
      case "AR":
        setResetProgess(parsedMessage[1]);
        break;
      // AD: A - Arduino, D - Draw Progress
      case "AD":
        setDrawProgess(parsedMessage[1]);
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
      ></DrawingPane>
    </div>
  );
}

export default App;
