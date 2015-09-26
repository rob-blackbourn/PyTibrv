#include "stdafx.h"

#include "tibrv_exception.h"
#include "traits.h"
#include "types.h"
#include "messages.h"
#include "PyTibrvException.h"
#include "PyTibrvListener.h"
#include "PyTibrvNetTransport.h"
#include "PyTibrvQueue.h"
#include "PyTibrvCallback.h"

/*
** NetTransport
*/

static void
dealloc_method(PyTibrvListenerObject* self)
{
	if (self->listener != NULL)
	{
		TibrvStatus status = self->listener->destroy();
		delete self->listener;
		self->listener = NULL;
	}
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
new_method(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyTibrvListenerObject *self = (PyTibrvListenerObject*)type->tp_alloc(type, 0);

    if (self != NULL)
	{
		self->listener = new TibrvListener();
        if (self->listener == NULL)
		{
            Py_DECREF(self);
            return NULL;
		}
    }

    return (PyObject *)self;
}

static int
init_method(PyTibrvListenerObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *queue, *callback, *transport, *closure;
    char *subject = NULL;

    static char *kwlist[] = {"queue", "callback", "transport", "subject", "closure", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "OOOs|O", kwlist, &queue, &callback, &transport, &subject, &closure))
        return -1; 

	if (!PyTibrvQueue_Check(queue))
	{
		PyErr_SetString(PyExc_TypeError, "queue type invalid");
		return -1;
	}

	if (!PyTibrvNetTransport_Check(transport))
	{
		PyErr_SetString(PyExc_TypeError, "transport type invalid");
		return -1;
	}

	PyTibrvCallback* cb = new PyTibrvCallback(callback, false);
	TibrvStatus status = self->listener->create(PyTibrvQueue_AsQueue(queue), cb, PyTibrvNetTransport_AsTransport(transport), subject, closure);

    return 0;
}

static PyMemberDef type_members[] = {
    {NULL}  /* Sentinel */
};

static PyObject *
getter_subject(PyTibrvListenerObject *self, void *closure)
{
	const char* subject = NULL;
	TibrvStatus status = self->listener->getSubject(subject);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	return PyString_FromString(subject);
}

static PyObject *
getter_transport(PyTibrvListenerObject *self, void *closure)
{
	TibrvNetTransport* transport = (TibrvNetTransport*) self->listener->getTransport();
	return PyTibrvNetTransport_FromTransport(transport);
}

static PyGetSetDef type_getseters[] = {
    {"subject", (getter)getter_subject, NULL, "service", NULL},
    {"transport", (getter)getter_transport, NULL, "transport", NULL},
    {NULL}  /* Sentinel */
};

static PyObject *
typemethod_destroy(PyTibrvListenerObject* self)
{
	TibrvStatus status = self->listener->destroy();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef type_methods[] = {
    {"destroy", (PyCFunction)typemethod_destroy, METH_NOARGS, "Destroy a listener. "},
    {NULL}  /* Sentinel */
};

PyTypeObject PyTibrvListener_Type = {
    PyObject_HEAD_INIT(NULL)
    0,									/*ob_size*/
    "tibrv.listener",					/*tp_name*/
    sizeof(PyTibrvListenerObject),		/*tp_basicsize*/
    0,									/*tp_itemsize*/
    (destructor)dealloc_method,			/*tp_dealloc*/
    0,									/*tp_print*/
    0,									/*tp_getattr*/
    0,									/*tp_setattr*/
    0,									/*tp_compare*/
    0,									/*tp_repr*/
    0,									/*tp_as_number*/
    0,									/*tp_as_sequence*/
    0,									/*tp_as_mapping*/
    0,									/*tp_hash */
    0,									/*tp_call*/
    0,									/*tp_str*/
    0,									/*tp_getattro*/
    0,									/*tp_setattro*/
    0,									/*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/*tp_flags*/
    "Tibco Listener",					/* tp_doc */
    0,									/* tp_traverse */
    0,									/* tp_clear */
    0,									/* tp_richcompare */
    0,									/* tp_weaklistoffset */
    0,									/* tp_iter */
    0,									/* tp_iternext */
    type_methods,						/* tp_methods */
    type_members,						/* tp_members */
    type_getseters,						/* tp_getset */
    0,									/* tp_base */
    0,									/* tp_dict */
    0,									/* tp_descr_get */
    0,									/* tp_descr_set */
    0,									/* tp_dictoffset */
    (initproc)init_method,				/* tp_init */
    0,									/* tp_alloc */
    new_method,							/* tp_new */
};

void initPyTibrvListener(PyObject* module) 
{
    if (PyType_Ready(&PyTibrvListener_Type) < 0)
        return;
    Py_INCREF(&PyTibrvListener_Type);
    PyModule_AddObject(module, "listener", (PyObject*) &PyTibrvListener_Type);
}
