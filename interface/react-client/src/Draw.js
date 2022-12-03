import "./Draw.css";
import "bootstrap/dist/css/bootstrap.css";
import Button from "react-bootstrap/Button";
import Dropdown from "react-bootstrap/Dropdown";
import ProgressBar from "react-bootstrap/ProgressBar";
import { useEffect, useState } from "react";

export function DrawingPane() {
  const [selectedMode, setSelectedMode] = useState("Select Mode");
  const [displayDrawingProgressBar, setDisplayDrawingProgressBar] =
    useState(false);
  const [displayStoppingProgressBar, setDisplayStoppingProgressBar] =
    useState(false);

  var canvas;

  var canvasPoints = {
    clickX: [0],
    clickY: [0],
    paint: false,
    drawPoints: function (cvs) {
      //cvs = document.getElementById('theCanvas');
      var ctx = cvs.getContext("2d");
      ctx.clearRect(0, 0, cvs.width, cvs.height);
      ctx.strokeStyle = "black";
      ctx.beginPath();
      ctx.moveTo(this.clickX[0], this.clickY[0]);
      for (var i = 0; i < this.clickX.length; i++) {
        ctx.lineTo(this.clickX[i], this.clickY[i]);
        ctx.moveTo(this.clickX[i], this.clickY[i]);
      }
      ctx.stroke();
      ctx.closePath();
    },
    // Only add new point if it differs from the last one
    addPoint: function (x, y) {
      if (
        x != this.clickX[this.nPoints() - 1] ||
        y != this.clickY[this.nPoints() - 1]
      ) {
        this.clickX.push(x);
        this.clickY.push(y);
      }
    },
    clearPoints: function () {
      this.clickX = [0];
      this.clickY = [0];
      this.paint = false;
    },
    nPoints: function () {
      return this.clickX.length;
    },
    popPoint: function () {
      return [this.clickX.pop(), this.clickY.pop()];
    },
    reset: function () {
      this.clickX = [0];
      this.clickY = [0];
    },
    setPaint: function (p) {
      this.paint = p;
    },
  };

  useEffect(() => {
    canvas = document.getElementById("canvas");
    canvas.addEventListener("click", onCanvasMouseClick(canvasPoints));
    canvas.addEventListener("mousedown", onCanvasMouseDown(canvasPoints));
    canvas.addEventListener("mouseup", onCanvasMouseUp(canvasPoints));
    canvas.addEventListener("mousemove", onCanvasMouseMove(canvasPoints));
    canvas.addEventListener("mouseleave", onCanvasMouseLeave(canvasPoints));
    document
      .getElementById("eraseLast")
      .addEventListener("click", onClickEraseLast(canvas, canvasPoints));
    document
      .getElementById("erase")
      .addEventListener("click", onClickErase(canvas, canvasPoints));
  }, []);

  // Canvas drawing functions

  // Scope preserving wrapper for canvas callback function
  const onCanvasMouseClick = (canvasPoints) => {
    return function (e) {
      canvasPoints.addPoint(
        e.pageX - this.offsetLeft,
        e.pageY - this.offsetTop
      );
      canvasPoints.drawPoints(this);
    };
  };

  // Scope preserving wrapper for canvas callback function
  const onCanvasMouseDown = (canvasPoints) => {
    return function (e) {
      canvasPoints.setPaint(true);
      canvasPoints.drawPoints(this);
    };
  };

  // Scope preserving wrapper for canvas callback function
  const onCanvasMouseUp = (canvasPoints) => {
    return function (e) {
      canvasPoints.setPaint(false);
      canvasPoints.drawPoints(this);
    };
  };

  // Scope preserving wrapper for canvas callback function
  const onCanvasMouseMove = (canvasPoints) => {
    return function (e) {
      if (canvasPoints.paint) {
        canvasPoints.addPoint(
          e.pageX - this.offsetLeft,
          e.pageY - this.offsetTop
        );
        canvasPoints.drawPoints(this);
      }
    };
  };

  // Scope preserving wrapper for canvas callback function
  const onCanvasMouseLeave = (canvasPoints) => {
    return function (e) {
      canvasPoints.setPaint(false);
    };
  };

  // Scope preserving wrapper for canvas callback functions
  const onClickErase = (canvas, canvasPoints) => {
    return function (e) {
      canvasPoints.reset();
      canvasPoints.setPaint(false);
      canvasPoints.drawPoints(canvas);
    };
  };

  const onClickEraseLast = (canvas, canvasPoints) => {
    return function (e) {
      if (canvasPoints.nPoints() > 1) {
        canvasPoints.popPoint();
        canvasPoints.drawPoints(canvas);
      }
    };
  };

  const startDrawing = () => {
    setDisplayDrawingProgressBar(true);
  };

  const stopDrawing = () => {
    setDisplayDrawingProgressBar(false);
    setDisplayStoppingProgressBar(true);
  };

  return (
    <div className="main-container">
      <div className="header">
        <Dropdown>
          <Dropdown.Toggle variant="outline-info" id="dropdown-basic">
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
        <Button variant="outline-warning" id="eraseLast">
          Erase Last Stroke
        </Button>
        <Button variant="outline-danger" id="erase">
          Erase Entire Canvas
        </Button>
      </div>
      <div className="canvas-container">
        <canvas
          id="canvas"
          width="600px"
          height="400px"
          className="canvas-pane"
        ></canvas>
        <div className="progress-bar-container">
          {displayDrawingProgressBar ? "Drawing Progress: " : ""}
          {displayDrawingProgressBar ? (
            <ProgressBar
              striped
              variant="success"
              animated
              now={45}
              style={{ height: "30px" }}
            />
          ) : null}
          <br></br>
          {displayStoppingProgressBar ? "Stopping Progress: " : ""}
          {displayStoppingProgressBar ? (
            <ProgressBar
              striped
              variant="danger"
              animated
              now={45}
              style={{ height: "30px" }}
            />
          ) : null}
        </div>
      </div>
      <div className="footer">
        <Button variant="outline-success" onClick={() => startDrawing()}>
          Start Drawing
        </Button>
        <Button variant="outline-danger" onClick={() => stopDrawing()}>
          Stop Drawing
        </Button>
      </div>
    </div>
  );
}
