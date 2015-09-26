#include "stdafx.h"

#include "tibrv_exception.h"
#include "traits.h"
#include "types.h"
#include "messages.h"

#include "PyTibrvException.h"
#include "PyTibrvNetTransport.h"

/*
** NetTransport
*/

static void
dealloc_method(PyTibrvNetTransportObject* self)
{
	if (self->transport != NULL)
	{
		if (self->transport->isValid() == TIBRV_TRUE)
			self->transport->destroy();

		delete self->transport;
		self->transport = NULL;
	}
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
new_method(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyTibrvNetTransportObject *self = (PyTibrvNetTransportObject*)type->tp_alloc(type, 0);

    if (self != NULL)
	{
		self->transport = new TibrvNetTransport();
        if (self->transport == NULL)
		{
            Py_DECREF(self);
            return NULL;
		}
    }

    return (PyObject *)self;
}

static int
init_method(PyTibrvNetTransportObject *self, PyObject *args, PyObject *kwds)
{
    char *service = NULL, *network = NULL, *daemon = NULL, *license = NULL;

    static char *kwlist[] = {"service", "network", "daemon", "license", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "sss|s", kwlist, &service, &network, &daemon, &license))
        return -1; 

	TibrvStatus status =
		license == NULL
			? self->transport->create(service, network, daemon)
			: self->transport->createLicensed(service, network, daemon, license);

    return 0;
}

static PyMemberDef type_members[] = {
    {NULL}  /* Sentinel */
};

static PyObject *
getmember_service(PyTibrvNetTransportObject *self, void *closure)
{
	const char* service = NULL;
	TibrvStatus status = self->transport->getService(service);
	return PyString_FromString(service);
}

static PyObject *
getmember_network(PyTibrvNetTransportObject *self, void *closure)
{
	const char* network = NULL;
	TibrvStatus status = self->transport->getNetwork(network);
	return PyString_FromString(network);
}

static PyObject *
getmember_daemon(PyTibrvNetTransportObject *self, void *closure)
{
	const char* daemon = NULL;
	TibrvStatus status = self->transport->getDaemon(daemon);
	return PyString_FromString(daemon);
}

