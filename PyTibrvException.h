#pragma once

extern PyObject* tibrv_error;

void initPyTibrvException(PyObject* module);
void PyErr_Tibrv(const TibrvStatus& status);
