#include "stdafx.h"

#include "PyTibrvException.h"
#include "PyTibrvQueue.h"
#include "PyTibrvEnvironment.h"

static void
dealloc_method(PyTibrvEnvironmentObject* self)
{
	if (self->defaultQueue != NULL)
	{
		Py_DECREF(self->defaultQueue);
		self->defaultQueue = NULL;
	}

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
new_method(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyTibrvEnvironmentObject *self = (PyTibrvEnvironmentObject *)type->tp_alloc(type, 0);
	self->defaultQueue = (PyTibrvQueueObject*) PyTibrvQueue_FromQueue(Tibrv::defaultQueue());

    return (PyObject *)self;
}

static int
init_method(PyTibrvEnvironmentObject *self, PyObject *args, PyObject *kwds)
{
    return 0;
}

static PyMemberDef type_members[] = {
    {NULL}  /* Sentinel */
};

static PyObject *
getter_getDefaultQueue(PyTibrvEnvironmentObject *self, void *closure)
{
    Py_INCREF(self->defaultQueue);
    return (PyObject*) self->defaultQueue;
}

static PyGetSetDef type_getseters[] = {
    {"defaultqueue", (getter)getter_getDefaultQueue, NULL, "Extract the default queue object. ", NULL},
    {NULL}  /* Sentinel */
};

static PyObject *
typemethod_open(PyTibrvEnvironmentObject* self)
{
	TibrvStatus status = Tibrv::open();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
typemethod_close(PyTibrvEnvironmentObject* self)
{
	TibrvStatus status = Tibrv::close();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
typemethod_version(PyTibrvEnvironmentObject* self)
{
    return PyString_FromString(Tibrv::version());
}

static PyMethodDef type_methods[] = {
    {"close", (PyCFunction)typemethod_close, METH_NOARGS, "Stop and destroy Rendezvous internal machinery."},
    {"open", (PyCFunction)typemethod_open, METH_NOARGS, "Start Rendezvous internal machinery."},
    {"version", (PyCFunction)typemethod_version, METH_NOARGS, "Identify the Rendezvous API release number."},
    {NULL}  /* Sentinel */
};

PyTypeObject PyTibrvEnvironment_Type = {
    PyObject_HEAD_INIT(NULL)
    0,									/*ob_size*/
    "tibrv.environment",				/*tp_name*/
    sizeof(PyTibrvEnvironmentObject),				/*tp_basicsize*/
    0,									/*tp_itemsize*/
    (destructor)dealloc_method,	/*tp_dealloc*/
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
    "Tibco Environment",				/* tp_doc */
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

PyObject *
PyTibrvEnvironment_New(void)
{
	return PyTibrvEnvironment_Type.tp_new(&PyTibrvEnvironment_Type, NULL, NULL);
}

void initPyTibrvEnvironment(PyObject* module)
{
    if (PyType_Ready(&PyTibrvEnvironment_Type) < 0)
        return;
    Py_INCREF(&PyTibrvEnvironment_Type);
    PyModule_AddObject(module, "environment", (PyObject *)&PyTibrvEnvironment_Type);
}

