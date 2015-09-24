#include "stdafx.h"

struct { tibrv_status code; char* name; } exceptions[] = {
	{TIBRV_INIT_FAILURE,			"INIT_FAILURE"},
	{TIBRV_INVALID_TRANSPORT,		"INVALID_TRANSPORT"},
	{TIBRV_INVALID_ARG,				"INVALID_ARG"},
	{TIBRV_NOT_INITIALIZED,			"NOT_INITIALIZED"},
	{TIBRV_ARG_CONFLICT,			"ARG_CONFLICT"},

	{TIBRV_SERVICE_NOT_FOUND,		"SERVICE_NOT_FOUND"},
	{TIBRV_NETWORK_NOT_FOUND,		"NETWORK_NOT_FOUND"},
	{TIBRV_DAEMON_NOT_FOUND,		"DAEMON_NOT_FOUND"},
	{TIBRV_NO_MEMORY,				"NO_MEMORY"},
	{TIBRV_INVALID_SUBJECT,			"INVALID_SUBJECT"},
	{TIBRV_DAEMON_NOT_CONNECTED,	"DAEMON_NOT_CONNECTED"},
    {TIBRV_VERSION_MISMATCH,		"VERSION_MISMATCH"},
    {TIBRV_SUBJECT_COLLISION,		"SUBJECT_COLLISION"},
    {TIBRV_VC_NOT_CONNECTED,		"VC_NOT_CONNECTED"},

    {TIBRV_NOT_PERMITTED,			"NOT_PERMITTED"},

    {TIBRV_INVALID_NAME,			"INVALID_NAME"},
    {TIBRV_INVALID_TYPE,			"INVALID_TYPE"},
    {TIBRV_INVALID_SIZE,			"INVALID_SIZE"},
    {TIBRV_INVALID_COUNT,			"INVALID_COUNT"},

    {TIBRV_NOT_FOUND,				"NOT_FOUND"},
    {TIBRV_ID_IN_USE,				"ID_IN_USE"},
    {TIBRV_ID_CONFLICT,				"ID_CONFLICT"},
    {TIBRV_CONVERSION_FAILED,		"CONVERSION_FAILED"},
    {TIBRV_RESERVED_HANDLER,		"RESERVED_HANDLER"},
    {TIBRV_ENCODER_FAILED,			"ENCODER_FAILED"},
    {TIBRV_DECODER_FAILED,			"DECODER_FAILED"},
    {TIBRV_INVALID_MSG,				"INVALID_MSG"},
    {TIBRV_INVALID_FIELD,			"INVALID_FIELD"},
    {TIBRV_INVALID_INSTANCE,		"INVALID_INSTANCE"},
    {TIBRV_CORRUPT_MSG,				"CORRUPT_MSG"},
    {TIBRV_ENCODING_MISMATCH,		"ENCODING_MISMATCH"},

    {TIBRV_TIMEOUT,					"TIMEOUT"},
    {TIBRV_INTR,					"INTR"},

    {TIBRV_INVALID_DISPATCHABLE,	"INVALID_DISPATCHABLE"},
    {TIBRV_INVALID_DISPATCHER,		"INVALID_DISPATCHER"},

    {TIBRV_INVALID_EVENT,			"INVALID_EVENT"},
    {TIBRV_INVALID_CALLBACK,		"INVALID_CALLBACK"},
    {TIBRV_INVALID_QUEUE,			"INVALID_QUEUE"},
    {TIBRV_INVALID_QUEUE_GROUP,		"INVALID_QUEUE_GROUP"},

    {TIBRV_INVALID_TIME_INTERVAL,	"INVALID_TIME_INTERVAL"},

    {TIBRV_INVALID_IO_SOURCE,		"INVALID_IO_SOURCE"},
    {TIBRV_INVALID_IO_CONDITION,	"INVALID_IO_CONDITION"},
    {TIBRV_SOCKET_LIMIT,			"SOCKET_LIMIT"},

    {TIBRV_OS_ERROR,				"OS_ERROR"},

    {TIBRV_INSUFFICIENT_BUFFER,		"INSUFFICIENT_BUFFER"},
    {TIBRV_EOF,						"EOF"},
    {TIBRV_INVALID_FILE,			"INVALID_FILE"},
    {TIBRV_FILE_NOT_FOUND,			"FILE_NOT_FOUND"},
    {TIBRV_IO_FAILED,				"IO_FAILED"},

    {TIBRV_NOT_FILE_OWNER,			"NOT_FILE_OWNER"},
    {TIBRV_USERPASS_MISMATCH,		"USERPASS_MISMATCH"},

    {TIBRV_TOO_MANY_NEIGHBORS,		"TOO_MANY_NEIGHBORS"},
    {TIBRV_ALREADY_EXISTS,			"ALREADY_EXISTS"},

    {TIBRV_PORT_BUSY,				"PORT_BUSY"}
};

PyObject* tibrv_error = PyErr_NewException("tibrv.error", NULL, NULL);

typedef std::map<tibrv_status,PyObject*> exception_map_t;
static exception_map_t exception_map;

static PyObject* GetException(const TibrvStatus& status)
{
	exception_map_t::iterator i_map(exception_map.find(status.getCode()));
	if (i_map == exception_map.end())
		return tibrv_error;
	else
		return i_map->second;
}

void PyErr_Tibrv(const TibrvStatus& status)
{
	PyErr_SetString(GetException(status), status.getText());
}

void initPyTibrvException(PyObject* module)
{
	Py_INCREF(tibrv_error);
	PyModule_AddObject(module, "error", tibrv_error);

	for (size_t i = 0; i < sizeof(exceptions) / sizeof(exceptions[0]); ++i)
	{
		char name[BUFSIZ] = "tibrv.";
		strcat_s(name, BUFSIZ, exceptions[i].name);

		PyObject* error = PyErr_NewException(name, tibrv_error, NULL);
		Py_INCREF(error);
		PyModule_AddObject(module, exceptions[i].name, error);

		exception_map[exceptions[i].code] = error;
	}
}
