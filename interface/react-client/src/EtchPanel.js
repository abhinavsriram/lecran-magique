// custom CSS styling file
import "./EtchPanel.css";
// bootstrap styling and components
import "bootstrap/dist/css/bootstrap.css";
import Form from "react-bootstrap/Form";
import Button from "react-bootstrap/Button";
import Dropdown from "react-bootstrap/Dropdown";
import ProgressBar from "react-bootstrap/ProgressBar";
// custom popup modal
import { PopUp } from "./PopUp";
// react components
import { useEffect, useRef, useState } from "react";

// OPEN AI config values to make API calls to DALL·E
import { Configuration, OpenAIApi } from "openai";
const OPENAI_API_KEY = "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";
const configuration = new Configuration({
  apiKey: OPENAI_API_KEY,
});
const openai = new OpenAIApi(configuration);

// object to represent a point on the canvas
class CanvasPoint {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
}
// array of all coordinates plotted on the canvas
var canvasPoints = [new CanvasPoint(0, 0)];
// bool that represents if the canvas is currently being drawn on
var canvasDrawing = false;
// array of all instructions to be sent to Arduino
var arduinoInstructions = [];

// number of instructions to be sent at once
const BUFFER_SIZE = 1;

export function EtchPanel(props) {
  // STATE VARIABLES FOR BASE IMPLEMENTATION
  const [selectedMode, setSelectedMode] = useState("Draw"); // default mode is Draw
  const [resetPopUp, setResetPopUp] = useState(false); // bool to display reset popup
  const [shakePopUp, setShakePopUp] = useState(false); // bool to display shake popup
  const [canInteract, setCanInteract] = useState(true); // bool to determine if user can interact with UI
  const [canStart, setCanStart] = useState(true); // bool to determine if user can hit Start Drawing button
  const [sketchProgressBar, setSketchProgressBar] = useState(false); // bool to display sketch progress bar
  const [sketchSuccessPopUp, setSketchSuccessPopUp] = useState(false); // bool to display sketch success popup
  const canvas = useRef(null); // reference to canvas element

  // STATE VARIABLES FOR CAPSTONE
  const [prompt, setPrompt] = useState(""); // prompt for DALL·E
  const [propmtPopUp, setPromptPopUp] = useState(false); // bool to display prompt popup
  const [workingPopUp, setWorkingPopUp] = useState(false); // bool to display "working on it" popup

  // CANVAS UTILITY FUNCTIONS
  // Draws all points currenly present in canvasPoints as if the canvas
  // were an Etch A Sketch i.e., segments are drawn contiguously.
  const drawPointsContiguous = () => {
    const ctx = canvas.current.getContext("2d");
    ctx.strokeStyle = "black";
    ctx.clearRect(0, 0, canvas.current.width, canvas.current.height);
    ctx.beginPath();
    ctx.moveTo(canvasPoints[0].x, canvasPoints[0].y);
    for (var i = 0; i < canvasPoints.length; i++) {
      ctx.lineTo(canvasPoints[i].x, canvasPoints[i].y);
      ctx.moveTo(canvasPoints[i].x, canvasPoints[i].y);
    }
    ctx.stroke();
    ctx.closePath();
  };

  const addNewPoint = (x, y) => {
    if (
      x !== canvasPoints[canvasPoints.length - 1].x ||
      y !== canvasPoints[canvasPoints.length - 1].y
    ) {
      canvasPoints.push(new CanvasPoint(x, y));
    }
  };

  const popPoint = () => {
    canvasPoints.pop();
  };

  const clearAllPoints = () => {
    canvasPoints = [new CanvasPoint(0, 0)];
  };

  // CANVAS EVENT HANDLERS
  const onCanvasMouseDown = () => {
    if (canInteract) {
      canvasDrawing = true;
      drawPointsContiguous();
    }
  };

  const onCanvasMouseUp = () => {
    if (canInteract) {
      canvasDrawing = false;
      drawPointsContiguous();
    }
  };

  const onCanvasMouseClick = (e) => {
    if (canInteract) {
      addNewPoint(
        e.pageX - canvas.current.offsetLeft,
        e.pageY - canvas.current.offsetTop
      );
      drawPointsContiguous();
    }
  };

  const onCanvasMouseMove = (e) => {
    if (canInteract) {
      if (canvasDrawing) {
        addNewPoint(
          e.pageX - canvas.current.offsetLeft,
          e.pageY - canvas.current.offsetTop
        );
        drawPointsContiguous();
      }
    }
  };

  const onCanvasMouseLeave = () => {
    if (canInteract) {
      canvasDrawing = false;
      drawPointsContiguous();
    }
  };

  const onClickErase = () => {
    canvasDrawing = false;
    clearAllPoints();
    drawPointsContiguous();
  };

  const onClickEraseLast = () => {
    if (canvasPoints.length > 1) {
      popPoint();
      drawPointsContiguous();
    }
  };

  // GENERAL UTILITY FUNCTIONS
  // Once the Start Drawing button is pressed, the first step is to
  // initiate the cursor reset process by sending an SR instruction.
  const startDrawing = () => {
    startReset();
    // SR: S - Server, R - Reset Instruction
    props.socket.send("SR");
    setCanInteract(false);
    setCanStart(false);
  };

  const startReset = () => {
    setResetPopUp(true);
  };

  const displayShakePopUp = () => {
    setShakePopUp(true);
  };

  // hide reset popup once reset is complete
  // also show popup asking user to shake the Etch A Sketch to clear screen
  useEffect(() => {
    if (props.resetComplete) {
      setResetPopUp(false);
      setTimeout(() => {
        displayShakePopUp();
      }, 1000);
    }
  }, [props.resetComplete]);

  // once the user clears the screen, send an SD instruction to transition states
  const confirmShakePopUp = () => {
    setShakePopUp(false);
    // SD: S - Server, D - Draw Instruction
    if (selectedMode === "Draw") {
      props.socket.send("SD " + String(canvasPoints.length));
      // SF: S - Server, F - First Instruction
      setTimeout(() => {
        props.socket.send("SF 0 0");
        arduinoInstructions.shift();
      }, 2500);
    } else if (selectedMode === "Print") {
      props.socket.send("SD " + String(props.imagePoints.length));
      // SF: S - Server, F - First Instruction
      setTimeout(() => {
        props.socket.send("SF 0 0");
        arduinoInstructions.shift();
      }, 2500);
    }
    reallyStartDrawing();
  };

  // display drawing progress bar and convert coordinates to Arduino instructions
  const reallyStartDrawing = () => {
    setSketchProgressBar(true);
    translateToArduino();
  };

  // send instructions to Arduino in batches of BUFFER_SIZE
  useEffect(() => {
    for (var i = 0; i < BUFFER_SIZE; i++) {
      if (arduinoInstructions[i]) {
        setTimeout(() => {
          props.socket.send(arduinoInstructions[i]);
          arduinoInstructions.shift();
        }, 1000);
      }
    }
  }, [props.drawProgress]);

  // inform user that drawing has been stopped by ISR
  // reset all state variables appropriately
  useEffect(() => {
    setSketchProgressBar(false);
    setCanInteract(true);
    setCanStart(true);
    canvasDrawing = false;
    clearAllPoints();
    drawPointsContiguous();
  }, [props.stopDrawing]);

  // hide draw progress bar once drawing is complete
  // also display drawing success popup
  useEffect(() => {
    if (
      selectedMode === "Draw" &&
      ((props.drawProgress + 1) / canvasPoints.length) * 100 === 100
    ) {
      setSketchProgressBar(false);
      setSketchSuccessPopUp(true);
    } else if (
      selectedMode === "Print" &&
      ((props.drawProgress + 1) / props.imagePoints.length) * 100 === 100
    ) {
      setSketchProgressBar(false);
      setSketchSuccessPopUp(true);
    }
  }, [props.drawProgress]);

  // TRANSLATE TO ARDUINO INSTRUCTIONS
  const translateToArduino = () => {
    var i;
    arduinoInstructions = [];
    if (selectedMode === "Draw") {
      for (i = 0; i < canvasPoints.length; i++) {
        // SL: S - Server, L - Line Instruction
        arduinoInstructions.push(
          "SL" +
            " " +
            Math.round(canvasPoints[i].x * 0.5) +
            " " +
            Math.round(canvasPoints[i].y * 0.5)
        );
      }
    } else if (selectedMode === "Print") {
      for (i = 0; i < props.imagePoints.length; i++) {
        // SL: S - Server, L - Line Instruction
        arduinoInstructions.push(
          "SL" +
            " " +
            Math.round(props.imagePoints[i][0] * 0.5) +
            " " +
            Math.round(props.imagePoints[i][1] * 0.5)
        );
      }
    }
  };

  // CAPSTONE FEATURE FUNCTIONS
  const getImageFromOpenAI = async (prompt) => {
    if (prompt === "" || prompt.length < 30) {
      setPromptPopUp(true);
    } else {
      // prod:
      // setWorkingPopUp(true);
      // try {
      //   const response = await openai.createImage({
      //     prompt: prompt,
      //     n: 1,
      //     size: "256x256",
      //   });
      //   const image_url = response.data.data[0].url;
      //   props.socket.send("PY " + image_url);
      // } catch (error) {
      //   if (error.response) {
      //     console.log(error.response.status);
      //     console.log(error.response.data);
      //   } else {
      //     console.log(error.message);
      //   }
      // }
      // test:
      const image_url =
        "https://cdn.shopify.com/s/files/1/0590/3305/9479/files/cup_of_coffee_for_websites_256x256_crop_center.jpg?v=1630283140";
      props.socket.send("PY " + image_url);
      setWorkingPopUp(true);
    }
  };

  const confirmPromptPopUp = () => {
    setPromptPopUp(false);
  };

  // Python script has returned images and coordinates to be plotted
  useEffect(() => {
    if (props.originalImage !== "" && props.vectorImage !== "") {
      setCanStart(true);
      setWorkingPopUp(false);
    }
  }, [props.originalImage, props.vectorImage, props.imagePoints]);

  return (
    <div className="main-container">
      <PopUp
        title={"Resetting The Etch A Sketch Cursor"}
        body={
          "The Etch A Sketch Cursor is being reset to its start position. This popup will disappear once this process is complete."
        }
        button={""}
        show={resetPopUp}
      ></PopUp>
      <PopUp
        title={"Clear The Etch A Sketch"}
        body={
          "Shake the Etch A Sketch to clear the screen and hit confirm once you have done this and placed the Etch A Sketch back on a flat surface."
        }
        button={"Confirm"}
        show={shakePopUp}
        onHide={() => confirmShakePopUp()}
      ></PopUp>
      <PopUp
        title={"The Etch A Sketch Has Been Stopped"}
        body={
          "The Etch A Sketch was stopped using the hardware button on the device. The prior drawing cannot be continued, please draw something again and click the Start Drawing button when ready."
        }
        button={"Okay"}
        show={props.stopDrawing}
        onHide={() => {
          setTimeout(() => {
            window.location.reload(true);
          }, 1000);
        }}
      ></PopUp>
      <PopUp
        title={"The Etch A Sketch Has Been Stopped"}
        body={
          "The Etch A Sketch was stopped due to a system failure. The web interface will force refresh in 10 seconds and the hardware system must be reset manually."
        }
        button={"Okay"}
        show={props.watchdogTripped}
        onHide={() => {
          setTimeout(() => {
            window.location.reload(true);
          }, 10000);
        }}
      ></PopUp>
      <PopUp
        title={"The Drawing Is Complete"}
        body={
          "The drawing is complete, you can now show off your masterpiece or make another one."
        }
        button={"Okay"}
        show={sketchSuccessPopUp}
        onHide={() => {
          setSketchSuccessPopUp(false);
          setTimeout(() => {
            window.location.reload(true);
          }, 1000);
        }}
      ></PopUp>
      <PopUp
        title={"Enter a Prompt"}
        body={
          "Enter a descriptive (at least 40 character) prompt for DALL·E to produce something interesting."
        }
        button={"Okay"}
        show={propmtPopUp}
        onHide={() => confirmPromptPopUp()}
      ></PopUp>
      <PopUp
        title={"Working On It"}
        body={
          "DALL·E is hard at work to produce something interesting and so is an OpenCV Python script that creates the vector image."
        }
        button={""}
        show={workingPopUp}
      ></PopUp>
      <div className="header">
        <Dropdown>
          <Dropdown.Toggle
            variant="outline-info"
            id="dropdown-basic"
            disabled={!canInteract}
          >
            {selectedMode}
          </Dropdown.Toggle>
          <Dropdown.Menu>
            <Dropdown.Item
              href="#/action-1"
              onClick={() => {
                setCanStart(true);
                setSelectedMode("Draw");
              }}
            >
              Draw
            </Dropdown.Item>
            <Dropdown.Item
              href="#/action-2"
              onClick={() => {
                setCanStart(false);
                setSelectedMode("Print");
              }}
            >
              Print
            </Dropdown.Item>
          </Dropdown.Menu>
        </Dropdown>
        {selectedMode === "Draw" ? (
          <Button
            variant="outline-warning"
            id="eraseLast"
            onClick={() => onClickEraseLast()}
            disabled={!canInteract}
          >
            Erase Last Stroke
          </Button>
        ) : null}
        {selectedMode === "Draw" ? (
          <Button
            variant="outline-danger"
            id="erase"
            onClick={() => onClickErase()}
            disabled={!canInteract}
          >
            Erase Entire Canvas
          </Button>
        ) : null}
      </div>
      <div className="canvas-container">
        {selectedMode === "Print" ? (
          <Form.Group className="mb-3" controlId="formBasicEmail">
            <Form.Label>What would you like to sketch?</Form.Label>
            <Form.Control
              placeholder="An astronaut riding a dolphin in the style of Andy Warhol."
              onChange={(e) => setPrompt(e.target.value)}
            />
            <Form.Text className="text-muted">
              This prompt will be run through DALL·E, the resulting image and a
              vector sketch conversion will be displayed below.
            </Form.Text>
            <br></br>
            <br></br>
            {props.originalImage === "" ? (
              <Button
                variant="outline-primary"
                onClick={() => getImageFromOpenAI(prompt)}
              >
                Submit
              </Button>
            ) : null}
          </Form.Group>
        ) : null}
        {selectedMode === "Draw" ? (
          <canvas
            id="canvas"
            width="660px"
            height="480px"
            className="canvas-pane"
            ref={canvas}
            onClick={(e) => onCanvasMouseClick(e)}
            onMouseMove={(e) => onCanvasMouseMove(e)}
            onMouseUp={() => onCanvasMouseUp()}
            onMouseDown={() => onCanvasMouseDown()}
            onMouseLeave={() => onCanvasMouseLeave()}
            style={
              canInteract
                ? { backgroundColor: "white" }
                : { backgroundColor: "#f0f0f0" }
            }
          ></canvas>
        ) : null}
        <div className="progress-bar-container">
          {sketchProgressBar ? (
            <div style={{ marginBottom: "7px" }}>
              This is how long it takes to sketch your masterpiece:
            </div>
          ) : (
            ""
          )}
          {sketchProgressBar ? (
            <ProgressBar
              striped
              variant="success"
              animated
              now={
                selectedMode === "Draw"
                  ? ((props.drawProgress + 1) / canvasPoints.length) * 100
                  : ((props.drawProgress + 1) / props.imagePoints.length) * 100
              }
              style={{ height: "38px" }}
            />
          ) : null}
          <br></br>
        </div>
      </div>
      {selectedMode === "Print" && props.originalImage !== "" ? (
        <div className="picture-container">
          {selectedMode === "Print" && props.originalImage !== "" ? (
            <img src={props.originalImage} alt="produced by DALL·E"></img>
          ) : null}
          {selectedMode === "Print" && props.vectorImage !== "" ? (
            <img
              src={props.vectorImage}
              alt="vectorized, produced by DALL·E"
            ></img>
          ) : null}
        </div>
      ) : null}
      <div className="footer">
        <Button
          variant="outline-success"
          onClick={() => startDrawing()}
          disabled={!canStart}
        >
          Start Drawing
        </Button>
      </div>
    </div>
  );
}
