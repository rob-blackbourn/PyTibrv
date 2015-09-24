#pragma once

class PyTibrvCallback : public TibrvCallback
{
public:
	PyTibrvCallback(PyObject* Callback, bool OneOff)
		:	m_OneOff(OneOff)
	{
		if (!PyCallable_Check(Callback))
			throw "invalid callback";

		Py_INCREF(Callback);
		m_Callback = Callback;
	}

	~PyTibrvCallback()
	{
		if (m_Callback != NULL)
		{
			Py_DECREF(m_Callback);
			m_Callback = NULL;
		}
	}

	PyObject* getCallback() { return m_Callback; }
	const PyObject* getCallback() const { return m_Callback; }

private:
	virtual void onEvent(TibrvEvent* tibevent, TibrvMsg& msg)
	{
		tibrv_u32 num_fields;
		TibrvStatus status = msg.getNumFields(num_fields);
		PyObject* py_msg = PyTuple_New(num_fields);
		for (tibrv_u32 field_index = 0; field_index < num_fields; ++field_index)
		{
			TibrvMsgField msg_field;
			status = msg.getFieldByIndex(msg_field, field_index);
			tibrv_u8 type = msg_field.getType();
			PyObject* value;
			switch (type)
			{
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
				value = PyObject_From((tibrv_i8*) (msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_U8ARRAY:
				value = PyObject_From((tibrv_u8*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_I16ARRAY:
				value = PyObject_From((tibrv_i16*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_U16ARRAY:
				value = PyObject_From((tibrv_u16*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_I32ARRAY:
				value = PyObject_From((tibrv_i32*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_U32ARRAY:
				value = PyObject_From((tibrv_u32*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_I64ARRAY:
				value = PyObject_From((tibrv_i64*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_U64ARRAY:
				value = PyObject_From((tibrv_u64*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_F32ARRAY:
				value = PyObject_From((tibrv_f32*)(msg_field.getData().array), msg_field.getCount());
				break;
			case TIBRVMSG_F64ARRAY:
				value = PyObject_From((tibrv_f64*)(msg_field.getData().array), msg_field.getCount());
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

			PyTuple_SetItem(py_msg, field_index, item);
		}

		const char* send_subject, *reply_subject;
		status = msg.getSendSubject(send_subject);
		status = msg.getReplySubject(reply_subject);
		PyObject* args = Py_BuildValue("ssN", send_subject, reply_subject, py_msg);
		PyObject* result = PyObject_CallObject(m_Callback, args);
		Py_DECREF(args);
		if (result != NULL)
			Py_DECREF(result);

		if (m_OneOff)
			delete this;
	}

private:
	PyObject* m_Callback;
	bool m_OneOff;
};
