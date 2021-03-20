#include <winsock2.h>
#include "xdefs.h"
#include "xsocket.h"
#include "xlive.h"
#include "../xlln/DebugText.h"
#include "xnet.h"
#include "xlocator.h"
#include "NetEntity.h"
#include <ctime>

#define IPPROTO_VDP 254

WORD xlive_base_port = 2000;
BOOL xlive_netsocket_abort = FALSE;
static SOCKET xlive_VDP_socket = NULL;
SOCKET xlive_liveoverlan_socket = NULL;

struct SocketInfo {
	int protocol;
	WORD hPort;
};
static CRITICAL_SECTION xlive_critsec_sockets;
static std::map<SOCKET, SocketInfo*> xlive_sockets;

#pragma pack(push, 1) // Save then set byte alignment setting.
typedef struct {
	struct {
		XNADDR xnAddr;
		WORD numOfPorts;
	} HEAD;
	WORD ports[1];
} NET_USER_ASK;
#pragma pack(pop) // Return to original alignment setting.

static VOID SendUnknownUserAskRequest(SOCKET socket, char* data, int dataLen, sockaddr *to, int tolen, bool isAsking)
{
	if (isAsking) {
		addDebugText("XLLN: Send UNKNOWN_USER_ASK.");
	}
	else {
		addDebugText("XLLN: Send UNKNOWN_USER_REPLY.");
	}
	const int cpHeaderLen = sizeof(XLLN_CUSTOM_PACKET_SENTINEL) + sizeof(XLLNCustomPacketType::Type);

	EnterCriticalSection(&xlive_critsec_sockets);

	const int cpUUHeaderLen = cpHeaderLen + sizeof(NET_USER_ASK::HEAD) + (sizeof(WORD) * xlive_sockets.size());
	int cpTotalLen = cpUUHeaderLen + dataLen;

	// Check overflow condition.
	if (cpTotalLen < 0) {
		// Send only UNKNOWN_USER_ASK.
		cpTotalLen = cpUUHeaderLen;
	}

	BYTE *cpBuf = (BYTE*)malloc(cpTotalLen);
	cpBuf[0] = XLLN_CUSTOM_PACKET_SENTINEL;
	cpBuf[sizeof(XLLN_CUSTOM_PACKET_SENTINEL)] = isAsking ? XLLNCustomPacketType::UNKNOWN_USER_ASK : XLLNCustomPacketType::UNKNOWN_USER_REPLY;
	NET_USER_ASK &nea = *(NET_USER_ASK*)&cpBuf[cpHeaderLen];
	nea.HEAD.numOfPorts = (WORD)xlive_sockets.size();
	DWORD i = 0;
	for (auto const &socket : xlive_sockets) {
		nea.ports[i++] = socket.second->hPort;
	}

	LeaveCriticalSection(&xlive_critsec_sockets);

	nea.HEAD.xnAddr = xlive_local_xnAddr;

	if (cpTotalLen > cpUUHeaderLen) {
		memcpy_s(&cpBuf[cpUUHeaderLen], dataLen, data, dataLen);
	}

	INT bytesSent = sendto(socket, (char*)cpBuf, cpTotalLen, 0, to, tolen);

	if (bytesSent <= 0 && cpTotalLen > cpUUHeaderLen) {
		// Send only UNKNOWN_USER_ASK.
		bytesSent = sendto(socket, (char*)cpBuf, cpUUHeaderLen, 0, to, tolen);
	}

	/*char buffer[256];
	snprintf(buffer, 256, "CPU BUFFER: %d", cpBuf);
	addDebugText(buffer);

	//NEW_FUNCT - Checking for buffer value
	if (!cpBuf)  {

		addDebugText("Buffer was empty!");

	}
	else
	{

		free(cpBuf);

	}*/

}

VOID SendUnknownUserAskRequest(SOCKET socket, char* data, int dataLen, sockaddr *to, int tolen)
{
	SendUnknownUserAskRequest(socket, data, dataLen, to, tolen, true);
}

static VOID CustomMemCpy(void *dst, void *src, rsize_t len, bool directionAscending)
{
	if (directionAscending) {
		for (rsize_t i = 0; i < len; i++) {
			((BYTE*)dst)[i] = ((BYTE*)src)[i];
		}
	}
	else {
		if (len > 0) {
			for (rsize_t i = len - 1; true; i--) {
				((BYTE*)dst)[i] = ((BYTE*)src)[i];
				if (i == 0) {
					break;
				}
			}
		}
	}
}

