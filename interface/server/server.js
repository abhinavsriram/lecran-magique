let serialport = require("serialport");
let SerialPort = serialport.SerialPort;

// list serial ports:
serialport.list().then(
  (ports) => ports.forEach((port) => console.log(port.path)),
  (err) => console.log(err)
);
// get port name from the command line:
// use as node server.js /dev/cu.usbmodem1411
// let portName = process.argv[2];
// let myPort = new SerialPort(portName, 9600);
// let Readline = SerialPort.parsers.Readline; // make instance of Readline parser
// let parser = new Readline(); // make a new parser to read ASCII lines
// myPort.pipe(parser); // pipe the serial stream to the parser

// myPort.on("open", showPortOpen);
// parser.on("data", readSerialData);
// myPort.on("close", showPortClose);
// myPort.on("error", showError);

// function showPortOpen() {
//   console.log("port open. Data rate: " + myPort.baudRate);
// }

// function readSerialData(data) {
//   console.log(data);
// }

// function showPortClose() {
//   console.log("port closed.");
// }

// function showError(error) {
//   console.log("Serial port error: " + error);
// }
