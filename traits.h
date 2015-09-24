#pragma once

template<typename TIBCO_TYPE>
struct tibrv_traits
{
public:
	static PyObject* PyObject_FromType(const TIBCO_TYPE);
	static TIBCO_TYPE PyObject_AsType(PyObject*);
};

inline PyObject* tibrv_traits<tibrv_f64>::PyObject_FromType(const tibrv_f64 value) { return PyFloat_FromDouble(value); }
inline tibrv_f64 tibrv_traits<tibrv_f64>::PyObject_AsType(PyObject* value) { return PyFloat_AsDouble(value); }

inline PyObject* tibrv_traits<tibrv_f32>::PyObject_FromType(const tibrv_f32 value) { return PyFloat_FromDouble(static_cast<double>(value)); }
inline tibrv_f32 tibrv_traits<tibrv_f32>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_f32>(PyFloat_AsDouble(value)); }

inline PyObject* tibrv_traits<tibrv_i8>::PyObject_FromType(const tibrv_i8 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_i8 tibrv_traits<tibrv_i8>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_i8>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<tibrv_u8>::PyObject_FromType(const tibrv_u8 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_u8 tibrv_traits<tibrv_u8>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_u8>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<tibrv_i16>::PyObject_FromType(const tibrv_i16 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_i16 tibrv_traits<tibrv_i16>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_i16>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<tibrv_u16>::PyObject_FromType(const tibrv_u16 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_u16 tibrv_traits<tibrv_u16>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_u16>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<tibrv_i32>::PyObject_FromType(const tibrv_i32 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_i32 tibrv_traits<tibrv_i32>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_i32>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<tibrv_u32>::PyObject_FromType(const tibrv_u32 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_u32 tibrv_traits<tibrv_u32>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_u32>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<tibrv_i64>::PyObject_FromType(const tibrv_i64 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_i64 tibrv_traits<tibrv_i64>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_i64>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<tibrv_u64>::PyObject_FromType(const tibrv_u64 value) { return PyInt_FromLong(static_cast<long>(value)); }
inline tibrv_u64 tibrv_traits<tibrv_u64>::PyObject_AsType(PyObject* value) { return static_cast<tibrv_u64>(PyInt_AsLong(value)); }

inline PyObject* tibrv_traits<const char*>::PyObject_FromType(const char* value) { return PyString_FromString(value); }
inline const char* tibrv_traits<const char*>::PyObject_AsType(PyObject* value) { return static_cast<const char*>(PyString_AsString(value)); }

inline PyObject* tibrv_traits<tibrv_bool>::PyObject_FromType(const tibrv_bool value)
{
	PyObject* py_value = (value == TIBRV_TRUE ? Py_True : Py_False);
	Py_INCREF(py_value);
	return py_value;
}
inline tibrv_bool tibrv_traits<tibrv_bool>::PyObject_AsType(PyObject* value) { return PyObject_IsTrue(value) ? TIBRV_TRUE : TIBRV_FALSE; }

inline PyObject* tibrv_traits<tibrvMsgDateTime>::PyObject_FromType(const tibrvMsgDateTime value)
{
	time_t t = value.sec;

	struct tm _tm;
	_localtime64_s(&_tm, &t);

	PyObject* result = PyTuple_New(6);
	PyTuple_SetItem(result, 0, PyInt_FromLong(1900 + _tm.tm_year));
	PyTuple_SetItem(result, 1, PyInt_FromLong(1 + _tm.tm_mon));
	PyTuple_SetItem(result, 2, PyInt_FromLong(_tm.tm_mday));
	PyTuple_SetItem(result, 3, PyInt_FromLong(_tm.tm_hour));
	PyTuple_SetItem(result, 4, PyInt_FromLong(_tm.tm_min));
	PyTuple_SetItem(result, 5, PyInt_FromLong(_tm.tm_sec));

	return result;
}

inline tibrvMsgDateTime tibrv_traits<tibrvMsgDateTime>::PyObject_AsType(PyObject* value)
{
	tibrvMsgDateTime lhs;

	struct tm tmbuf;
	tmbuf.tm_sec = 0;
	tmbuf.tm_min = 0;
	tmbuf.tm_hour = 0;
	tmbuf.tm_wday = 0;
	tmbuf.tm_yday = 0;

	if (PySequence_Size(value) == 3)
	{
		PyObject* years = PySequence_GetItem(value, 0);
		PyObject* months = PySequence_GetItem(value, 1);
		PyObject* days = PySequence_GetItem(value, 2);

		if (PyInt_Check(years) && PyInt_Check(months) && PyInt_Check(days))
		{
			tmbuf.tm_mday = PyInt_AsLong(days);
			tmbuf.tm_mon = PyInt_AsLong(months) - 1;
			tmbuf.tm_year = PyInt_AsLong(years) - 1900;

			lhs.sec = mktime(&tmbuf);
			lhs.nsec = 0;

			return lhs;
		}
		else
			throw "invalid date time format";
	}
	else if (PySequence_Size(value) == 6)
	{
		PyObject* years = PySequence_GetItem(value, 0);
		PyObject* months = PySequence_GetItem(value, 1);
		PyObject* days = PySequence_GetItem(value, 2);
		PyObject* hours = PySequence_GetItem(value, 3);
		PyObject* minutes = PySequence_GetItem(value, 4);
		PyObject* seconds = PySequence_GetItem(value, 5);

		if (PyInt_Check(years) && PyInt_Check(months) && PyInt_Check(days) && PyInt_Check(hours) && PyInt_Check(minutes) && PyInt_Check(seconds))
		{
			tmbuf.tm_sec = PyInt_AsLong(seconds);
			tmbuf.tm_min = PyInt_AsLong(minutes);
			tmbuf.tm_hour = PyInt_AsLong(hours);
			tmbuf.tm_mday = PyInt_AsLong(days);
			tmbuf.tm_mon = PyInt_AsLong(months) - 1;
			tmbuf.tm_year = PyInt_AsLong(years) - 1900;

			lhs.sec = mktime(&tmbuf);
			lhs.nsec = 0;

			return lhs;
		}
		else
			throw "invalid date time format";
	}
	else
		throw "invalid date time format";
}

template <typename T>
inline PyObject* PyObject_From(const T& value)
{
	return tibrv_traits<T>::PyObject_FromType(value);
}

template <typename T>
inline PyObject* PyObject_From(const T* array, int count)
{
	PyObject* list = PyList_New(count);
	for (int i = 0; i < count; ++i)
		PyList_SetItem(list, i, tibrv_traits<T>::PyObject_FromType(array[i]));
	return list;
}

template <typename T>
inline T PyObject_As(PyObject* item)
{
	return tibrv_traits<T>::PyObject_AsType(item);
}

template <typename T>
inline T* PyList_As(PyObject* list)
{
	int count = PyList_Size(list);
	T* array = new T[count];
	for (int i = 0; i < count; ++i)
	{
		PyObject item = PyList_GetItem(list, i);
		array[i] = tibrv_traits<T>::PyObject_AsType(item);
	}

	return array;
}
