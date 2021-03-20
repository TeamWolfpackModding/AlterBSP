#pragma once
#include <map>
#include <vector>

typedef struct {
	XLOCATOR_SEARCHRESULT *searchresult;
	DWORD broadcastTime;
} XLOCATOR_SESSION;

#pragma pack(push, 1) // Save then set byte alignment setting.
typedef struct {
	struct {
		BYTE bSentinel;
		BYTE bCustomPacketType;
	} HEAD;
	union {
		struct {
			XUID xuid;
			XNADDR xnAddr;
			DWORD dwServerType;
			XNKID xnkid;
			XNKEY xnkey;
			DWORD dwMaxPublicSlots;
			DWORD dwMaxPrivateSlots;
			DWORD dwFilledPublicSlots;
			DWORD dwFilledPrivateSlots;
			DWORD cProperties;
			union {
				DWORD pProperties;
				DWORD propsSize;
			};
		} ADV;
		struct {
			XUID xuid;
		} UNADV;
	};
} LIVE_SERVER_DETAILS;
#pragma pack(pop) // Return to original alignment setting.

extern CRITICAL_SECTION xlive_xlocator_enumerators_lock;
extern std::map<HANDLE, std::vector<std::pair<DWORD, WORD>>> xlive_xlocator_enumerators;
extern CRITICAL_SECTION xlive_critsec_LiveOverLan_broadcast_handler;
extern VOID(WINAPI *liveoverlan_broadcast_handler)(LIVE_SERVER_DETAILS*);
extern std::map<std::pair<DWORD, WORD>, XLOCATOR_SESSION*> liveoverlan_sessions;
extern CRITICAL_SECTION liveoverlan_sessions_lock;

VOID LiveOverLanAbort();
VOID LiveOverLanClone(XLOCATOR_SEARCHRESULT **dst, XLOCATOR_SEARCHRESULT *src);
VOID LiveOverLanDelete(XLOCATOR_SEARCHRESULT *xlocator_result);
BOOL LiveOverLanBroadcastReceive(PXLOCATOR_SEARCHRESULT *result, BYTE *buf, DWORD buflen);
VOID LiveOverLanRecieve(SOCKET socket, sockaddr *to, int tolen, const std::pair<DWORD, WORD> hostpair, const LIVE_SERVER_DETAILS *session_details, INT &len);
