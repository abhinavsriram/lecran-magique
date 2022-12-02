let serialport = require("serialport");
let SerialPort = serialport.SerialPort;

// list serial ports:
serialport.list().then(
  (ports) => ports.forEach((port) => console.log(port.path)),
  (err) => console.log(err)
);

// get port name from the command line:
// use as node server.js /dev/cu.usbmodem1411
let portName = process.argv[2];
const myPort = new serialport(portName, 9600);
let Readline = serialport.parsers.Readline; // make instance of Readline parser
let parser = new Readline(); // make a new parser to read ASCII lines
myPort.pipe(parser); // pipe the serial stream to the parser

// call showPortOpen when port opens
myPort.on("open", showPortOpen);
// call readSerialData while port is open
parser.on("data", readSerialData);
// call showPortClose when port closes
myPort.on("close", showPortClose);
// call showError when there is any error
myPort.on("error", showError);

function showPortOpen() {
  console.log("port open. Data rate: " + myPort.baudRate);
}

// reads data
// prints Hello every 10s
function readSerialData(data) {
  console.log(data);
  //   setTimeout(() => {
  //     myPort.write("Hello from the server");
  //   }, 10000);
}

function showPortClose() {
  console.log("port closed.");
}

function showError(error) {
  console.log("Serial port error: " + error);
}
