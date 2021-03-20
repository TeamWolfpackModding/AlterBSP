#include "xdefs.h"
#include "NetEntity.h"
#include "../xlln/DebugText.h"
#include <vector>
#include <set>

CRITICAL_SECTION xlln_critsec_net_entity;

std::map<DWORD, NET_ENTITY*> xlln_net_entity_userId;
std::map<std::pair<DWORD, WORD>, NET_ENTITY*> xlln_net_entity_IPv4PortBase;
std::map<std::pair<DWORD, WORD>, NET_ENTITY*> xlln_net_entity_IPv4PortOffset;
std::map<std::pair<DWORD, WORD>, NET_ENTITY*> xlln_net_entity_IPv4PortResolved;

// REQUIRES critsec
INT NetEntityGetSecure(NET_ENTITY *&ne, DWORD inaOnlineSecure)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;
	DWORD userId = inaOnlineSecure >> 8;
	if (xlln_net_entity_userId.count(userId)) {
		ne = xlln_net_entity_userId[userId];
		result = ERROR_SUCCESS;
	}
	else {
		result = ERROR_NOT_FOUND;
	}
	return result;
}

INT NetEntityGetSecure(XNADDR &xnAddr, DWORD inaOnlineSecure)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;
	DWORD userId = inaOnlineSecure >> 8;

	EnterCriticalSection(&xlln_critsec_net_entity);
	
	if (xlln_net_entity_userId.count(userId)) {
		NET_ENTITY* result_ne = xlln_net_entity_userId[userId];
		xnAddr = result_ne->xnAddr;
		result = ERROR_SUCCESS;
	}
	else {
		result = ERROR_NOT_FOUND;
	}

	LeaveCriticalSection(&xlln_critsec_net_entity);

	return result;
}

// REQUIRES critsec
INT NetEntityGetHostPairResolved(NET_ENTITY *&ne, std::pair<DWORD, WORD> host_pair)
{
	TRACE_FX();
	
	INT result = ERROR_UNHANDLED_ERROR;
	if (xlln_net_entity_IPv4PortResolved.count(host_pair)) {
		ne = xlln_net_entity_IPv4PortResolved[host_pair];
		result = ERROR_SUCCESS;
	}
	else {
		result = ERROR_NOT_FOUND;
	}
	return result;
}

INT NetEntityGetHostPairBase(XNADDR &xnAddr, std::pair<DWORD, WORD> host_pair)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;

	EnterCriticalSection(&xlln_critsec_net_entity);

	if (xlln_net_entity_IPv4PortBase.count(host_pair)) {
		NET_ENTITY *result_ne = xlln_net_entity_IPv4PortBase[host_pair];
		xnAddr = result_ne->xnAddr;
		result = ERROR_SUCCESS;
	}
	else {
		result = ERROR_NOT_FOUND;
	}

	LeaveCriticalSection(&xlln_critsec_net_entity);

	return result;
}

INT NetEntityGetHostPairResolved(XNADDR &xnAddr, std::pair<DWORD, WORD> host_pair)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;

	EnterCriticalSection(&xlln_critsec_net_entity);

	if (xlln_net_entity_IPv4PortResolved.count(host_pair)) {
		NET_ENTITY *result_ne = xlln_net_entity_IPv4PortResolved[host_pair];
		xnAddr = result_ne->xnAddr;
		result = ERROR_SUCCESS;
	}
	else {
		result = ERROR_NOT_FOUND;
	}

	LeaveCriticalSection(&xlln_critsec_net_entity);

	return result;
}

// critsec
INT NetEntityAdd_(NET_ENTITY *ne, DWORD IPv4, WORD portOffset, WORD portResolved)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;

	std::pair<DWORD, WORD> host_pair = std::make_pair(IPv4, portOffset);
	ne->host_pair_mappings[host_pair] = portResolved;
	xlln_net_entity_IPv4PortBase[std::make_pair(IPv4, ne->hPortBase)] = ne;

	result = ERROR_SUCCESS;
	return result;
}

// critsec
INT NetEntityCreate_(DWORD userId, WORD basePort, XUID *xuid, XNADDR *xnAddr)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;

	NET_ENTITY *ne = new NET_ENTITY;
	ne->userId = userId;
	ne->hPortBase = basePort;
	ne->xuid = *xuid;
	ne->xnAddr = *xnAddr;

	xlln_net_entity_userId[userId] = ne;

	result = ERROR_SUCCESS;
	return result;
}

