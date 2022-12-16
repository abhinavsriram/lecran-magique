# README

## Running The Client

Run `npm start` within [/interface/react-client](/interface/react-client) to start the web interface/client. Open [http://localhost:3000](http://localhost:3000) to view it in your browser.

## Running The Arduino

Open up the fsm directory in the Arduino IDE and upload the code to an Arduino MKR 1000.

## Running The Server

Run `node server.js --list` to see the list of all ports on your machine. Run `node server.js --port <arduino port>` within [/interface/server](/interface/server) to start the server. When you see the message "SERVER UPDATE: Serial Port (`<arduino port>`) Open." on your terminal window then you know that the server has successfully connected to the Arduino.

Once you have all 3 components up and running, feel free to use the web interface.
