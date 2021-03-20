#pragma once

extern BOOL xlive_net_initialized;
extern XNADDR xlive_local_xnAddr;

HRESULT WINAPI XNetCreateKey(XNKID *pxnkid, XNKEY *pxnkey);
