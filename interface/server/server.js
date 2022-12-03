const SerialPort = require("serialport");
const WebSocketServer = require("ws").Server;

// SERIAL COMMUNICATION

function listSerialPorts() {
  SerialPort.list().then(
    (ports) => ports.forEach((port) => console.log(port.path)), // list all ports on machine
    (err) => console.log(err) // log any errors
  );
}

function setupSerialConnection(portName, baudRate) {
  sp = new SerialPort(portName, baudRate); // make a new port
  const Readline = SerialPort.parsers.Readline; // make instance of Readline parser
  const parser = new Readline(); // make a new parser to read ASCII lines
  sp.pipe(parser); // pipe the serial stream to the parser
  configureSerialConnection(parser); // configure connection behavior on open and close
}

function configureSerialConnection(parser) {
  sp.on("open", showOnPortOpen); // call showOnPortOpen when port opens
  parser.on("data", readSerialData); // call readSerialData while port is open
  sp.on("close", showOnPortClose); // call showOnPortClose when port closes
  sp.on("error", showOnError); // call showOnError when there is any error
}

function showOnPortOpen() {
  console.log("SERVER UPDATE: Serial Port (" + sp.path + ") Open.");
}

function readSerialData(data) {
  console.log(
    "SERVER UPDATE: Data Received From Arduino (via " + sp.path + "): " + data
  );
  if (connections.length > 0) {
    console.log(
      "SERVER UPDATE: Broadcasting Data To All Websocket Connections."
    );
    broadcastData(data); // broadcast data to all websocket connections
  }
}

function showOnPortClose() {
  console.log("SERVER UPDATE: Serial Port (" + sp.path + ") Closed.");
}

function showOnError(error) {
  console.log("SERVER UPDATE: Serial Port (" + sp.path + ") Error: " + error);
}

// WEB SOCKET COMMUNICATION

function setupWebSocketServer(portNumber) {
  wss = new WebSocketServer({ port: portNumber }); // make a new websocket server
  configureWebSocketServer();
}

function configureWebSocketServer() {
  connections = new Array(); // list of connections to the server
  wss.on("connection", handleConnection); // upon establishin a connection, call handleConnection
}

function sendToSerial(data) {
  console.log(
    "SERVER UPDATE: Sending Data To Arduino (via " + sp.path + "): " + data
  );
  sp.write(data);
}

function handleConnection(client) {
  console.log("SERVER UPDATE: Client Connection Established."); // we have a new client
  connections.push(client); // add this client to the connections array
  client.on("message", sendToSerial); // when a client sends a message
  client.on("close", function () {
    // when a client closes its connection
    console.log("SERVER UPDATE: Client Connection Closed."); // print it out
    let position = connections.indexOf(client); // get the client's position in the array
    connections.splice(position, 1); // and delete it from the array
  });
}

function broadcastData(data) {
  var connection;
  for (connection in connections) {
    connections[connection].send(data); // send the data to each connection
  }
}

// MAIN METHOD

var sp; // global var to store reference to serial port instance
var wss; // global var to store reference to websocket server instance
var connections; // global var to store reference to all websocket connections

function main() {
  const BAUD_RATE = 9600; // baud rate for the serial port connection
  const SERVER_PORT = 8081; // port number for the websocket server
  if (process.argv.length === 4 && process.argv[2] === "--port") {
    setupSerialConnection(process.argv[3], BAUD_RATE);
    setupWebSocketServer(SERVER_PORT);
  } else if (process.argv.length === 3 && process.argv[2] === "--list") {
    listSerialPorts();
  } else {
    console.log("Usage: ");
    console.log(
      "node server.js --port <portname> to start serial communication with <portname>"
    );
    console.log("node server.js --list to list all port names");
  }
}

main();
