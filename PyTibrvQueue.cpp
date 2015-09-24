#include "stdafx.h"

#include "PyTibrvException.h"
#include "PyTibrvQueue.h"

static void
Queue_dealloc(PyTibrvQueueObject* self)
{
	if (self->queue)
	{
		if (self->queue->isValid() == TIBRV_TRUE && self->queue != Tibrv::defaultQueue())
		{
			self->queue->destroy();
			delete self->queue;
		}
	}
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
Queue_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyTibrvQueueObject *self = (PyTibrvQueueObject *)type->tp_alloc(type, 0);
    if (self != NULL)
	{
		self->queue = new TibrvQueue();
        if (self->queue == NULL)
		{
            Py_DECREF(self);
            return NULL;
        }
    }

    return (PyObject *)self;
}

static int
Queue_init(PyTibrvQueueObject *self, PyObject *args, PyObject *kwds)
{
	char* name;
	int priority = TIBRVQUEUE_DEFAULT_PRIORITY;
	PyObject* limit_policy;

    static char *kwlist[] = {"name", "priority", "limitpolicy", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "s|iO", kwlist, &name, &priority, &limit_policy))
        return -1; 

	self->queue->setName(name);
	self->queue->setPriority(priority);
	if (limit_policy)
	{
		// TODO: should return error if tuple wrong length or items incorrect types
		if (PySequence_Check(limit_policy) && PySequence_Length(limit_policy) == 3)
		{
			tibrvQueueLimitPolicy policy = static_cast<tibrvQueueLimitPolicy>(PyInt_AsLong(PySequence_GetItem(limit_policy, 0)));
			int max_events = static_cast<int>(PyInt_AsLong(PySequence_GetItem(limit_policy, 1)));
			int discard_amount = static_cast<int>(PyInt_AsLong(PySequence_GetItem(limit_policy, 2)));

			self->queue->setLimitPolicy(policy, max_events, discard_amount);
		}
	}

    return 0;
}

static PyMemberDef Queue_members[] = {
    {NULL}  /* Sentinel */
};

static PyObject *
Queue_getName(PyTibrvQueueObject *self, void *closure)
{
	const char* queue_name;
	TibrvStatus status = self->queue->getName(queue_name);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

    return PyString_FromString(queue_name);
}

static PyObject *
Queue_getPriority(PyTibrvQueueObject *self, void *closure)
{
	tibrv_u32 priority;
	TibrvStatus status = self->queue->getPriority(priority);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}
	return PyInt_FromLong(priority);
}

static PyObject *
Queue_getLimitPolicy(PyTibrvQueueObject *self, void *closure)
{
	tibrvQueueLimitPolicy policy;
	tibrv_u32 max_events, discard_amount;
	TibrvStatus status = self->queue->getLimitPolicy(policy, max_events, discard_amount);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}
	PyObject* tuple = PyTuple_New(3);
	if (tuple == NULL)
	{
		PyErr_SetFromErrno(tibrv_error);
		return NULL;
	}
	PyTuple_SetItem(tuple, 0, PyInt_FromLong(static_cast<long>(policy)));
	PyTuple_SetItem(tuple, 1, PyInt_FromLong(max_events));
	PyTuple_SetItem(tuple, 2, PyInt_FromLong(discard_amount));

	return tuple;
}
static int
Queue_setLimitPolicy(PyTibrvQueueObject *self, PyObject *value, void *closure)
{
	if (value == NULL)
	{
		PyErr_SetString(PyExc_TypeError, "Cannot have an empty policy");
		return -1;
	}

	if (!PySequence_Check(value))
	{
		PyErr_SetString(PyExc_TypeError, "invalid type");
		return -1;
	}

	if (PySequence_Length(value) != 3)
	{
		PyErr_SetString(PyExc_ValueError, "expected sequence with three items");
		return -1;
	}

	PyObject* policy = PySequence_GetItem(value, 0);
	if (!PyInt_Check(policy))
	{
		PyErr_SetString(PyExc_TypeError, "expected first item (policy) to be of type int");
		return -1;
	}

	PyObject* max_events = PySequence_GetItem(value, 1);
	if (!PyInt_Check(max_events))
	{
		PyErr_SetString(PyExc_TypeError, "expected first item (max events) to be of type int");
		return -1;
	}

	PyObject *discard_amount = PySequence_GetItem(value, 2);
	if (!PyInt_Check(discard_amount))
	{
		PyErr_SetString(PyExc_TypeError, "expected first item (discard amount) to be of type int");
		return -1;
	}

	tibrvQueueLimitPolicy _policy = static_cast<tibrvQueueLimitPolicy>(PyInt_AsLong(policy));
	tibrv_u32 _max_events = static_cast<tibrv_u32>(PyInt_AsLong(max_events));
	tibrv_u32 _discard_amount = static_cast<tibrv_u32>(PyInt_AsLong(discard_amount));

	self->queue->setLimitPolicy(_policy, _max_events, _discard_amount);

	return 0;
}

static PyGetSetDef Queue_getseters[] = {
    {"name", (getter)Queue_getName, (setter)NULL, "name", NULL},
    {"priority", (getter)Queue_getPriority, (setter)NULL, "priority", NULL},
    {"limitpolicy", (getter)Queue_getLimitPolicy, (setter)Queue_setLimitPolicy, "limit policy", NULL},
    {NULL}  /* Sentinel */
};

static PyObject *
Queue_Count(PyTibrvQueueObject* self)
{
	tibrv_u32 num_events;
	TibrvStatus status = self->queue->getCount(num_events);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	return PyInt_FromLong(num_events);
}

