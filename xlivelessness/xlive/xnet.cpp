#include <winsock2.h>
#include "xdefs.h"
#include "xnet.h"
#include "xlive.h"
#include "../xlln/DebugText.h"
#include "xsocket.h"
#include "NetEntity.h"

BOOL WINAPI xlive_net_initialized = TRUE;

XNADDR xlive_local_xnAddr;

void UnregisterSecureAddr(const IN_ADDR ina)
{
	//TODO
}


// #51
INT WINAPI XNetStartup(const XNetStartupParams *pxnsp)
{
	TRACE_FX();
	if (pxnsp && pxnsp->cfgSizeOfStruct != sizeof(XNetStartupParams))
		return ERROR_INVALID_PARAMETER;
	if (!xlive_netsocket_abort)
		xlive_net_initialized = TRUE;
	if (!xlive_net_initialized)
		addDebugText("- Xlive Net not initialized!");
		return ERROR_FUNCTION_FAILED;
	return ERROR_SUCCESS;
}

// #52
INT WINAPI XNetCleanup()
{
	TRACE_FX();
	xlive_net_initialized = FALSE;
	if (!xlive_online_initialized)
		addDebugText("- Xlive Online not initialized!");
		return WSANOTINITIALISED;
	return ERROR_SUCCESS;
}

// #53
INT WINAPI XNetRandom(BYTE* pb, DWORD cb)
{
	TRACE_FX();
	
	if (cb)
		for (DWORD i = 0; i < cb; i++)
			pb[i] = static_cast<BYTE>(rand());

	return S_OK;
}

// #54
HRESULT WINAPI XNetCreateKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	TRACE_FX();
	if (!pxnkid)
		return E_INVALIDARG;
	if (!pxnkey)
		return E_INVALIDARG;

	memcpy(pxnkid->ab, "DEADC0DE", sizeof(pxnkid->ab));
	memcpy(pxnkey->ab, "DEADC0DEDEADC0DE", sizeof(pxnkey->ab));

	memset(pxnkid, 0x8B, sizeof(XNKID));
	memset(pxnkid, 0xAB, sizeof(XNKID));
	memset(pxnkey, 0XAA, sizeof(XNKEY));

	// These are un-necessary.
	pxnkid->ab[0] &= ~XNET_XNKID_MASK;
	pxnkid->ab[0] |= XNET_XNKID_SYSTEM_LINK;
	
	return S_OK;
}

// #55: need #51
INT WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	TRACE_FX();
	return S_OK;
}

// #56: need #51
INT WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	TRACE_FX();
	return S_OK;
}

// #57
INT WINAPI XNetXnAddrToInAddr(XNADDR *pxna, XNKID *pnkid, IN_ADDR *pina)
{
	TRACE_FX();
	ULONG secure = pxna->inaOnline.s_addr;

	NetEntityCreate(pxna);

	if (secure != 0) {
		pina->s_addr = secure;
	}
	/*else {
		XllnDebugBreak("ERROR XNetXnAddrToInAddr NULL inaOnline.");
		//*pina = 0;
		return E_UNEXPECTED;
	}*/

	return S_OK;
}

// #58
VOID XNetServerToInAddr()
{
	TRACE_FX();
	FUNC_STUB();
}

// #59
VOID XNetTsAddrToInAddr()
{
	TRACE_FX();
	FUNC_STUB();
}

// #60
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR *pxna, XNKID *pxnkid)
{
	TRACE_FX();

	DWORD hSecure = ntohl(ina.s_addr);

	if (NetEntityGetSecure(*pxna, hSecure) == ERROR_SUCCESS) {
		return S_OK;
	}

	__debugbreak();
	return E_FAIL;
}

// #61
VOID XNetInAddrToServer()
{
	TRACE_FX();
	FUNC_STUB();
}

// #62
VOID XNetInAddrToString()
{
	TRACE_FX();
	FUNC_STUB();
}

// #63
INT WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	TRACE_FX();
	UnregisterSecureAddr(ina);
	return S_OK;
}