// #3
SOCKET WINAPI XSocketCreate(int af, int type, int protocol)
{
	TRACE_FX();
	bool vdp = false;
	if (protocol == IPPROTO_VDP) {
		vdp = true;
		// We can't support VDP (Voice / Data Protocol) it's some encrypted crap which isn't standard.
		protocol = IPPROTO_UDP;
	}

	SOCKET result = socket(af, type, protocol);

	if (vdp) {
		//("Socket: %08X was VDP", result);
		xlive_VDP_socket = result;
	}

	if (result == INVALID_SOCKET) {
		__debugbreak();
	}
	else {
		SocketInfo* si = (SocketInfo*)malloc(sizeof(SocketInfo));
		si->protocol = protocol;
		si->hPort = 0;
		EnterCriticalSection(&xlive_critsec_sockets);
		xlive_sockets[result] = si;
		LeaveCriticalSection(&xlive_critsec_sockets);
	}

	return result;
}

// #4
INT WINAPI XSocketClose(SOCKET s)
{
	TRACE_FX();
	INT result = closesocket(s);

	EnterCriticalSection(&xlive_critsec_sockets);
	SocketInfo *si = xlive_sockets[s];
	free(si);
	xlive_sockets.erase(s);
	LeaveCriticalSection(&xlive_critsec_sockets);

	return result;
}

// #5
INT WINAPI XSocketShutdown(SOCKET s, int how)
{
	TRACE_FX();
	INT result = shutdown(s, how);

	EnterCriticalSection(&xlive_critsec_sockets);
	xlive_sockets[s]->hPort = 0;
	LeaveCriticalSection(&xlive_critsec_sockets);

	return result;
}

// #6
INT WINAPI XSocketIOCTLSocket(SOCKET s, __int32 cmd, ULONG *argp)
{
	TRACE_FX();
	INT result = ioctlsocket(s, cmd, argp);
	return result;
}

// #7
INT WINAPI XSocketSetSockOpt(SOCKET s, int level, int optname, const char *optval, int optlen)
{
	TRACE_FX();
	if ((level & SO_BROADCAST) > 0) {
		//"XSocketSetSockOpt - SO_BROADCAST";
	}

	INT result = setsockopt(s, level, optname, optval, optlen);
	if (result == SOCKET_ERROR) {
		__debugbreak();
	}

	return result;
}

// #8
INT WINAPI XSocketGetSockOpt(SOCKET s, int level, int optname, char *optval, int *optlen)
{
	TRACE_FX();
	INT result = getsockopt(s, level, optname, optval, optlen);
	return result;
}

// #9
INT WINAPI XSocketGetSockName(SOCKET s, struct sockaddr *name, int *namelen)
{
	TRACE_FX();
	INT result = getsockname(s, name, namelen);
	return result;
}

// #10
INT WINAPI XSocketGetPeerName(SOCKET s, struct sockaddr *name, int *namelen)
{
	TRACE_FX();
	INT result = getpeername(s, name, namelen);
	return result;
}

// #11
SOCKET WINAPI XSocketBind(SOCKET s, const struct sockaddr *name, int namelen)
{
	TRACE_FX();
	WORD hPort = ntohs(((struct sockaddr_in*)name)->sin_port);
	WORD port_base = (hPort / 1000) * 1000;
	WORD port_offset = hPort % 1000;
	WORD hPortShift = xlive_base_port + port_offset;

	if (port_offset == 0) {
		xlive_liveoverlan_socket = s;
	}
	if (false) {}
	//else if (s == xlive_VDP_socket) {
	//	__debugbreak();
	//	//("[h2mod-voice] Game bound potential voice socket to : %i", ntohs(port));
	//	((struct sockaddr_in*)name)->sin_port = htons(xlive_base_port + 10);
	//}
	else {
		((struct sockaddr_in*)name)->sin_port = htons(hPortShift);
	}

	SOCKET result = bind(s, name, namelen);

	if (result == SOCKET_ERROR) {
		char debugText[200];
		snprintf(debugText, sizeof(debugText), "Socket Bind ERROR.\nAnother program has taken port:\nBase: %hd\nOffset: %hd\nNew: %hd\nOriginal: %hd", xlive_base_port, port_offset, hPortShift, hPort);
		XllnDebugBreak(debugText);
	}
	else {
		EnterCriticalSection(&xlive_critsec_sockets);
		xlive_sockets[s]->hPort = hPortShift;
		LeaveCriticalSection(&xlive_critsec_sockets);
	}
	return result;
}

