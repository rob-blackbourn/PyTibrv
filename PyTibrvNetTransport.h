#pragma once

typedef struct {
    PyObject_HEAD
	TibrvNetTransport* transport;
} PyTibrvNetTransportObject;

extern PyTypeObject PyTibrvNetTransport_Type;

PyObject *PyTibrvNetTransport_FromTransport(TibrvNetTransport *transport);
TibrvNetTransport *PyTibrvNetTransport_AsTransport(PyObject *transport);

#define PyTibrvNetTransport_Check(op) PyObject_TypeCheck(op, &PyTibrvNetTransport_Type)
#define PyTibrvNetTransport_CheckExact(op) ((op)->ob_type == &PyTibrvNetTransport_Type)

void initPyTibrvNetTransport(PyObject* module);
