import "./Draw.css";
import "bootstrap/dist/css/bootstrap.css";
import { useEffect, useRef, useState } from "react";
import Button from "react-bootstrap/Button";
import Dropdown from "react-bootstrap/Dropdown";
import ProgressBar from "react-bootstrap/ProgressBar";
import { Warning } from "./Warning";

export function DrawingPane(props) {
  // STATE VARIABLES

  const [selectedMode, setSelectedMode] = useState("Draw");
  const [resetProgressBar, setResetProgressBar] = useState(false);
  const [drawProgressBar, setDrawProgressBar] = useState(false);
  const [showShakeWarning, setShowShakeWarning] = useState(false);
  const [canInteract, setCanInteract] = useState(true);

  // CANVAS GLOBAL VARIABLES

  const canvas = useRef(null);
  class CanvasPoint {
    constructor(x, y) {
      this.x = x;
      this.y = y;
    }
  }
  var canvasPoints = [new CanvasPoint(0, 0)];
  var canvasDrawing = false;

  // ARDUINO GLOBAL VARIABLES

  var arduinoInstructions = [];

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
  };

  const startReset = () => {
    setResetProgressBar(true);
  };

  const shakeWarning = () => {
    setShowShakeWarning(true);
  };

  useEffect(() => {
    if (props.resetProgress === 100) {
      setResetProgressBar(false);
      shakeWarning();
    }
  }, [props.resetProgress]);

  const confirmShakeWarning = () => {
    setShowShakeWarning(false);
    // SD: S - Server, D - Draw Instruction
    props.socket.send("SD");
    reallyStartDrawing();
  };

  const reallyStartDrawing = () => {
    setDrawProgressBar(true);
    translateToArduino();
    var instruction;
    for (instruction in arduinoInstructions) {
      props.socket.write(instruction);
    }
  };

  const stopDrawing = () => {
    setDrawProgressBar(false);
    setCanInteract(true);
  };

  // TRANSLATE TO ARDUINO INSTRUCTIONS

  const translateToArduino = () => {
    arduinoInstructions = [];
    for (var i = 0; i < canvasPoints.length; i++) {
      // SL: S - Server, L - Line Instruction
      arduinoInstructions.push(
        "SL" +
          " " +
          Math.round(canvasPoints[i].x) +
          " " +
          Math.round(canvasPoints[i].y)
      );
    }
  };

  return (
    <div className="main-container">
      <Warning
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
                setSelectedMode("Draw");
              }}
            >
              Draw
            </Dropdown.Item>
            <Dropdown.Item
              href="#/action-2"
              onClick={() => {
                setSelectedMode("Print");
              }}
            >
              Print
            </Dropdown.Item>
          </Dropdown.Menu>
        </Dropdown>
        <Button
          variant="outline-warning"
          id="eraseLast"
          onClick={() => onClickEraseLast()}
          disabled={!canInteract}
        >
          Erase Last Stroke
        </Button>
        <Button
          variant="outline-danger"
          id="erase"
          onClick={() => onClickErase()}
          disabled={!canInteract}
        >
          Erase Entire Canvas
        </Button>
      </div>
      <div className="canvas-container">
        <canvas
          id="canvas"
          width="600px"
          height="400px"
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
        <div className="progress-bar-container">
          <span style={{ marginBottom: "10px" }}>
            {resetProgressBar ? "Reset Progress: " : ""}
          </span>
          {resetProgressBar ? (
            <ProgressBar
              striped
              variant="warning"
              animated
              now={props.resetProgress}
              style={{ height: "30px" }}
            />
          ) : null}
          <span style={{ marginBottom: "10px" }}>
            {drawProgressBar ? "Drawing Progress: " : ""}
          </span>
          {drawProgressBar ? (
            <ProgressBar
              striped
              variant="success"
              animated
              now={props.drawProgress}
              style={{ height: "30px" }}
            />
          ) : null}
          <br></br>
        </div>
      </div>
      <div className="footer">
        <Button
          variant="outline-success"
          onClick={() => startDrawing()}
          disabled={!canInteract}
        >
          Start Drawing
        </Button>
        <Button
          variant="outline-danger"
          onClick={() => stopDrawing()}
          disabled={canInteract}
        >
          Stop Drawing
        </Button>
      </div>
    </div>
  );
}
