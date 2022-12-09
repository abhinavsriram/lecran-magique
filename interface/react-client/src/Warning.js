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
          {props.title}
        </Modal.Title>
      </Modal.Header>
      <Modal.Body>
        <p>{props.body}</p>
      </Modal.Body>
      <Modal.Footer>
        <Button variant="outline-success" onClick={props.onHide}>
          {props.button}
        </Button>
      </Modal.Footer>
    </Modal>
  );
}
