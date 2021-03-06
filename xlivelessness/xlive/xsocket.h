#pragma once

extern WORD xlive_base_port;
extern BOOL xlive_netsocket_abort;
extern SOCKET xlive_liveoverlan_socket;

#define XLLN_CUSTOM_PACKET_SENTINEL (BYTE)0x00
namespace XLLNCustomPacketType {
	enum Type : BYTE {
		UNKNOWN = 0x00,
		STOCK_PACKET,
		STOCK_PACKET_FORWARDED,
		CUSTOM_OTHER,
		UNKNOWN_USER_ASK,
		UNKNOWN_USER_REPLY,
		LIVE_OVER_LAN_ADVERTISE,
		LIVE_OVER_LAN_UNADVERTISE,

	};
}

BOOL InitXSocket();
BOOL UninitXSocket();

VOID SendUnknownUserAskRequest(SOCKET socket, char* data, int dataLen, sockaddr *to, int tolen);

INT WINAPI XllnSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen);
