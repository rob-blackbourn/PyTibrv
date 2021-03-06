#pragma once

struct message_key
{
	message_key(tibrv_u16 id, const char* name) : id(id), name(name) { }
	message_key(const message_key& key) : id(key.id), name(key.name) { }

	const char* name;
	const tibrv_u16 id;
};

inline PyObject* python_traits<message_key>::PyObject_FromType(const message_key& value)
{
	PyObject* key = PyTuple_New(2);
	PyTuple_SetItem(key, 0, python_traits<tibrv_u16>::PyObject_FromType(value.id));
	PyTuple_SetItem(key, 1, python_traits<const char*>::PyObject_FromType(value.name));
	return key;
}

inline message_key python_traits<message_key>::PyObject_AsType(PyObject* value)
{
	if (PyObject_Is<const char*>(value))
		return message_key(0, PyObject_As<const char*>(value));
	else if (PyObject_Is<tibrv_u16>(value))
		return message_key(PyObject_As<tibrv_u16>(value), NULL);
	else if (PyObject_Is<tibrv_u16, const char*>(value))
		return message_key(python_traits<tibrv_u16>::PyObject_AsType(PySequence_GetItem(value, 0)), python_traits<const char*>::PyObject_AsType(PySequence_GetItem(value, 1)));
	else
		throw std::exception("invalid key");
}

inline bool python_traits<message_key>::PyObject_CheckType(PyObject* value)
{
	return PyObject_Is<const char*>(value) || PyObject_Is<tibrv_u16>(value) || PyObject_Is<tibrv_u16, const char*>(value);
}

inline PyObject* python_traits<tibrvMsg>::PyObject_FromType(const tibrvMsg& msg)
{
	std::unique_ptr<TibrvMsg> tibrv_msg(new TibrvMsg(msg, TIBRV_TRUE));
	PyObject* py_object = PyObject_From(*tibrv_msg);
	return py_object;
}

