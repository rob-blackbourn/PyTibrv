#pragma once

template<typename T>
struct python_traits
{
public:
	static PyObject* PyObject_FromType(T const&);
	static T PyObject_AsType(PyObject*);
	static bool PyObject_CheckType(PyObject*);
};

inline PyObject* python_traits<const char*>::PyObject_FromType(const char* const& value) { return PyString_FromString(value); }
inline const char* python_traits<const char*>::PyObject_AsType(PyObject* value) { return PyString_AsString(value); }
inline bool python_traits<const char*>::PyObject_CheckType(PyObject* value) { return PyString_Check(value); }

template <typename T>
inline PyObject* PyObject_From(const T& value)
{
	return python_traits<T>::PyObject_FromType(value);
}

template <typename T>
inline PyObject* PyTuple_From(const T* array, int count)
{
	PyObject* tuple = PyTuple_New(count);
	for (int i = 0; i < count; ++i)
		PyTuple_SetItem(tuple, i, PyObject_From<T>(array[i]));
	return tuple;
}

template <typename T>
inline T PyObject_As(PyObject* item)
{
	return python_traits<T>::PyObject_AsType(item);
}

template <typename T>
inline bool PyObject_Is(PyObject* item)
{
	return python_traits<T>::PyObject_CheckType(item);
}

template <typename T>
inline bool PySequence_Is(PyObject* item)
{
	if (!PySequence_Check(item))
		return false;

	for (Py_ssize_t i = 0; i < PySequence_Size(item); ++i)
		if (!python_traits<T>::PyObject_CheckType(PySequence_GetItem(item, i)))
			return false;

	return true;
}

template <typename T1, typename T2>
inline bool PyObject_Is(PyObject* item)
{
	return PySequence_Check(item) && PySequence_Size(item) == 2 && PyObject_Is<T1>(PySequence_GetItem(item, 0)) && PyObject_Is<T2>(PySequence_GetItem(item, 2));
}

template <typename T>
inline T* PySequence_As(PyObject* sequence)
{
	int count = PySequence_Size(sequence);
	T* array = new T[count];
	for (int i = 0; i < count; ++i)
	{
		PyObject* item = PySequence_GetItem(sequence, i);
		array[i] = PyObject_As<T>(item);
	}

	return array;
}