// #64
VOID XNetXnAddrToMachineId()
{
	TRACE_FX();
	FUNC_STUB();
}

// #65
INT WINAPI XNetConnect(const IN_ADDR ina)
{
	TRACE_FX();
	return S_OK;
}

// #66
INT WINAPI XNetGetConnectStatus(const IN_ADDR ina)
{
	TRACE_FX();
	if (!xlive_net_initialized)
		addDebugText("- Xlive Net not initialized!");
		//return XNET_CONNECT_STATUS_PENDING;
	return XNET_CONNECT_STATUS_CONNECTED;
}

// #67
INT WINAPI XNetDnsLookup(const char *pszHost, WSAEVENT hEvent, XNDNS **ppxndns)
{
	TRACE_FX();
	if (!ppxndns) {
		return ERROR_INVALID_PARAMETER;
	}
	
	// Improper Implementation.
	struct hostent *he = gethostbyname(pszHost);
	INT result = WSAGetLastError();

	if (he != NULL) {
		struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
		int i = 0;
		for (; addr_list[i] != NULL && i < 8; i++) {
			memcpy_s((&(*ppxndns)->aina)[i], sizeof(in_addr), addr_list[i], sizeof(in_addr));
		}
		(*ppxndns)->cina = i;
	}

	return result;
}

// #68
INT WINAPI XNetDnsRelease(XNDNS *pxndns)
{
	TRACE_FX();
	INT result = ERROR_SUCCESS;
	return result;
}

// #73
DWORD WINAPI XNetGetTitleXnAddr(XNADDR *pAddr)
{
	TRACE_FX();
	if (!xlive_net_initialized)
		addDebugText("- Xlive Net not initialized!");
		//return XNET_GET_XNADDR_PENDING;
	if (pAddr) {
		xlive_local_xnAddr.ina.s_addr = htonl(LocalUserHostIpv4());
		// test without editing local version?
		*pAddr = xlive_local_xnAddr;
	}
	return XNET_GET_XNADDR_STATIC | XNET_GET_XNADDR_ETHERNET | XNET_GET_XNADDR_ONLINE;
}

// #74
VOID XNetGetDebugXnAddr()
{
	TRACE_FX();
	FUNC_STUB();
}

// #75
DWORD WINAPI XNetGetEthernetLinkStatus()
{
	TRACE_FX();
	if (!xlive_net_initialized)
		addDebugText("- Xlive Net not initialized!");
		//return XNET_ETHERNET_LINK_INACTIVE;
	return XNET_ETHERNET_LINK_ACTIVE | XNET_ETHERNET_LINK_100MBPS | XNET_ETHERNET_LINK_FULL_DUPLEX;
}

// #76
VOID XNetGetBroadcastVersionStatus()
{
	TRACE_FX();
	FUNC_STUB();
}

// #78
VOID XNetGetOpt()
{
	TRACE_FX();
	FUNC_STUB();
}

// #79
VOID XNetSetOpt()
{
	TRACE_FX();
	FUNC_STUB();
}

// #80
VOID XNetStartupEx()
{
	TRACE_FX();
	FUNC_STUB();
}

// #81
VOID XNetReplaceKey()
{
	TRACE_FX();
	FUNC_STUB();
}

// #82
INT WINAPI XNetGetXnAddrPlatform(in_addr* a1, int a2)
{
	TRACE_FX();
	
	return XNET_XNADDR_PLATFORM_WINPC; //0x00000002
}

// #83
VOID XNetGetSystemLinkPort()
{
	TRACE_FX();
	FUNC_STUB();
}

// #84
INT WINAPI XNetSetSystemLinkPort(WORD wSystemLinkPort)
{
	TRACE_FX();
	//network byte (big-endian) to little-endian host
	WORD hPort = ntohs(wSystemLinkPort);

	return ERROR_SUCCESS;
}

// #5023
VOID XNetGetCurrentAdapter()
{
	TRACE_FX();
	FUNC_STUB();
}
