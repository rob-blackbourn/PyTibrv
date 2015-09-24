#pragma once

typedef struct {
    PyObject_HEAD
	TibrvListener* listener;
} PyTibrvListenerObject;

extern PyTypeObject PyTibrvListener_Type;

#define PyTibrvListener_Check(op) PyObject_TypeCheck(op, &PyTibrvListener_Type)
#define PyTibrvListener_CheckExact(op) ((op)->ob_type == &PyTibrvListener_Type)

void initPyTibrvListener(PyObject* module);