// critsec
INT NetEntityGetUserId_(NET_ENTITY *&ne, DWORD userId)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;
	ne = xlln_net_entity_userId[userId];
	result = ERROR_SUCCESS;
	return result;
}

INT NetEntityGetIPv4Base(NET_ENTITY *&ne, DWORD IPv4, WORD portBase)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;
	EnterCriticalSection(&xlln_critsec_net_entity);
	ne = xlln_net_entity_IPv4PortBase[std::make_pair(IPv4, portBase)];
	LeaveCriticalSection(&xlln_critsec_net_entity);
	result = ERROR_SUCCESS;
	return result;
}

INT NetEntityGetUserId(NET_ENTITY *&ne, DWORD userId)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;
	EnterCriticalSection(&xlln_critsec_net_entity);
	result = NetEntityGetUserId_(ne, userId);
	LeaveCriticalSection(&xlln_critsec_net_entity);
	return result;
}

INT NetEntityCreate(DWORD userId, WORD basePort, XUID *xuid, XNADDR *xnAddr)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;
	EnterCriticalSection(&xlln_critsec_net_entity);
	result = NetEntityCreate_(userId, basePort, xuid, xnAddr);
	LeaveCriticalSection(&xlln_critsec_net_entity);
	return result;
}

INT NetEntityCreate(DWORD userId, XUID xuid, WORD basePort, DWORD IPv4, WORD portOffset, WORD portResolved)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;
	EnterCriticalSection(&xlln_critsec_net_entity);
	//result = NetEntityCreate(userId, xuid, basePort);
	NET_ENTITY *ne;
	if (result == ERROR_SUCCESS && (result = NetEntityGetUserId_(ne, userId)) == ERROR_SUCCESS) {
		result = NetEntityAdd_(ne, IPv4, portOffset, portResolved);
	}
	LeaveCriticalSection(&xlln_critsec_net_entity);
	return result;
}

// REQUIRES critsec
INT NetEntityUpdate_(XNADDR *pXnAddr)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;

	DWORD userId = ntohl(pXnAddr->inaOnline.s_addr) >> 8;
	DWORD hIPv4 = ntohl(pXnAddr->ina.s_addr);
	WORD hPortBase = ntohs(pXnAddr->wPortOnline);
	std::pair<DWORD, WORD> hIPv4PortBase = std::make_pair(hIPv4, hPortBase);

	NET_ENTITY *ne = NULL;

	if (xlln_net_entity_userId.count(userId)) {
		ne = xlln_net_entity_userId[userId];
	}
	else if (xlln_net_entity_IPv4PortBase.count(hIPv4PortBase)) {
		ne = xlln_net_entity_IPv4PortBase[hIPv4PortBase];
	}
	else if (xlln_net_entity_IPv4PortOffset.count(hIPv4PortBase)) {
		ne = xlln_net_entity_IPv4PortOffset[hIPv4PortBase];
	}
	else {
		result = ERROR_NOT_FOUND;
	}

	if (ne) {

		if (ne->userId != userId) {
			DWORD userId2 = ne->userId;
			ne->userId = userId;

			__debugbreak();
			// Remove userId2 dupe.

			xlln_net_entity_userId[userId] = ne;
		}

		if (ne->hPortBase != hPortBase) {
			__debugbreak();
			// Remove all ip:port combos.
			//host_pair_mappings
			//host_pair_resolved

			ne->host_pair_mappings[hIPv4PortBase] = 0;
			//ne->host_pair_mappings[hIPv4PortBase] = hPortBase;

			ne->hPortBase = hPortBase;
			xlln_net_entity_IPv4PortBase[hIPv4PortBase] = ne;
			xlln_net_entity_IPv4PortOffset[hIPv4PortBase] = ne;
		}
		
		if (xlln_net_entity_IPv4PortBase.count(hIPv4PortBase)) {
			NET_ENTITY *ne2 = xlln_net_entity_IPv4PortBase[hIPv4PortBase];
			if (ne2 != ne) {
				__debugbreak();
				// Remove ip dupe.

				xlln_net_entity_IPv4PortBase[hIPv4PortBase] = ne;
			}
		}

		if (xlln_net_entity_IPv4PortOffset.count(hIPv4PortBase)) {
			NET_ENTITY *ne2 = xlln_net_entity_IPv4PortOffset[hIPv4PortBase];
			if (ne2 != ne) {
				__debugbreak();
				// Remove ip dupe.
				
				xlln_net_entity_IPv4PortOffset[hIPv4PortBase] = ne;
			}
		}

		if (ne->host_pair_mappings.count(hIPv4PortBase)) {
			WORD hPortResolved = ne->host_pair_mappings[hIPv4PortBase];
			// FIXME
			if (false && hPortResolved != 0) {
				std::pair<DWORD, WORD> hIPv4PortResolvedOld = std::make_pair(hIPv4, hPortResolved);
				ne->host_pair_resolved.erase(hIPv4PortResolvedOld);
				xlln_net_entity_IPv4PortResolved.erase(hIPv4PortResolvedOld);

				ne->host_pair_mappings[hIPv4PortBase] = 0;
				//ne->host_pair_mappings[hIPv4PortBase] = hPortBase;
				//xlln_net_entity_IPv4PortResolved[hIPv4PortResolvedOld] = ne;
				//xlln_net_entity_IPv4PortOffset[hIPv4PortBase] = ne;
			}
		}

		result = ERROR_SUCCESS;
	}

	return result;
}

