#pragma once
#include <map>

typedef struct {
	DWORD userId;
	XUID xuid;
	WORD hPortBase;
	std::map<std::pair<DWORD, WORD>, WORD> host_pair_mappings;
	std::map<std::pair<DWORD, WORD>, WORD> host_pair_resolved;
	XNADDR xnAddr;
} NET_ENTITY;

extern CRITICAL_SECTION xlln_critsec_net_entity;
extern std::map<DWORD, NET_ENTITY*> xlln_net_entity_userId;

BOOL InitNetEntity();
BOOL UninitNetEntity();

INT NetEntityGetSecure(NET_ENTITY *&ne, DWORD inaOnlineSecure);
INT NetEntityGetSecure(XNADDR &xnAddr, DWORD inaOnlineSecure);
INT NetEntityGetHostPairResolved(NET_ENTITY *&ne, std::pair<DWORD, WORD> host_pair);
INT NetEntityGetHostPairBase(XNADDR &xnAddr, std::pair<DWORD, WORD> host_pair);
INT NetEntityGetHostPairResolved(XNADDR &xnAddr, std::pair<DWORD, WORD> host_pair);

INT NetEntityCreate(DWORD userId, WORD basePort, XUID *xuid, XNADDR *xnAddr);
INT NetEntityAddMapping_(NET_ENTITY *ne, DWORD hIPv4, WORD portOffset, WORD portResolved);
INT NetEntityCreate(XNADDR *pXnAddr);
