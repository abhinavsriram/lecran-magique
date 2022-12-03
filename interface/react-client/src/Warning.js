import Button from "react-bootstrap/Button";
import Modal from "react-bootstrap/Modal";

export function Warning(props) {
  return (
    <Modal
      {...props}
      size="lg"
      aria-labelledby="contained-modal-title-vcenter"
      centered
    >
      <Modal.Header closeButton>
        <Modal.Title id="contained-modal-title-vcenter">
          Clear The Etch A Sketch
        </Modal.Title>
      </Modal.Header>
      <Modal.Body>
        <p>
          Shake the Etch A Sketch to clear the screen and hit confirm once you
          have done this and placed the Etch A Sketch back on a flat surface.
        </p>
      </Modal.Body>
      <Modal.Footer>
        <Button variant="outline-success" onClick={props.onHide}>
          Confirm
        </Button>
      </Modal.Footer>
    </Modal>
  );
}