// REQUIRES critsec
INT NetEntityAddMapping_(NET_ENTITY *ne, DWORD hIPv4, WORD portOffset, WORD portResolved)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;

	std::pair<DWORD, WORD> hIPv4PortOffset = std::make_pair(hIPv4, portOffset);
	std::pair<DWORD, WORD> hIPv4PortResolved = std::make_pair(hIPv4, portResolved);

	if (ne->host_pair_mappings.count(hIPv4PortOffset)) {
		std::pair<DWORD, WORD> hIPv4PortResolvedOld = std::make_pair(hIPv4, ne->host_pair_mappings[hIPv4PortOffset]);
		ne->host_pair_resolved.erase(hIPv4PortResolvedOld);
		xlln_net_entity_IPv4PortResolved.erase(hIPv4PortResolvedOld);
	}
	ne->host_pair_mappings[hIPv4PortOffset] = portResolved;
	ne->host_pair_resolved[hIPv4PortResolved] = portOffset;

	if (portOffset == ne->hPortBase) {
		xlln_net_entity_IPv4PortBase[hIPv4PortOffset] = ne;
	}
	xlln_net_entity_IPv4PortOffset[hIPv4PortOffset] = ne;
	xlln_net_entity_IPv4PortResolved[hIPv4PortResolved] = ne;

	result = ERROR_SUCCESS;
	return result;
}

INT NetEntityCreate(XNADDR *pXnAddr)
{
	TRACE_FX();

	INT result = ERROR_UNHANDLED_ERROR;

	DWORD userId = ntohl(pXnAddr->inaOnline.s_addr) >> 8;
	DWORD hIPv4 = ntohl(pXnAddr->ina.s_addr);
	WORD hPortBase = ntohs(pXnAddr->wPortOnline);
	std::pair<DWORD, WORD> hIPv4PortBase = std::make_pair(hIPv4, hPortBase);

	NET_ENTITY *ne = new NET_ENTITY;
	ne->userId = userId;
	ne->xnAddr = *pXnAddr;
	ne->hPortBase = hPortBase;
	ne->xuid = 0;
	//ne->host_pair_mappings[hIPv4PortBase] = 0;
	ne->host_pair_resolved;

	EnterCriticalSection(&xlln_critsec_net_entity);
	if (!xlln_net_entity_userId.count(userId)) {

		xlln_net_entity_userId[userId] = ne;
		xlln_net_entity_IPv4PortBase[hIPv4PortBase] = ne;
		xlln_net_entity_IPv4PortOffset[hIPv4PortBase] = ne;

		result = ERROR_SUCCESS;

		LeaveCriticalSection(&xlln_critsec_net_entity);
	}
	else {
		result = NetEntityUpdate_(pXnAddr);

		LeaveCriticalSection(&xlln_critsec_net_entity);

		delete ne;
	}

	return result;
}

BOOL InitNetEntity()
{
	TRACE_FX();

	InitializeCriticalSection(&xlln_critsec_net_entity);

	return TRUE;
}

BOOL UninitNetEntity()
{
	TRACE_FX();
	
	DeleteCriticalSection(&xlln_critsec_net_entity);

	return TRUE;
}