static PyObject *
Queue_Create(PyTibrvQueueObject* self)
{
	TibrvStatus status = self->queue->create();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
Queue_Destroy(PyTibrvQueueObject* self)
{
	TibrvStatus status = self->queue->destroy();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
Queue_Dispatch(PyTibrvQueueObject* self)
{
	TibrvStatus status = self->queue->dispatch();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
Queue_IsValid(PyTibrvQueueObject* self)
{
	tibrv_bool is_valid = self->queue->isValid();
	PyObject* _is_valid = (is_valid == TIBRV_TRUE ? Py_True : Py_False);
	Py_INCREF(_is_valid);
	return _is_valid;
}

static PyObject *
Queue_Poll(PyTibrvQueueObject* self)
{
	TibrvStatus status = self->queue->poll();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
Queue_TimedDispatch(PyTibrvQueueObject* self, PyObject *args, PyObject *kwds)
{
    double timeout;

    static char *kwlist[] = {"timeout", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "d", kwlist, &timeout))
        return NULL; 

	TibrvStatus status = self->queue->timedDispatch(timeout);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef Queue_methods[] = {
    {"count", (PyCFunction)Queue_Count, METH_NOARGS, "Extract the number of events in a queue."},
    {"create", (PyCFunction)Queue_Create, METH_NOARGS, "Create a queue."},
    {"destroy", (PyCFunction)Queue_Destroy, METH_NOARGS, "Destroy a queue."},
    {"dispatch", (PyCFunction)Queue_Dispatch, METH_NOARGS, "Dispatch an event; if no event is ready, block."},
    {"isvalid", (PyCFunction)Queue_IsValid, METH_NOARGS, "Test validity of a queue. "},
    {"poll", (PyCFunction)Queue_Poll, METH_NOARGS, "Dispatch an event, if possible."},
	{"timeddispatch", (PyCFunction)Queue_TimedDispatch, METH_VARARGS | METH_KEYWORDS, "Dispatch an event, but if no event is ready to dispatch, limit the time that this call blocks while waiting for an event."},
	{NULL}  /* Sentinel */
};

PyTypeObject PyTibrvQueue_Type = {
    PyObject_HEAD_INIT(NULL)
    0,							/*ob_size*/
    "tibrv.Queue",				/*tp_name*/
    sizeof(PyTibrvQueueObject),	/*tp_basicsize*/
    0,							/*tp_itemsize*/
    (destructor)Queue_dealloc,	/*tp_dealloc*/
    0,							/*tp_print*/
    0,							/*tp_getattr*/
    0,							/*tp_setattr*/
    0,							/*tp_compare*/
    0,							/*tp_repr*/
    0,							/*tp_as_number*/
    0,							/*tp_as_sequence*/
    0,							/*tp_as_mapping*/
    0,							/*tp_hash */
    0,							/*tp_call*/
    0,							/*tp_str*/
    0,							/*tp_getattro*/
    0,							/*tp_setattro*/
    0,							/*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Queue objects",			/* tp_doc */
    0,							/* tp_traverse */
    0,							/* tp_clear */
    0,							/* tp_richcompare */
    0,							/* tp_weaklistoffset */
    0,							/* tp_iter */
    0,							/* tp_iternext */
    Queue_methods,				/* tp_methods */
    Queue_members,				/* tp_members */
    Queue_getseters,			/* tp_getset */
    0,							/* tp_base */
    0,							/* tp_dict */
    0,							/* tp_descr_get */
    0,							/* tp_descr_set */
    0,							/* tp_dictoffset */
    (initproc)Queue_init,		/* tp_init */
    0,							/* tp_alloc */
    Queue_new,					/* tp_new */
};

PyObject *
PyTibrvQueue_FromQueue(TibrvQueue *queue)
{
	PyTibrvQueueObject *q = (PyTibrvQueueObject*)PyTibrvQueue_Type.tp_new(&PyTibrvQueue_Type, NULL, NULL);
	if (q != NULL)
	{
		q->queue = queue;
	}
	return (PyObject *) q;
}

TibrvQueue *
PyTibrvQueue_AsQueue(PyObject *queue)
{
	PyTibrvQueueObject *q = (PyTibrvQueueObject *)queue;
	if (q != NULL)
		return q->queue;
	else
		return NULL;
}

static struct { char* name; int id; } IntVars[] = {
	{"QUEUE_DISCARD_NONE",		TIBRVQUEUE_DISCARD_NONE},
	{"QUEUE_DISCARD_NEW",		TIBRVQUEUE_DISCARD_NEW},
	{"QUEUE_DISCARD_FIRST",		TIBRVQUEUE_DISCARD_FIRST},
	{"QUEUE_DISCARD_LAST",		TIBRVQUEUE_DISCARD_LAST},
};

static struct { char* name; double id; } DblVars[] = {
	{"WAIT_FOREVER",			TIBRV_WAIT_FOREVER},
	{"NO_WAIT",					TIBRV_NO_WAIT},
};

void initPyTibrvQueue(PyObject* module)
{
    if (PyType_Ready(&PyTibrvQueue_Type) < 0)
        return;

    Py_INCREF(&PyTibrvQueue_Type);
    PyModule_AddObject(module, "Queue", (PyObject*)&PyTibrvQueue_Type);

	// Setup all the integer constants
	for (size_t i = 0; i < sizeof(IntVars) / sizeof(IntVars[0]); ++i)
		PyModule_AddIntConstant(module, IntVars[i].name, IntVars[i].id);

	// Setup all the double constants
	for (size_t i = 0; i < sizeof(DblVars) / sizeof(DblVars[0]); ++i)
	{
		PyObject* constant = PyFloat_FromDouble(DblVars[i].id);
		PyModule_AddObject(module, DblVars[i].name, constant);
		Py_INCREF(constant);
	}
}