// #12
INT WINAPI XSocketConnect(SOCKET s, const struct sockaddr *name, int namelen)
{
	TRACE_FX();

	ULONG niplong = ((struct sockaddr_in*)name)->sin_addr.s_addr;
	ULONG hIPv4 = ntohl(niplong);
	WORD hPort = ntohs(((struct sockaddr_in*)name)->sin_port);
	WORD port_base = (hPort / 1000) * 1000;
	WORD port_offset = hPort % 1000;

	EnterCriticalSection(&xlln_critsec_net_entity);

	NET_ENTITY *ne;
	if (NetEntityGetSecure(ne, hIPv4) == ERROR_SUCCESS) {
		DWORD nIPv4 = ne->xnAddr.ina.s_addr;
		((struct sockaddr_in*)name)->sin_addr.s_addr = nIPv4;

		std::pair<DWORD, WORD> host_pair = std::make_pair(ntohl(nIPv4), hPort);
		if (ne->host_pair_mappings.count(host_pair)) {
			((struct sockaddr_in*)name)->sin_port = htons(ne->host_pair_mappings[host_pair]);
		}
		else {
			__debugbreak();
		}
	}
	else {
		__debugbreak();
	}

	LeaveCriticalSection(&xlln_critsec_net_entity);

	INT result = connect(s, name, namelen);
	return result;
}

// #13
INT WINAPI XSocketListen(SOCKET s, int backlog)
{
	TRACE_FX();
	INT result = listen(s, backlog);
	return result;
}

// #14
SOCKET WINAPI XSocketAccept(SOCKET s, struct sockaddr *addr, int *addrlen)
{
	TRACE_FX();
	SOCKET result = accept(s, addr, addrlen);
	return result;
}

// #15
INT WINAPI XSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout)
{
	TRACE_FX();
	INT result = select(nfds, readfds, writefds, exceptfds, timeout);
	return result;
}

// #18
INT WINAPI XSocketRecv(SOCKET s, char * buf, int len, int flags)
{
	TRACE_FX();
	INT result = recv(s, buf, len, flags);
	return result;
}