static PyObject *
getmember_description(PyTibrvNetTransportObject *self, void *closure)
{
	const char* description = NULL;
	TibrvStatus status = self->transport->getDescription(&description);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	if (description != NULL)
		return PyString_FromString(description);
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static int
setmember_description(PyTibrvNetTransportObject *self, PyObject *value, void *closure)
{
	if (value == NULL)
	{
		PyErr_SetString(PyExc_TypeError, "Cannot delete the description");
		return -1;
	}
  
	if (! PyString_Check(value))
	{
		PyErr_SetString(PyExc_TypeError, "The last attribute value must be a string");
		return -1;
	}

	char* description = PyString_AsString(value);
	TibrvStatus status = self->transport->setDescription(description);

	return 0;
}

static PyGetSetDef type_getseters[] = {
    {"service", (getter)getmember_service, NULL, "service", NULL},
    {"network", (getter)getmember_network, NULL, "network", NULL},
    {"daemon", (getter)getmember_daemon, NULL, "daemon", NULL},
    {"description", (getter)getmember_description, (setter)setmember_description, "description", NULL},
    {NULL}  /* Sentinel */
};

static PyObject *
typemethod_createinbox(PyTibrvNetTransportObject* self)
{
	char subject[TIBRV_SUBJECT_MAX];
	TibrvStatus status = self->transport->createInbox(subject, TIBRV_SUBJECT_MAX);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	if (subject != NULL)
	{
		return PyString_FromString(subject);
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static PyObject *
typemethod_destroy(PyTibrvNetTransportObject* self)
{
	TibrvStatus status = self->transport->destroy();
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
typemethod_isvalid(PyTibrvNetTransportObject* self)
{
	PyObject* value = (self->transport->isValid() == TIBRV_TRUE ? Py_True : Py_False);
    Py_INCREF(value);
    return value;
}

static PyObject*
typemethod_send(PyTibrvNetTransportObject *self, PyObject *args, PyObject *kwds)
{
    char *send_subject = NULL, *reply_subject = NULL;
	PyObject* message_dictionary;

    static char *kwlist[] = {"sendsubject", "message", "replysubject", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "sO|s", kwlist, &send_subject, &message_dictionary, &reply_subject))
        return NULL; 
	
	TibrvStatus status;

	TibrvMsg msg;
	status = msg.setSendSubject(send_subject);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	if (reply_subject != 0)
		msg.setReplySubject(reply_subject);

	if (!PyDict_Check(message_dictionary))
	{
		PyErr_SetString(PyExc_TypeError, "expected message to be dictionary");
		return NULL;
	}

	PyObject *key, *value;
	int pos = 0;

	while (PyDict_Next(message_dictionary, &pos, &key, &value))
	{
		if (PyObject_Is<const char*>(key))
		{
			const char* mnemonic = PyObject_As<const char*>(key);

			if (PyObject_Is<tibrv_i32>(value))
				status = msg.addI32(mnemonic, PyObject_As<tibrv_i32>(value));
			else if (PyObject_Is<const char*>(value))
				status = msg.addString(mnemonic, PyObject_As<const char*>(value));
			else if (PyObject_Is<tibrv_f64>(value))
				status = msg.addF64(mnemonic, PyObject_As<tibrv_f64>(value));
			else if (PyObject_Is<tibrv_bool>(value))
				status = msg.addBool(mnemonic, PyObject_As<tibrv_bool>(value));
			else if (PyObject_Is<tibrvMsgDateTime>(value))
				status = msg.addDateTime(mnemonic, PyObject_As<tibrvMsgDateTime>(value));
			else
			{
				PyErr_SetString(PyExc_TypeError, "invalid type for tibco message");
				return NULL;
			}

			if (status != TIBRV_OK)
			{
				PyErr_Tibrv(status);
				return NULL;
			}
		}
	}

	status = self->transport->send(msg);
	if (status != TIBRV_OK)
	{
		PyErr_Tibrv(status);
		return NULL;
	}

	Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef type_methods[] = {
    {"createinbox", (PyCFunction)typemethod_createinbox, METH_NOARGS, "Create a unique inbox subject name."},
    {"destroy", (PyCFunction)typemethod_destroy, METH_NOARGS, "Destroy a transport."},
    {"isvalid", (PyCFunction)typemethod_isvalid, METH_NOARGS, "Destroy a transport."},
    {"send", (PyCFunction)typemethod_send, METH_VARARGS | METH_KEYWORDS, "Send a message."},
    {NULL}  /* Sentinel */
};

PyTypeObject PyTibrvNetTransport_Type = {
    PyObject_HEAD_INIT(NULL)
    0,									/*ob_size*/
    "tibrv.nettransport",					/*tp_name*/
    sizeof(PyTibrvNetTransportObject),	/*tp_basicsize*/
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
    "Tibco Net Transport",				/* tp_doc */
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
PyTibrvNetTransport_FromTransport(TibrvNetTransport *transport)
{
	PyTibrvNetTransportObject *t = (PyTibrvNetTransportObject*)PyTibrvNetTransport_Type.tp_new(&PyTibrvNetTransport_Type, NULL, NULL);
	if (t != NULL)
	{
		t->transport = transport;
	}
	return (PyObject *) t;
}

TibrvNetTransport *
PyTibrvNetTransport_AsTransport(PyObject *transport)
{
	PyTibrvNetTransportObject *t = (PyTibrvNetTransportObject*)transport;
	if (t != NULL)
		return t->transport;
	else
		return NULL;
}

void initPyTibrvNetTransport(PyObject* module) 
{
    if (PyType_Ready(&PyTibrvNetTransport_Type) < 0)
        return;
    Py_INCREF(&PyTibrvNetTransport_Type);
    PyModule_AddObject(module, "transport", (PyObject*) &PyTibrvNetTransport_Type);
}
