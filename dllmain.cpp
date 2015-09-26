// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "tibrv_exception.h"
#include "traits.h"
#include "types.h"
#include "messages.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#include "PyTibrvException.h"
#include "PyTibrvQueue.h"
#include "PyTibrvEnvironment.h"
#include "PyTibrvNetTransport.h"
#include "PyTibrvListener.h"

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

extern "C" {
	__declspec(dllexport) void inittibrv(void)
	{
		PyObject *module = Py_InitModule("tibrv", module_methods);

		initPyTibrvException(module);
		initPyTibrvQueue(module);
		initPyTibrvEnvironment(module);
		initPyTibrvNetTransport(module);
		initPyTibrvListener(module);

		PyObject* environment = PyTibrvEnvironment_New();
		Py_INCREF(environment);
		PyModule_AddObject(module, "environment", environment);
	}
}