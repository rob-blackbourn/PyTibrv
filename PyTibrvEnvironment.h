#pragma once

typedef struct {
    PyObject_HEAD
	PyTibrvQueueObject *defaultQueue;
} PyTibrvEnvironmentObject;

extern PyTypeObject PyTibrvEnvironment_Type;

extern PyObject* PyTibrvEnvironment_New(void);
#define PyTibrvEnvironment_Check(op) PyObject_TypeCheck(op, &PyTibrvEnvironment_Type)
#define PyTibrvEnvironment_CheckExact(op) ((op)->ob_type == &PyTibrvEnvironment_Type)

extern void initPyTibrvEnvironment(PyObject* module);