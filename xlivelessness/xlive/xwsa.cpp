#include "xdefs.h"
#include "xwsa.h"
#include "../xlln/DebugText.h"

// #1
INT WINAPI XWSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData)
{
	TRACE_FX();
	INT result = WSAStartup(wVersionRequested, lpWSAData);
	return result;
}

// #2
INT WINAPI XWSACleanup()
{
	TRACE_FX();
	INT result = WSACleanup();
	if (result != ERROR_SUCCESS) {
		//TODO Store error in XWSAGetLastError().
		return SOCKET_ERROR;
	}
	return result;
}

// #16
VOID XWSAGetOverlappedResult()
{
	TRACE_FX();
	FUNC_STUB();
}

// #17
VOID XWSACancelOverlappedIO()
{
	TRACE_FX();
	FUNC_STUB();
}

// #19
VOID XWSARecv()
{
	TRACE_FX();
	FUNC_STUB();
}

// #21
VOID XWSARecvFrom()
{
	TRACE_FX();
	FUNC_STUB();
}

// #23
VOID XWSASend()
{
	TRACE_FX();
	FUNC_STUB();
}

// #25
VOID XWSASendTo()
{
	TRACE_FX();
	FUNC_STUB();
}

// #28
VOID XWSASetLastError()
{
	TRACE_FX();
	FUNC_STUB();
}

// #29
VOID XWSACreateEvent()
{
	TRACE_FX();
	FUNC_STUB();
}

// #30
VOID XWSACloseEvent()
{
	TRACE_FX();
	FUNC_STUB();
}

// #31
VOID XWSASetEvent()
{
	TRACE_FX();
	FUNC_STUB();
}

// #32
VOID XWSAResetEvent()
{
	TRACE_FX();
	FUNC_STUB();
}

// #33
VOID XWSAWaitForMultipleEvents()
{
	TRACE_FX();
	FUNC_STUB();
}

// #34
VOID XWSAFDIsSet()
{
	TRACE_FX();
	FUNC_STUB();
}

// #35
VOID XWSAEventSelect()
{
	TRACE_FX();
	FUNC_STUB();
}
