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
		TibrvStatus status;

		PyObject* py_msg = PyObject_From(msg);

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
