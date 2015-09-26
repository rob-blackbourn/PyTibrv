#pragma once

template<typename T>
struct python_traits
{
public:
	static PyObject* PyObject_FromType(const T);
	static T PyObject_AsType(PyObject*);
	static bool PyObject_CheckType(PyObject*);
};

inline PyObject* python_traits<const char*>::PyObject_FromType(const char* value) { return PyString_FromString(value); }
inline const char* python_traits<const char*>::PyObject_AsType(PyObject* value) { return static_cast<const char*>(PyString_AsString(value)); }
inline bool python_traits<const char*>::PyObject_CheckType(PyObject* value) { return PyString_Check(value); }

template <typename T>
inline PyObject* PyObject_From(const T& value)
{
	return python_traits<T>::PyObject_FromType(value);
}

template <typename T>
inline PyObject* PyObject_From(const T* array, int count)
{
	PyObject* list = PyList_New(count);
	for (int i = 0; i < count; ++i)
		PyList_SetItem(list, i, python_traits<T>::PyObject_FromType(array[i]));
	return list;
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

template <typename T1, typename T2>
inline bool PyObject_Is(PyObject* item)
{
	return PySequence_Check(item) && PySequence_Size(item) == 2 && PyObject_Is<T1>(PySequence_GetItem(item, 0)) && PyObject_Is<T2>(PySequence_GetItem(item, 2));
}

template <typename T>
inline T* PyList_As(PyObject* list)
{
	int count = PyList_Size(list);
	T* array = new T[count];
	for (int i = 0; i < count; ++i)
	{
		PyObject item = PyList_GetItem(list, i);
		array[i] = python_traits<T>::PyObject_AsType(item);
	}

	return array;
}

template <typename T>
inline T* PyTuple_As(PyObject* list)
{
	int count = PyTuple_Size(list);
	T* array = new T[count];
	for (int i = 0; i < count; ++i)
	{
		PyObject item = PyTuple_GetItem(list, i);
		array[i] = python_traits<T>::PyObject_AsType(item);
	}

	return array;
}
