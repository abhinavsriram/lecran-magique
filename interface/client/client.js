const SERVER_PORT = 8081;
const socket = new WebSocket("ws://localhost:" + SERVER_PORT);

function setupWebSocketConnection() {
  socket.onopen = openSocket; // perform when socket is opened
  socket.onmessage = showData; // perform when message is received on socket
}

function openSocket() {
  document.write("Socket Open.");
  socket.send("Hello Arduino!");
}

function showData(message) {
  let parsedMessage = JSON.parse(message.data); // messages are JSON, parse
  document.write(parsedMessage);
  socket.send("Successfully Received Message: " + parsedMessage);
}

function main() {
  setupWebSocketConnection();
}

main();