inline PyObject* python_traits<TibrvMsgField>::PyObject_FromType(const TibrvMsgField& msg_field)
{
	tibrv_u8 type = msg_field.getType();
	PyObject* value;
	switch (type)
	{
	case TIBRVMSG_MSG:
		value = PyObject_From(msg_field.getData().msg);
		break;
	case TIBRVMSG_DATETIME:
		value = PyObject_From(msg_field.getData().date);
		break;
	case TIBRVMSG_OPAQUE:
		value = Py_None;
		Py_INCREF(value);
		break;
	case TIBRVMSG_STRING:
		value = PyObject_From(msg_field.getData().str);
		break;
	case TIBRVMSG_BOOL:
		value = PyObject_From(msg_field.getData().boolean);
		break;
	case TIBRVMSG_I8:
		value = PyObject_From(msg_field.getData().i8);
		break;
	case TIBRVMSG_U8:
		value = PyObject_From(msg_field.getData().u8);
		break;
	case TIBRVMSG_I16:
		value = PyObject_From(msg_field.getData().i16);
		break;
	case TIBRVMSG_U16:
		value = PyObject_From(msg_field.getData().u16);
		break;
	case TIBRVMSG_I32:
		value = PyObject_From(msg_field.getData().i32);
		break;
	case TIBRVMSG_U32:
		value = PyObject_From(msg_field.getData().u32);
		break;
	case TIBRVMSG_I64:
		value = PyObject_From(msg_field.getData().i64);
		break;
	case TIBRVMSG_U64:
		value = PyObject_From(msg_field.getData().u64);
		break;
	case TIBRVMSG_F32:
		value = PyObject_From(msg_field.getData().f32);
		break;
	case TIBRVMSG_F64:
		value = PyObject_From(msg_field.getData().f64);
		break;
	case TIBRVMSG_I8ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_i8*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_U8ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_u8*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_I16ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_i16*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_U16ARRAY:
		value = PyTuple_From(reinterpret_cast< const tibrv_u16*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_I32ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_i32*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_U32ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_u32*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_I64ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_i64*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_U64ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_u64*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_F32ARRAY:
		value = PyTuple_From(reinterpret_cast<const tibrv_f32*>(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_F64ARRAY:
		value = PyTuple_From((tibrv_f64*)(msg_field.getData().array), msg_field.getCount());
		break;
	case TIBRVMSG_NONE:
		value = Py_None;
		Py_INCREF(value);
		break;
	}

	tibrv_u16 id = msg_field.getId();
	const char* name = msg_field.getName();

	PyObject* key = PyTuple_New(2);
	PyTuple_SetItem(key, 0, PyObject_From(msg_field.getId()));
	PyTuple_SetItem(key, 1, PyString_FromString(msg_field.getName()));

	PyObject* item = PyTuple_New(2);
	PyTuple_SetItem(item, 0, key);
	PyTuple_SetItem(item, 1, value);

	return item;
}

inline PyObject* python_traits<TibrvMsg>::PyObject_FromType(const TibrvMsg& msg)
{
	tibrv_u32 num_fields;
	TibrvStatus status = msg.getNumFields(num_fields);
	PyObject* py_msg = PyTuple_New(num_fields);
	for (tibrv_u32 field_index = 0; field_index < num_fields; ++field_index)
	{
		TibrvMsgField msg_field;
		status = const_cast<TibrvMsg&>(msg).getFieldByIndex(msg_field, field_index);
		PyObject* item = PyObject_From(msg_field);
		PyTuple_SetItem(py_msg, field_index, item);
	}
	return py_msg;
}

inline TibrvStatus add_field(TibrvMsg& msg, const message_key& key, PyObject* value)
{
	if (PyObject_Is<tibrv_i32>(value))
		return msg.addI32(key.name, PyObject_As<tibrv_i32>(value), key.id);
	else if (PyObject_Is<const char*>(value))
		return msg.addString(key.name, PyObject_As<const char*>(value), key.id);
	else if (PyObject_Is<tibrv_f64>(value))
		return msg.addF64(key.name, PyObject_As<tibrv_f64>(value), key.id);
	else if (PyObject_Is<tibrv_bool>(value))
		return msg.addBool(key.name, PyObject_As<tibrv_bool>(value), key.id);
	else if (PyObject_Is<tibrvMsgDateTime>(value))
		return msg.addDateTime(key.name, PyObject_As<tibrvMsgDateTime>(value), key.id);
	else if (PySequence_Is<tibrv_i32>(value))
		return msg.addI32Array(key.name, PySequence_As<tibrv_i32>(value), PySequence_Size(value), key.id);
	else if (PySequence_Is<tibrv_f64>(value))
		return msg.addF64Array(key.name, PySequence_As<tibrv_f64>(value), PySequence_Size(value), key.id);
	else if (PyObject_Is<TibrvMsg>(value))
		return msg.addMsg(key.name, PyObject_As<TibrvMsg>(value), key.id);
	else
		return TibrvStatus(tibrv_status::TIBRV_INVALID_TYPE);
}

inline TibrvStatus add_field(TibrvMsg& msg, PyObject* key, PyObject* value)
{
	if (!PyObject_Is<message_key>(key))
		throw std::exception("invalid key");

	TibrvStatus status = add_field(msg, PyObject_As<message_key>(key), value);

	if (status != TIBRV_OK)
		throw tibrv_exception(status, "failed to create field");
}

inline bool python_traits<TibrvMsg>::PyObject_CheckType(PyObject* py_object)
{
	return PySequence_Check(py_object) || PyDict_Check(py_object);
}

inline TibrvMsg python_traits<TibrvMsg>::PyObject_AsType(PyObject* fields)
{
	TibrvMsg msg;
	TibrvStatus status;

	if (PyDict_Check(fields))
	{
		PyObject *key, *value;
		int pos = 0;

		while (PyDict_Next(fields, &pos, &key, &value))
		{
			status = add_field(msg, key, value);

			if (status != TIBRV_OK)
				throw tibrv_exception(status, "failed to create field");
		}
	}
	else if (PySequence_Check(fields))
	{
		for (Py_ssize_t i = 0; i < PySequence_Size(fields); ++i)
		{
			PyObject* item = PySequence_GetItem(fields, i);
			if (!PySequence_Check(item))
				throw std::exception("When the fields is a sequence, each item must be a sequence of two elements.");

			status = add_field(msg, PySequence_GetItem(item, 0), PySequence_GetItem(item, 1));

			if (status != TIBRV_OK)
				throw tibrv_exception(status, "failed to create field");
		}
	}
}