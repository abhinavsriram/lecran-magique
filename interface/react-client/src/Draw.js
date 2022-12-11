import "./Draw.css";
import "bootstrap/dist/css/bootstrap.css";
import { useEffect, useRef, useState } from "react";
import Form from "react-bootstrap/Form";
import Button from "react-bootstrap/Button";
import Dropdown from "react-bootstrap/Dropdown";
import ProgressBar from "react-bootstrap/ProgressBar";
import { Warning } from "./Warning";

import { Configuration, OpenAIApi } from "openai";
const OPENAI_API_KEY = "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";
const configuration = new Configuration({
  apiKey: OPENAI_API_KEY,
});
const openai = new OpenAIApi(configuration);

class CanvasPoint {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
}
var canvasPoints = [new CanvasPoint(0, 0)];
var canvasDrawing = false;
var arduinoInstructions = [];

const BUFFER_SIZE = 1;

export function DrawingPane(props) {
  // STATE VARIABLES FOR DEFAULT
  const [selectedMode, setSelectedMode] = useState("Draw");
  const [resetProgressBar, setResetProgressBar] = useState(false);
  const [drawProgressBar, setDrawProgressBar] = useState(false);
  const [showShakeWarning, setShowShakeWarning] = useState(false);
  const [canInteract, setCanInteract] = useState(true);
  const [canStart, setCanStart] = useState(true);
  const [canStop, setCanStop] = useState(false);
  const canvas = useRef(null);

  // STATE VARIABLES FOR CAPSTONE
  const [prompt, setPrompt] = useState("");
  const [showPromptWarning, setShowPromptWarning] = useState(false);
  const [showWorkingOnIt, setShowWorkingOnIt] = useState(false);

  // CANVAS UTILITY FUNCTIONS
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
  const startDrawing = () => {
    startReset();
    // SR: S - Server, R - Reset Instruction
    props.socket.send("SR");
    setCanInteract(false);
    setCanStart(false);
    setCanStop(false);
  };

  const startReset = () => {
    setResetProgressBar(true);
  };

  const shakeWarning = () => {
    setShowShakeWarning(true);
  };

  // hide reset progress bar once reset is complete
  useEffect(() => {
    if (props.resetProgress === 100) {
      setResetProgressBar(false);
      shakeWarning();
    }
  }, [props.resetProgress]);

  const confirmShakeWarning = () => {
    setShowShakeWarning(false);
    setCanStop(true);
    // SD: S - Server, D - Draw Instruction
    if (selectedMode === "Draw") {
      props.socket.send("SD " + String(canvasPoints.length));
    } else if (selectedMode === "Print") {
      props.socket.send("SD " + String(props.imagePoints.length));
    }
    reallyStartDrawing();
  };

  const reallyStartDrawing = () => {
    setDrawProgressBar(true);
    translateToArduino();
    // send the first BUFFER_SIZE instructions
    for (var i = 0; i < BUFFER_SIZE; i++) {
      if (arduinoInstructions[i]) {
        props.socket.send(arduinoInstructions[i]);
        arduinoInstructions.shift();
      }
    }
  };

  // send subsequent batches of BUFFER_SIZE instructions
  useEffect(() => {
    if (props.drawProgress % BUFFER_SIZE === 0) {
      for (var i = 0; i < BUFFER_SIZE; i++) {
        if (arduinoInstructions[i]) {
          setTimeout(() => {
            props.socket.send(arduinoInstructions[i]);
            arduinoInstructions.shift();
          }, 1000);
        }
      }
    }
  }, [props.drawProgress]);

  const stopDrawing = () => {
    setDrawProgressBar(false);
    setCanInteract(true);
    setCanStart(true);
    setCanStop(false);
    canvasDrawing = false;
    clearAllPoints();
    drawPointsContiguous();
    // SS: S - Server, S - Stop Instruction
    props.socket.send("SS");
  };

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
            Math.round(props.imagePoints[i][0]) +
            " " +
            Math.round(props.imagePoints[i][1])
        );
      }
    }
  };

  // CAPSTONE FEATURE FUNCTIONS
  const getImageFromOpenAI = async (prompt) => {
    if (prompt === "" || prompt.length < 30) {
      setShowPromptWarning(true);
    } else {
      // try {
      //   const response = await openai.createImage({
      //     prompt: prompt,
      //     n: 1,
      //     size: "256x256",
      //   });
      //   const image_url = response.data.data[0].url;
      //   console.log(image_url);
      // } catch (error) {
      //   if (error.response) {
      //     console.log(error.response.status);
      //     console.log(error.response.data);
      //   } else {
      //     console.log(error.message);
      //   }
      // }
      const image_url = "https://picsum.photos/256/256";
      // everything after this must be called in a callback once openai responds
      // run python script to get vector image
      props.socket.send("PY " + image_url);
      setShowWorkingOnIt(true);
    }
  };

  const confirmPromptWarning = () => {
    setShowPromptWarning(false);
  };

  // images are retrieved from Python script
  useEffect(() => {
    if (props.originalImage !== "" && props.vectorImage !== "") {
      setCanStart(true);
      setShowWorkingOnIt(false);
    }
  }, [props.originalImage, props.vectorImage, props.imagePoints]);

  return (
    <div className="main-container">
      <Warning
        title={"Working On It"}
        body={
          "DALL·E is hard at work to produce something interesting and so is an OpenCV Python script that creates the vector image."
        }
        button={""}
        show={showWorkingOnIt}
      ></Warning>
      <Warning
        title={"Enter a Prompt"}
        body={
          "Enter a descriptive (at least 40 character) prompt for DALL·E to produce something interesting."
        }
        button={"Okay"}
        show={showPromptWarning}
        onHide={() => confirmPromptWarning()}
      ></Warning>
      <Warning
        title={"Clear The Etch A Sketch"}
        body={
          "Shake the Etch A Sketch to clear the screen and hit confirm once you have done this and placed the Etch A Sketch back on a flat surface."
        }
        button={"Confirm"}
        show={showShakeWarning}
        onHide={() => confirmShakeWarning()}
      ></Warning>
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
          {resetProgressBar ? (
            <div style={{ marginBottom: "7px" }}>
              This is how long it takes to reset:
            </div>
          ) : (
            ""
          )}
          {resetProgressBar ? (
            <ProgressBar
              striped
              variant="warning"
              animated
              now={props.resetProgress}
              style={{ height: "38px" }}
            />
          ) : null}
          {drawProgressBar ? (
            <div style={{ marginBottom: "7px" }}>
              This is how long it takes to draw:
            </div>
          ) : (
            ""
          )}
          {drawProgressBar ? (
            <ProgressBar
              striped
              variant="success"
              animated
              now={
                selectedMode === "Draw"
                  ? (props.drawProgress / canvasPoints.length) * 100
                  : (props.drawProgress / props.imagePoints.length) * 100
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
        <Button
          variant="outline-danger"
          onClick={() => stopDrawing()}
          disabled={!canStop}
        >
          Stop Drawing
        </Button>
      </div>
    </div>
  );
}