INT WINAPI XSocketRecvFromHelper(INT result, SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	//if (result > 0) {
		addDebugText(__func__);
		const u_long niplong = ((struct sockaddr_in*)from)->sin_addr.s_addr;
		const WORD hPort = ntohs(((struct sockaddr_in*)from)->sin_port);
		const std::pair<DWORD, WORD> host_pair_resolved = std::make_pair(ntohl(niplong), hPort);

		if (buf[0] == XLLN_CUSTOM_PACKET_SENTINEL) {
			const int cpHeaderLen = sizeof(XLLN_CUSTOM_PACKET_SENTINEL) + sizeof(XLLNCustomPacketType::Type);
			if (result < cpHeaderLen) {
				addDebugText("XLLN: ERROR custom packet received was too short!");
				return 0;
			}
			switch (buf[sizeof(XLLN_CUSTOM_PACKET_SENTINEL)]) {
				case XLLNCustomPacketType::STOCK_PACKET: {
					CustomMemCpy(buf, (void*)((DWORD)buf + cpHeaderLen), result - cpHeaderLen, true);
					result -= cpHeaderLen;
					break;
				}
				case XLLNCustomPacketType::STOCK_PACKET_FORWARDED: {
					((struct sockaddr_in*)from)->sin_addr.s_addr = ((XNADDR*)(buf + cpHeaderLen))->inaOnline.s_addr;
					//TODO ports?
					CustomMemCpy(buf, (void*)((DWORD)buf + cpHeaderLen + sizeof(XNADDR)), result - cpHeaderLen + sizeof(XNADDR), true);
					result -= cpHeaderLen + sizeof(XNADDR);
					/*if (result <= 0) {
						goto RETURN_LE_ZERO;
					}*/
					return result;
				}
				case XLLNCustomPacketType::UNKNOWN_USER_ASK:
				case XLLNCustomPacketType::UNKNOWN_USER_REPLY: {
					// Less than since there is likely another packet pushed onto the end of this one.
					if (result < cpHeaderLen + sizeof(NET_USER_ASK::HEAD)) {
						addDebugText("XLLN: ERROR INVALID UNKNOWN_USER_<?> Recieved.");
						return 0;
					}
					addDebugText("XLLN: UNKNOWN_USER_<?> Recieved.");
					NET_USER_ASK &nea = *(NET_USER_ASK*)&buf[cpHeaderLen];
					nea.HEAD.xnAddr.ina.s_addr = niplong;
					NetEntityCreate(&nea.HEAD.xnAddr);
					DWORD userId = ntohl(nea.HEAD.xnAddr.inaOnline.s_addr) >> 8;
					ULONG hIPv4 = ntohl(niplong);
					EnterCriticalSection(&xlln_critsec_net_entity);
					if (xlln_net_entity_userId.count(userId)) {
						NET_ENTITY *ne = xlln_net_entity_userId[userId];
						//TODO replace with hole punshing
						for (DWORD i = 0; i < nea.HEAD.numOfPorts; i++) {
							NetEntityAddMapping_(ne, hIPv4, nea.ports[i], nea.ports[i]);
						}
					}
					LeaveCriticalSection(&xlln_critsec_net_entity);

					XLLNCustomPacketType::Type &packet_type = *(XLLNCustomPacketType::Type*)&buf[sizeof(XLLN_CUSTOM_PACKET_SENTINEL)];

					const int UnknownUserPacketLen = cpHeaderLen + sizeof(NET_USER_ASK::HEAD) + (sizeof(WORD) * nea.HEAD.numOfPorts);

					if (packet_type == XLLNCustomPacketType::UNKNOWN_USER_ASK) {
						addDebugText("XLLN: UNKNOWN_USER_ASK Sending REPLY.");
						packet_type = XLLNCustomPacketType::UNKNOWN_USER_REPLY;

						SendUnknownUserAskRequest(s, (char*)((DWORD)buf + UnknownUserPacketLen), result - UnknownUserPacketLen, from, *fromlen, false);

						//TODO for efficiency can use current data if enough space avail.
						/*nea.HEAD.xnAddr = xlive_local_xnAddr;

						EnterCriticalSection(&xlive_critsec_sockets);

						if (nea.HEAD.numOfPorts < xlive_sockets.size()) {

						}

						LeaveCriticalSection(&xlive_critsec_sockets);

						INT bytesSent = sendto(s, buf, result, 0, from, *fromlen);*/
						return 0;
					}
					else {
						addDebugText("XLLN: UNKNOWN_USER_REPLY passing original data back.");
						//CustomMemCpy(buf, (void*)((DWORD)buf + UnknownUserPacketLen), result - UnknownUserPacketLen, true);
						result -= UnknownUserPacketLen;
						return XSocketRecvFromHelper(result, s, (char*)((DWORD)buf + UnknownUserPacketLen), len, flags, from, fromlen);
					}
				}
				case XLLNCustomPacketType::LIVE_OVER_LAN_ADVERTISE:
				case XLLNCustomPacketType::LIVE_OVER_LAN_UNADVERTISE: {
					LiveOverLanRecieve(s, from, *fromlen, host_pair_resolved, (const LIVE_SERVER_DETAILS*)buf, result);
					return 0;
				}
				default: {
					addDebugText("XLLN: ERROR unknown custom packet type received!");
					break;
				}
			}
		}
		
		/*if (result <= 0) {
			RETURN_LE_ZERO:
			if (result < 0) {
				XllnDebugBreak("XLLN: ERROR XSocketRecvFrom result became less than 0!");
			}
			return 0;
		}*/

		EnterCriticalSection(&xlln_critsec_net_entity);

		NET_ENTITY *ne;
		if (NetEntityGetHostPairResolved(ne, host_pair_resolved) == ERROR_SUCCESS) {
			((struct sockaddr_in*)from)->sin_addr.s_addr = ne->xnAddr.inaOnline.s_addr;

			if (ne->host_pair_resolved.count(host_pair_resolved)) {
				((struct sockaddr_in*)from)->sin_port = htons(ne->host_pair_resolved[host_pair_resolved]);
			}
			else {
				LeaveCriticalSection(&xlln_critsec_net_entity);
				SendUnknownUserAskRequest(s, buf, result, from, *fromlen);
				return 0;
			}
		}
		else {
			LeaveCriticalSection(&xlln_critsec_net_entity);
			SendUnknownUserAskRequest(s, buf, result, from, *fromlen);
			return 0;
		}

		LeaveCriticalSection(&xlln_critsec_net_entity);
	//}
	return result;
}

// #20
INT WINAPI XSocketRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	TRACE_FX();
	INT result = recvfrom(s, buf, len, flags, from, fromlen);
	result = XSocketRecvFromHelper(result, s, buf, len, flags, from, fromlen);
	return result;
}

// #22
INT WINAPI XSocketSend(SOCKET s, const char *buf, int len, int flags)
{
	TRACE_FX();
	INT result = send(s, buf, len, flags);
	return result;
}

