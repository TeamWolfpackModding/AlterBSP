#include "xdefs.h"
#include "xstorage.h"
#include "xlive.h"
#include "../xlln/DebugText.h"

// #5304
VOID XStorageUploadFromMemoryGetProgress()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5305
DWORD WINAPI XStorageUploadFromMemory(DWORD dwUserIndex, const WCHAR *wszServerPath, DWORD dwBufferSize, const BYTE *pbBuffer, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (!wszServerPath)
		return ERROR_INVALID_PARAMETER;
	if (!dwBufferSize)
		return ERROR_INVALID_PARAMETER;
	if (!pbBuffer)
		return ERROR_INVALID_PARAMETER;

	//TODO XStorageUploadFromMemory
	if (pXOverlapped) {
		//asynchronous

		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = ERROR_SUCCESS;
		pXOverlapped->dwExtendedError = ERROR_SUCCESS;

		Check_Overlapped(pXOverlapped);

		return ERROR_IO_PENDING;
	}
	else {
		//synchronous
		//return result;
	}
	return ERROR_SUCCESS;
}

// #5306
VOID XStorageEnumerate()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5307
VOID XStorageDownloadToMemoryGetProgress()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5308
DWORD WINAPI XStorageDelete(DWORD dwUserIndex, const WCHAR *wszServerPath, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (!wszServerPath)
		return ERROR_INVALID_PARAMETER;

	//TODO XStorageDelete
	if (pXOverlapped) {
		//asynchronous

		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = ERROR_SUCCESS;
		pXOverlapped->dwExtendedError = ERROR_SUCCESS;

		Check_Overlapped(pXOverlapped);

		return ERROR_IO_PENDING;
	}
	else {
		//synchronous
		//return result;
	}
	return ERROR_SUCCESS;
}

// #5309
VOID XStorageBuildServerPathByXuid()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5344
DWORD WINAPI XStorageBuildServerPath(
	DWORD dwUserIndex,
	XSTORAGE_FACILITY StorageFacility,
	CONST void *pvStorageFacilityInfo,
	DWORD dwStorageFacilityInfoSize,
	LPCWSTR *lpwszItemName,
	WCHAR *pwszServerPath,
	DWORD *pdwServerPathLength)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (!pvStorageFacilityInfo && dwStorageFacilityInfoSize)
		return ERROR_INVALID_PARAMETER;
	if (!lpwszItemName)
		return ERROR_INVALID_PARAMETER;
	if (!*lpwszItemName)
		return ERROR_INVALID_PARAMETER;
	if (!pwszServerPath)
		return ERROR_INVALID_PARAMETER;
	if (!pdwServerPathLength)
		return ERROR_INVALID_PARAMETER;
	if (StorageFacility == XSTORAGE_FACILITY_GAME_CLIP && !pvStorageFacilityInfo)
		return ERROR_INVALID_PARAMETER;
	if (pvStorageFacilityInfo && dwStorageFacilityInfoSize < sizeof(XSTORAGE_FACILITY_GAME_CLIP))
		return ERROR_INVALID_PARAMETER;

	//TODO XStorageBuildServerPath

	return ERROR_FUNCTION_FAILED;
	return ERROR_SUCCESS;
}

// #5345
DWORD WINAPI XStorageDownloadToMemory(
	DWORD dwUserIndex,
	const WCHAR *wszServerPath,
	DWORD dwBufferSize,
	const BYTE *pbBuffer,
	DWORD cbResults,
	XSTORAGE_DOWNLOAD_TO_MEMORY_RESULTS *pResults,
	XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (!wszServerPath)
		return ERROR_INVALID_PARAMETER;
	if (!*wszServerPath)
		return ERROR_INVALID_PARAMETER;
	if (!dwBufferSize)
		return ERROR_INVALID_PARAMETER;
	if (!pbBuffer)
		return ERROR_INVALID_PARAMETER;
	if (cbResults != sizeof(XSTORAGE_DOWNLOAD_TO_MEMORY_RESULTS))
		return ERROR_INVALID_PARAMETER;
	if (!pResults)
		return ERROR_INVALID_PARAMETER;

	//TODO XStorageDownloadToMemory
	if (pXOverlapped) {
		//asynchronous

		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = ERROR_SUCCESS;
		pXOverlapped->dwExtendedError = ERROR_SUCCESS;

		Check_Overlapped(pXOverlapped);

		return ERROR_IO_PENDING;
	}
	else {
		//synchronous
		//return result;
	}
	return ERROR_SUCCESS;
}
