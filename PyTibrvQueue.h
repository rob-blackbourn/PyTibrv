#pragma once

typedef struct {
    PyObject_HEAD
	TibrvQueue *queue;
} PyTibrvQueueObject;

extern PyTypeObject PyTibrvQueue_Type;

extern PyObject *PyTibrvQueue_FromQueue(TibrvQueue *queue);
extern TibrvQueue *PyTibrvQueue_AsQueue(PyObject *queue);

#define PyTibrvQueue_Check(op) PyObject_TypeCheck(op, &PyTibrvQueue_Type)
#define PyTibrvQueue_CheckExact(op) ((op)->ob_type == &PyTibrvQueue_Type)

extern void initPyTibrvQueue(PyObject* module);