INT WINAPI XllnSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen)
{
	TRACE_FX();
	INT result = SOCKET_ERROR;

	WORD nPort = ((struct sockaddr_in*)to)->sin_port;
	WORD hPort = ntohs(nPort);
	ULONG nIPv4 = ((struct sockaddr_in*)to)->sin_addr.s_addr;
	ULONG hIPv4 = ntohl(nIPv4);
	WORD port_base = (hPort / 1000) * 1000;
	WORD port_offset = hPort % 1000;
	ADDRESS_FAMILY af = ((struct sockaddr_in*)to)->sin_family;

	if (hIPv4 == INADDR_BROADCAST || hIPv4 == INADDR_ANY) {
		addDebugText("XSocketSendTo() - Broadcast.");

		((struct sockaddr_in*)to)->sin_addr.s_addr = htonl(xlive_network_adapter.hBroadcast);

		for (WORD port_base = 1000; port_base <= 6000; port_base += 1000) {
			((struct sockaddr_in*)to)->sin_port = htons(port_base + port_offset);
			result = sendto(s, buf, len, 0, to, tolen);
		}
	}
	else {

		EnterCriticalSection(&xlln_critsec_net_entity);

		NET_ENTITY *ne;
		if (NetEntityGetSecure(ne, hIPv4) == ERROR_SUCCESS) {
			DWORD nIPv4 = ne->xnAddr.ina.s_addr;
			((struct sockaddr_in*)to)->sin_addr.s_addr = nIPv4;

			std::pair<DWORD, WORD> hIPv4PortOffset = std::make_pair(ntohl(nIPv4), ne->hPortBase + port_offset);
			if (ne->host_pair_mappings.count(hIPv4PortOffset)) {
				((struct sockaddr_in*)to)->sin_port = htons(ne->host_pair_mappings[hIPv4PortOffset]);
			}
			else {
				__debugbreak();
			}
		}
		else {
			addDebugText("ERROR: Unable to find XNADDR, sending anyway.");
			//__debugbreak();
		}

		LeaveCriticalSection(&xlln_critsec_net_entity);

		result = sendto(s, buf, len, flags, to, tolen);
	}

	if (result == SOCKET_ERROR) {
		INT Error = WSAGetLastError();
		__debugbreak();
	}

	return result;
}

// #24
INT WINAPI XSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen)
{
	TRACE_FX();
	INT result = SOCKET_ERROR;

	// Check if the first byte is the same as the custom XLLN packets.
	// If so wrap the data in a new message.
	if (buf[0] == XLLN_CUSTOM_PACKET_SENTINEL) {
		addDebugText("Stock 00 Packet Adapted.");
		const int altBufLen = len + 2;
		// Check overflow condition.
		if (altBufLen < 0) {
			WSASetLastError(WSAEMSGSIZE);
			return SOCKET_ERROR;
		}

		char *altBuf = (char*)malloc(sizeof(char) * altBufLen);

		altBuf[0] = XLLN_CUSTOM_PACKET_SENTINEL;
		altBuf[1] = XLLNCustomPacketType::STOCK_PACKET;
		memcpy_s(&altBuf[2], altBufLen-2, buf, len);

		result = XllnSocketSendTo(s, altBuf, altBufLen, flags, to, tolen);

		free(altBuf);
	}
	else {
		result = XllnSocketSendTo(s, buf, len, flags, to, tolen);
	}

	return result;
}

// #26
VOID XSocketInet_Addr()
{
	TRACE_FX();
	FUNC_STUB();
}

// #27
INT WINAPI XSocketWSAGetLastError()
{
	TRACE_FX();
	INT result = WSAGetLastError();
	return result;
}

// #37
ULONG WINAPI XSocketHTONL(ULONG hostlong)
{
	TRACE_FX();
	ULONG result = htonl(hostlong);
	return result;
}

// #38
USHORT WINAPI XSocketNTOHS(USHORT netshort)
{
	TRACE_FX();
	USHORT result = ntohs(netshort);
	return result;
}

// #39
ULONG WINAPI XSocketNTOHL(ULONG netlong)
{
	TRACE_FX();
	ULONG result = ntohl(netlong);
	return result;
}

// #40
USHORT WINAPI XSocketHTONS(USHORT hostshort)
{
	TRACE_FX();
	USHORT result = htons(hostshort);
	return result;
}

BOOL InitXSocket()
{
	InitializeCriticalSection(&xlive_critsec_sockets);

	return TRUE;
}

BOOL UninitXSocket()
{
	DeleteCriticalSection(&xlive_critsec_sockets);

	return TRUE;
}
