#include "xdefs.h"
#include "xuser.h"
#include "xlive.h"
#include "xsession.h"
#include "../xlln/DebugText.h"
#include <stdio.h>

//TODO move to a utils file.
void EnsureDirectoryExists(wchar_t* path) {
	int buflen = wcslen(path) + 1;
	wchar_t* path2 = (wchar_t*)malloc(sizeof(wchar_t) * buflen);
	memcpy(path2, path, sizeof(wchar_t) * buflen);

	for (int i = 1; i < buflen; i++) {
		if (path2[i] == L'/' || path2[i] == L'\\') {
			wchar_t temp_cut = 0;
			if (path2[i + 1] != 0) {
				temp_cut = path2[i + 1];
				path2[i + 1] = 0;
			}
			BOOL err_create = CreateDirectoryW(path2, NULL);
			if (temp_cut) {
				path2[i + 1] = temp_cut;
			}
		}
	}

	free(path2);
}

BOOL XLivepIsPropertyIdValid(DWORD dwPropertyId, BOOL a2)
{
	return !(dwPropertyId & X_PROPERTY_SCOPE_MASK)
		|| dwPropertyId == X_PROPERTY_RANK
		|| dwPropertyId == X_PROPERTY_SESSION_ID
		|| dwPropertyId == X_PROPERTY_GAMER_ZONE
		|| dwPropertyId == X_PROPERTY_GAMER_COUNTRY
		|| dwPropertyId == X_PROPERTY_GAMER_LANGUAGE
		|| dwPropertyId == X_PROPERTY_GAMER_RATING
		|| dwPropertyId == X_PROPERTY_GAMER_MU
		|| dwPropertyId == X_PROPERTY_GAMER_SIGMA
		|| dwPropertyId == X_PROPERTY_GAMER_PUID
		|| dwPropertyId == X_PROPERTY_AFFILIATE_SCORE
		|| dwPropertyId == X_PROPERTY_RELATIVE_SCORE
		|| dwPropertyId == X_PROPERTY_SESSION_TEAM
		|| !a2 && dwPropertyId == X_PROPERTY_GAMER_HOSTNAME;
}

// #5261
DWORD WINAPI XUserGetXUID(DWORD dwUserIndex, XUID *pxuid)
{
	TRACE_FX();
	if (!pxuid)
		return ERROR_INVALID_PARAMETER;
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;

	if (xlive_users_info[dwUserIndex]->UserSigninState & (eXUserSigninState_SignedInLocally | eXUserSigninState_SignedInToLive)) {
		*pxuid = xlive_users_info[dwUserIndex]->xuid;
		return ERROR_SUCCESS;
	}
	return ERROR_NOT_LOGGED_ON;
}

// #5262
XUSER_SIGNIN_STATE WINAPI XUserGetSigninState(DWORD dwUserIndex)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return eXUserSigninState_NotSignedIn;
	return xlive_users_info[dwUserIndex]->UserSigninState;
}

// #5263
DWORD WINAPI XUserGetName(DWORD dwUserIndex, LPSTR szUserName, DWORD cchUserName)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (!szUserName)
		return ERROR_INVALID_PARAMETER;
	if (!cchUserName)
		return ERROR_INVALID_PARAMETER;

	if (cchUserName > XUSER_NAME_SIZE)
		cchUserName = XUSER_NAME_SIZE;

	memcpy(szUserName, xlive_users_info[dwUserIndex]->szUserName, cchUserName);
	return ERROR_SUCCESS;
}

// #5264
INT WINAPI XUserAreUsersFriends(DWORD dwUserIndex, DWORD* pXuids, DWORD dwXuidCount, DWORD* pResult, PXOVERLAPPED pOverlapped)
{
	TRACE_FX();
	
	return ERROR_NOT_LOGGED_ON;
}

// #5265
DWORD WINAPI XUserCheckPrivilege(DWORD dwUserIndex, XPRIVILEGE_TYPE PrivilegeType, BOOL *pfResult)
{
	TRACE_FX();
	if (!pfResult)
		return ERROR_INVALID_PARAMETER;
	*pfResult = FALSE;
	if (PrivilegeType != XPRIVILEGE_MULTIPLAYER_SESSIONS &&
		PrivilegeType != XPRIVILEGE_COMMUNICATIONS &&
		PrivilegeType != XPRIVILEGE_COMMUNICATIONS_FRIENDS_ONLY &&
		PrivilegeType != XPRIVILEGE_PROFILE_VIEWING &&
		PrivilegeType != XPRIVILEGE_PROFILE_VIEWING_FRIENDS_ONLY &&
		PrivilegeType != XPRIVILEGE_USER_CREATED_CONTENT &&
		PrivilegeType != XPRIVILEGE_USER_CREATED_CONTENT_FRIENDS_ONLY &&
		PrivilegeType != XPRIVILEGE_PURCHASE_CONTENT &&
		PrivilegeType != XPRIVILEGE_PRESENCE &&
		PrivilegeType != XPRIVILEGE_PRESENCE_FRIENDS_ONLY &&
		PrivilegeType != XPRIVILEGE_TRADE_CONTENT &&
		PrivilegeType != XPRIVILEGE_VIDEO_COMMUNICATIONS &&
		PrivilegeType != XPRIVILEGE_VIDEO_COMMUNICATIONS_FRIENDS_ONLY &&
		PrivilegeType != XPRIVILEGE_MULTIPLAYER_DEDICATED_SERVER)
		return ERROR_INVALID_PARAMETER;
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;

	if (TRUE)//TODO XUserCheckPrivilege
		*pfResult = TRUE;

	return ERROR_SUCCESS;
}

// #5267
DWORD WINAPI XUserGetSigninInfo(DWORD dwUserIndex, DWORD dwFlags, XUSER_SIGNIN_INFO *pSigninInfo)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;//ERROR_NO_SUCH_USER
	if (!pSigninInfo)
		return ERROR_INVALID_PARAMETER;
	if (dwFlags & ~(XUSER_GET_SIGNIN_INFO_ONLINE_XUID_ONLY | XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY | 0b100) || ((dwFlags & XUSER_GET_SIGNIN_INFO_ONLINE_XUID_ONLY) && (dwFlags & XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY)))
		return ERROR_INVALID_PARAMETER;

	memcpy_s(pSigninInfo, sizeof(XUSER_SIGNIN_INFO), xlive_users_info[dwUserIndex], sizeof(XUSER_SIGNIN_INFO));

	if (dwFlags & XUSER_GET_SIGNIN_INFO_ONLINE_XUID_ONLY && xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive) {
		pSigninInfo->xuid = INVALID_XUID;
	}

	return ERROR_SUCCESS;
}

// #5273
VOID XUserReadGamerPictureByKey()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5274
VOID XUserAwardGamerPicture()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5276
VOID WINAPI XUserSetProperty(DWORD dwUserIndex, DWORD dwPropertyId, DWORD cbValue, CONST VOID *pvValue)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return;// ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return;// ERROR_NOT_LOGGED_ON;
	if (!cbValue)
		return;// ERROR_INVALID_PARAMETER;
	if (!pvValue)
		return;// ERROR_INVALID_PARAMETER;
	if (!XLivepIsPropertyIdValid(dwPropertyId, TRUE))
		return;

	//TODO XUserSetProperty
}

// #5277
DWORD WINAPI XUserSetContext(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		addDebugText("- ERROR: No such user!");
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		addDebugText("- ERROR: User not logged on!");
		return ERROR_NOT_LOGGED_ON;

	if (dwContextId == X_CONTEXT_PRESENCE) {

	}
	else if (dwContextId == X_CONTEXT_GAME_TYPE) {
		xlive_session_details.dwGameType = dwContextValue;
	}
	else if (dwContextId == X_CONTEXT_GAME_MODE) {
		xlive_session_details.dwGameMode = dwContextValue;
	}
	else if (dwContextId == X_CONTEXT_SESSION_JOINABLE) {
		addDebugText("- Session joinable!");
	}

	return ERROR_SUCCESS;
}

// #5278
DWORD WINAPI XUserWriteAchievements(DWORD dwNumAchievements, CONST XUSER_ACHIEVEMENT *pAchievements, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (!dwNumAchievements)
		return ERROR_INVALID_PARAMETER;
	if (!pAchievements)
		return ERROR_INVALID_PARAMETER;
	if (pAchievements->dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;

	//TODO XUserWriteAchievements
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

// #5279
VOID XUserReadAchievementPicture()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5280
DWORD WINAPI XUserCreateAchievementEnumerator(DWORD dwTitleId, DWORD dwUserIndex, XUID xuid, DWORD dwDetailFlags, DWORD dwStartingIndex, DWORD cItem, DWORD *pcbBuffer, HANDLE *phEnum)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (cItem > XACHIEVEMENT_MAX_COUNT)
		return ERROR_INVALID_PARAMETER;
	if (dwStartingIndex >= XACHIEVEMENT_MAX_COUNT)
		return ERROR_INVALID_PARAMETER;
	if (dwStartingIndex + cItem < dwStartingIndex || dwStartingIndex + cItem >= XACHIEVEMENT_MAX_COUNT)
		return ERROR_INVALID_PARAMETER;
	if (!dwDetailFlags || (dwDetailFlags != XACHIEVEMENT_DETAILS_ALL && dwDetailFlags & ~(XACHIEVEMENT_DETAILS_LABEL | XACHIEVEMENT_DETAILS_DESCRIPTION | XACHIEVEMENT_DETAILS_UNACHIEVED | XACHIEVEMENT_DETAILS_TFC)))
		return ERROR_INVALID_PARAMETER;
	if (!pcbBuffer)
		return ERROR_INVALID_PARAMETER;
	if (!phEnum)
		return ERROR_INVALID_PARAMETER;

	if (xuid == INVALID_XUID) {
		//enumerate the local signed-in gamer's achievements.
	}

	for (DWORD i = dwStartingIndex; i < dwStartingIndex + cItem; i++) {
		//?
	}
	//TODO XUserCreateAchievementEnumerator

	*pcbBuffer = cItem * sizeof(XACHIEVEMENT_DETAILS);
	*phEnum = CreateMutex(NULL, NULL, NULL);

	return ERROR_SUCCESS;
}

// #5281
DWORD WINAPI XUserReadStats(DWORD dwTitleId, DWORD dwNumXuids, CONST XUID *pXuids, DWORD dwNumStatsSpecs, CONST XUSER_STATS_SPEC *pSpecs, DWORD *pcbResults, XUSER_STATS_READ_RESULTS *pResults, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (!dwNumXuids || dwNumXuids > 0x65)
		return ERROR_INVALID_PARAMETER;
	if (!pXuids)
		return ERROR_INVALID_PARAMETER;
	if (!dwNumStatsSpecs || dwNumStatsSpecs > 0x40)
		return ERROR_INVALID_PARAMETER;
	if (!pSpecs)
		return ERROR_INVALID_PARAMETER;
	if (!pcbResults)
		return ERROR_INVALID_PARAMETER;
	if (*pcbResults && !pResults)
		return ERROR_INVALID_PARAMETER;
	if (!*pcbResults && pResults)
		return ERROR_INVALID_PARAMETER;

	if (!pResults) {
		*pcbResults = sizeof(XUSER_STATS_READ_RESULTS);
		return ERROR_INSUFFICIENT_BUFFER;
	}

	pResults->dwNumViews = 0;
	pResults->pViews = 0;

	return ERROR_NOT_FOUND;

	DWORD *v9 = pcbResults;
	DWORD v10 = *pcbResults;
	DWORD v11 = dwNumStatsSpecs * (52 * dwNumXuids + 16) + 8;
	if (dwNumStatsSpecs)
	{
		DWORD *v12 = (DWORD*)((char*)pSpecs + 4);
		do
		{
			v11 += 28 * dwNumXuids * *v12;
			v12 += 34;
			--dwNumStatsSpecs;
		} while (dwNumStatsSpecs);
		v9 = pcbResults;
	}
	if (v11 > v10)
	{
		*v9 = v11;
		return ERROR_INSUFFICIENT_BUFFER;
	}

	//TODO XUserReadStats
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

// #5282
VOID XUserReadGamerPicture()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5284
DWORD WINAPI XUserCreateStatsEnumeratorByRank(DWORD dwTitleId, DWORD dwRankStart, DWORD dwNumRows, DWORD dwNumStatsSpecs, const XUSER_STATS_SPEC *pSpecs, DWORD *pcbBuffer, PHANDLE ph)
{
	TRACE_FX();
	if (!dwRankStart)
		return ERROR_INVALID_PARAMETER;
	if (!dwNumRows || dwNumRows > 0x64)
		return ERROR_INVALID_PARAMETER;
	if (!dwNumStatsSpecs || dwNumStatsSpecs > 0x40)
		return ERROR_INVALID_PARAMETER;
	if (!pSpecs)
		return ERROR_INVALID_PARAMETER;
	if (!pcbBuffer)
		return ERROR_INVALID_PARAMETER;
	if (!ph)
		return ERROR_INVALID_PARAMETER;

	DWORD v9 = dwNumStatsSpecs;
	DWORD v12 = v9 * (48 * dwNumRows + 16) + 8;
	if (v9)
	{
		DWORD *v13 = (DWORD*)((char *)pSpecs + 4);
		do
		{
			v12 += 28 * dwNumRows * *v13;
			v13 += 34;
			--v9;
		} while (v9);
	}
	*pcbBuffer = v12;
	*ph = CreateMutex(NULL, NULL, NULL);

	return ERROR_SUCCESS;
}

// #5285
VOID XUserCreateStatsEnumeratorByRating()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5286
DWORD WINAPI XUserCreateStatsEnumeratorByXuid(DWORD dwTitleId, XUID XuidPivot, DWORD dwNumRows, DWORD dwNumStatsSpecs, const XUSER_STATS_SPEC *pSpecs, DWORD *pcbBuffer, HANDLE *ph)
{
	TRACE_FX();
	if (!XuidPivot)
		return ERROR_INVALID_PARAMETER;
	if (!dwNumRows || dwNumRows > 0x64)
		return ERROR_INVALID_PARAMETER;
	if (!dwNumStatsSpecs || dwNumStatsSpecs > 0x40)
		return ERROR_INVALID_PARAMETER;
	if (!pSpecs)
		return ERROR_INVALID_PARAMETER;
	if (!pcbBuffer)
		return ERROR_INVALID_PARAMETER;
	if (!ph)
		return ERROR_INVALID_PARAMETER;

	DWORD v9 = dwNumStatsSpecs;
	DWORD v12 = v9 * (48 * dwNumRows + 16) + 8;
	if (v9)
	{
		DWORD *v13 = (DWORD*)((char *)pSpecs + 4);
		do
		{
			v12 += 28 * dwNumRows * *v13;
			v13 += 34;
			--v9;
		} while (v9);
	}
	*pcbBuffer = v12;
	*ph = CreateMutex(NULL, NULL, NULL);

	return ERROR_SUCCESS;
}

// #5287
VOID XUserResetStatsView()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5288
VOID XUserGetProperty()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5289
VOID XUserGetContext()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5290
VOID XUserGetReputationStars()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5291
VOID XUserResetStatsViewAllUsers()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5292
DWORD WINAPI XUserSetContextEx(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (dwContextId != X_CONTEXT_PRESENCE || dwContextId != X_CONTEXT_GAME_TYPE || dwContextId != X_CONTEXT_GAME_MODE || dwContextId != X_CONTEXT_SESSION_JOINABLE)
		return ERROR_INVALID_PARAMETER;

	if (dwContextId == X_CONTEXT_GAME_TYPE) {
		xlive_session_details.dwGameType = dwContextValue;
	}
	else if (dwContextId == X_CONTEXT_GAME_MODE) {
		xlive_session_details.dwGameMode = dwContextValue;
	}

	//TODO XUserSetContextEx
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

// #5293
DWORD WINAPI XUserSetPropertyEx(DWORD dwUserIndex, DWORD dwPropertyId, DWORD cbValue, CONST VOID *pvValue, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (cbValue == 0)
		return ERROR_INVALID_PARAMETER;
	if (!pvValue)
		return ERROR_INVALID_PARAMETER;
	if (!XLivepIsPropertyIdValid(dwPropertyId, TRUE))
		return ERROR_INVALID_PARAMETER;

	//TODO XUserSetPropertyEx
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

bool IsValidSettingId(DWORD dwTitleId, DWORD dwSettingId)
{
	if (((SETTING_ID*)&dwSettingId)->id < 0x50 || (dwSettingId == XPROFILE_TITLE_SPECIFIC1 || dwSettingId == XPROFILE_TITLE_SPECIFIC2 || dwSettingId == XPROFILE_TITLE_SPECIFIC3) && dwTitleId != DASHBOARD_TITLE_ID) {
		return true;
	}
	return false;
}

DWORD ValidateSettings(DWORD dwTitleId, DWORD dwNumSettings, const XUSER_PROFILE_SETTING *pSettings)
{
	DWORD v3 = 0;
	if (dwNumSettings == 0)
		return ERROR_SUCCESS;
	while (IsValidSettingId(dwTitleId, pSettings[v3++].dwSettingId)) {
		if (v3 >= dwNumSettings) {
			return ERROR_SUCCESS;
		}
	}
	return ERROR_INVALID_PARAMETER;
}

DWORD ValidateSettingIds(DWORD dwTitleId, DWORD dwNumSettingIds, const DWORD *pdwSettingIds)
{
	DWORD v3 = 0;
	if (dwNumSettingIds == 0)
		return ERROR_SUCCESS;
	while (IsValidSettingId(dwTitleId, pdwSettingIds[v3++])) {
		if (v3 >= dwNumSettingIds) {
			return ERROR_SUCCESS;
		}
	}
	return ERROR_INVALID_PARAMETER;
}

DWORD GetXUserProfileSettingsBufferLength(
	DWORD dwTitleId,
	DWORD dwNumFor,
	DWORD dwNumSettingIds,
	const DWORD *pdwSettingIds,
	DWORD *pcbResults,
	XUSER_READ_PROFILE_SETTING_RESULT *pResults)
{
	TRACE_FX();
	if (dwNumSettingIds == 0 || dwNumSettingIds > 32)
		return ERROR_INVALID_PARAMETER;
	if (!pdwSettingIds)
		return ERROR_INVALID_PARAMETER;
	if (!pcbResults)
		return ERROR_INVALID_PARAMETER;
	if (*pcbResults && !pResults)
		return ERROR_INVALID_PARAMETER;

	DWORD result = ValidateSettingIds(dwTitleId, dwNumSettingIds, pdwSettingIds);
	if (result) {
		return result;
	}

	DWORD setting_buffer_required = 0;
	for (DWORD i = 0; i < dwNumSettingIds; i++) {
		DWORD &settingId = *(DWORD*)(&pdwSettingIds[i]);
		BYTE setting_data_type = ((SETTING_ID*)&settingId)->data_type;
		setting_buffer_required += sizeof(XUSER_PROFILE_SETTING);
		if (setting_data_type == XUSER_DATA_TYPE_UNICODE || setting_data_type == XUSER_DATA_TYPE_BINARY) {
			setting_buffer_required += ((SETTING_ID*)&settingId)->max_length;
		}
	}
	setting_buffer_required *= dwNumFor;
	setting_buffer_required += sizeof(XUSER_READ_PROFILE_SETTING_RESULT);

	if (!pResults || setting_buffer_required > *pcbResults) {
		*pcbResults = setting_buffer_required;
		return ERROR_INSUFFICIENT_BUFFER;
	}

	return ERROR_SUCCESS;
}

bool ReadXUserData(XUSER_DATA *xuser_data, BYTE *&extra_data_buffer_pos, DWORD extra_data_buffer_len, BYTE *buffer, DWORD buffer_len)
{
	BYTE *buffer_pos = buffer;

	xuser_data->type = *(BYTE*)buffer_pos;
	buffer_pos += sizeof(xuser_data->type);

	if (xuser_data->type == XUSER_DATA_TYPE_INT32) {
		xuser_data->nData = *(LONG*)buffer_pos;
		buffer_pos += sizeof(LONG);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_INT64) {
		xuser_data->i64Data = *(LONGLONG*)buffer_pos;
		buffer_pos += sizeof(LONGLONG);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_DOUBLE) {
		xuser_data->dblData = *(DOUBLE*)buffer_pos;
		buffer_pos += sizeof(DOUBLE);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_FLOAT) {
		xuser_data->fData = *(FLOAT*)buffer_pos;
		buffer_pos += sizeof(FLOAT);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_UNICODE) {
		xuser_data->string.cbData = *(DWORD*)buffer_pos;
		buffer_pos += sizeof(xuser_data->string.cbData);
		if (extra_data_buffer_len < xuser_data->string.cbData) {
			return false;
		}
		if (xuser_data->string.cbData % 2) {
			return false;
		}
		wcsncpy_s((wchar_t*)extra_data_buffer_pos, extra_data_buffer_len/2, (wchar_t*)buffer_pos, xuser_data->string.cbData/2);
		xuser_data->string.pwszData = (wchar_t*)extra_data_buffer_pos;
		extra_data_buffer_pos += xuser_data->string.cbData;
		buffer_pos += xuser_data->string.cbData;
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_BINARY) {
		xuser_data->binary.cbData = *(DWORD*)buffer_pos;
		buffer_pos += sizeof(xuser_data->binary.cbData);
		memcpy_s(extra_data_buffer_pos, extra_data_buffer_len, buffer_pos, xuser_data->binary.cbData);
		xuser_data->binary.pbData = extra_data_buffer_pos;
		extra_data_buffer_pos += xuser_data->binary.cbData;
		buffer_pos += xuser_data->binary.cbData;
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_DATETIME) {
		xuser_data->ftData = *(FILETIME*)buffer_pos;
		buffer_pos += sizeof(FILETIME);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_NULL) {
		return false;
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_CONTEXT) {
		return false;
	}
	else {
		return false;
	}

	return true;
}

DWORD ReadXUserProfileSettings(
	DWORD dwTitleId,
	DWORD dwUserIndex,
	DWORD dwNumSettingIds,
	const DWORD *pdwSettingIds,
	DWORD cbResults,
	XUSER_READ_PROFILE_SETTING_RESULT *pResults,
	BYTE *&pResultBufPos)
{
	TRACE_FX();
	if (dwNumSettingIds == 0 || dwNumSettingIds > 32)
		return ERROR_INVALID_PARAMETER;
	if (!pdwSettingIds)
		return ERROR_INVALID_PARAMETER;
	if (cbResults == 0)
		return ERROR_INVALID_PARAMETER;
	if (!pResults)
		return ERROR_INVALID_PARAMETER;
	if (!pResultBufPos)
		return ERROR_INVALID_PARAMETER;

	for (DWORD i = 0; i < dwNumSettingIds; i++) {
		DWORD &settingId = *(DWORD*)(&pdwSettingIds[i]);
		XUSER_PROFILE_SETTING *pSetting = &pResults->pSettings[pResults->dwSettingsLen];
		pResults->dwSettingsLen++;

		pSetting->dwSettingId = settingId;
		pSetting->source = XSOURCE_DEFAULT;
		memset(&pSetting->data, 0, sizeof(pSetting->data));
		if (dwUserIndex == XLIVE_LOCAL_USER_INVALID) {
			pSetting->user.dwUserIndex = 0;
			pSetting->user.xuid = INVALID_XUID;
			continue;
		}
		pSetting->user.dwUserIndex = dwUserIndex;
		pSetting->user.xuid = xlive_users_info[dwUserIndex]->xuid;

		DWORD save_file_buf_len_max = sizeof(XUSER_DATA);
		BYTE setting_data_type = ((SETTING_ID*)&settingId)->data_type;
		if (setting_data_type == XUSER_DATA_TYPE_UNICODE || setting_data_type == XUSER_DATA_TYPE_BINARY) {
			save_file_buf_len_max += ((SETTING_ID*)&settingId)->max_length;
		}

		int save_file_num = 0;
		if (settingId == XPROFILE_TITLE_SPECIFIC1) {
			save_file_num = 1;
		}
		else if (settingId == XPROFILE_TITLE_SPECIFIC2) {
			save_file_num = 2;
		}
		else if (settingId == XPROFILE_TITLE_SPECIFIC3) {
			save_file_num = 3;
		}

		wchar_t save_file_path[100];

		if (save_file_num == 0) {
			swprintf_s(save_file_path, L".\\profiles\\User_%hs_%08X.dat", xlive_users_info[dwUserIndex]->szUserName, settingId);
		}
		else {
			swprintf_s(save_file_path, L".\\profiles\\%08X\\User_%hs_%d.dat", xlive_title_id, xlive_users_info[dwUserIndex]->szUserName, save_file_num);
		}

		EnsureDirectoryExists(save_file_path);
		FILE *save_file;
		errno_t err_file = _wfopen_s(&save_file, save_file_path, L"rb");
		if (err_file) {
			addDebugText("Unable to read title save file:");
			addDebugText(save_file_path);
			continue;
		}

		fseek(save_file, 0L, SEEK_END);

		long fp_buflen = ftell(save_file);
		if (fp_buflen < 0L || (DWORD)fp_buflen > save_file_buf_len_max) {
			addDebugText("ERROR: Save file is larger than expected max value:");
			addDebugText(save_file_path);
			fclose(save_file);
			continue;
		}

		fseek(save_file, 0L, SEEK_SET);

		BYTE* fp_buffer = (BYTE*)malloc(sizeof(BYTE) * fp_buflen);
		if (!fp_buffer) {
			addDebugText("ERROR: Unable to allocate enough memory to load save file:");
			addDebugText(save_file_path);
			fclose(save_file);
			continue;
		}

		fread(fp_buffer, sizeof(BYTE), fp_buflen, save_file);
		fclose(save_file);

		if (ReadXUserData(&pSetting->data, pResultBufPos, cbResults - (pResultBufPos - (BYTE*)pResults), fp_buffer, fp_buflen)) {
			pSetting->source = XSOURCE_TITLE;
		}
		else {
			addDebugText("INVALID xuser_data save file (or it may have cascaded from an earlier one wrt. buf_pos total mem alloc space):");
			addDebugText(save_file_path);
		}

		free(fp_buffer);
	}

	return ERROR_SUCCESS;
}

bool WriteXUserData(BYTE *&buffer, DWORD &buffer_len, const XUSER_DATA *xuser_data)
{
	DWORD xuser_data_len = sizeof(xuser_data->type);

	if (xuser_data->type == XUSER_DATA_TYPE_INT32) {
		xuser_data_len += sizeof(LONG);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_INT64) {
		xuser_data_len += sizeof(LONGLONG);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_DOUBLE) {
		xuser_data_len += sizeof(DOUBLE);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_FLOAT) {
		xuser_data_len += sizeof(FLOAT);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_UNICODE) {
		xuser_data_len += sizeof(xuser_data->string.cbData);
		xuser_data_len += xuser_data->string.cbData;
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_BINARY) {
		xuser_data_len += sizeof(xuser_data->binary.cbData);
		xuser_data_len += xuser_data->binary.cbData;
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_DATETIME) {
		xuser_data_len += sizeof(FILETIME);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_NULL) {
		__debugbreak();
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_CONTEXT) {
		__debugbreak();
	}
	else {
		__debugbreak();
	}

	BYTE *xuser_data_buf = (BYTE*)malloc(sizeof(BYTE) * xuser_data_len);
	BYTE *xuser_data_buf_pos = xuser_data_buf;

	*(BYTE*)xuser_data_buf_pos = xuser_data->type;
	xuser_data_buf_pos += sizeof(xuser_data->type);

	if (xuser_data->type == XUSER_DATA_TYPE_INT32) {
		*(LONG*)xuser_data_buf_pos = xuser_data->nData;
		xuser_data_buf_pos += sizeof(LONG);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_INT64) {
		*(LONGLONG*)xuser_data_buf_pos = xuser_data->i64Data;
		xuser_data_buf_pos += sizeof(LONGLONG);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_DOUBLE) {
		*(DOUBLE*)xuser_data_buf_pos = xuser_data->dblData;
		xuser_data_buf_pos += sizeof(DOUBLE);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_FLOAT) {
		*(FLOAT*)xuser_data_buf_pos = xuser_data->fData;
		xuser_data_buf_pos += sizeof(FLOAT);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_UNICODE) {
		*(DWORD*)xuser_data_buf_pos = xuser_data->string.cbData;
		xuser_data_buf_pos += sizeof(xuser_data->string.cbData);
		wcsncpy_s((wchar_t*)xuser_data_buf_pos, xuser_data->string.cbData, xuser_data->string.pwszData, xuser_data->string.cbData);
		xuser_data_buf_pos += xuser_data->string.cbData;
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_BINARY) {
		*(DWORD*)xuser_data_buf_pos = xuser_data->binary.cbData;
		xuser_data_buf_pos += sizeof(xuser_data->binary.cbData);
		memcpy_s((BYTE*)xuser_data_buf_pos, xuser_data->binary.cbData, xuser_data->binary.pbData, xuser_data->binary.cbData);
		xuser_data_buf_pos += xuser_data->binary.cbData;
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_DATETIME) {
		*(FILETIME*)xuser_data_buf_pos = xuser_data->ftData;
		xuser_data_buf_pos += sizeof(FILETIME);
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_NULL) {
		__debugbreak();
	}
	else if (xuser_data->type == XUSER_DATA_TYPE_CONTEXT) {
		__debugbreak();
	}
	else {
		__debugbreak();
	}
	if (xuser_data_buf_pos - xuser_data_buf != xuser_data_len) {
		// Misaligned buffer!
		__debugbreak();
	}

	buffer = xuser_data_buf;
	buffer_len = xuser_data_len;
	return true;
}

// #5331
DWORD WINAPI XUserReadProfileSettings(
	DWORD dwTitleId,
	DWORD dwUserIndex,
	DWORD dwNumSettingIds,
	const DWORD *pdwSettingIds,
	DWORD *pcbResults,
	XUSER_READ_PROFILE_SETTING_RESULT *pResults,
	XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (dwNumSettingIds == 0 || dwNumSettingIds > 32)
		return ERROR_INVALID_PARAMETER;
	if (!pdwSettingIds)
		return ERROR_INVALID_PARAMETER;
	if (!pcbResults)
		return ERROR_INVALID_PARAMETER;
	if (*pcbResults && !pResults)
		return ERROR_INVALID_PARAMETER;

	DWORD dwNumFor = 1;

	DWORD result = GetXUserProfileSettingsBufferLength(dwTitleId, dwNumFor, dwNumSettingIds, pdwSettingIds, pcbResults, pResults);
	if (result) {
		return result;
	}

	BYTE *pResultBufPos = (BYTE*)pResults + sizeof(XUSER_READ_PROFILE_SETTING_RESULT);

	pResults->dwSettingsLen = 0;
	pResults->pSettings = (XUSER_PROFILE_SETTING*)pResultBufPos;
	pResultBufPos += sizeof(XUSER_PROFILE_SETTING) * dwNumSettingIds * dwNumFor;

	result = ReadXUserProfileSettings(dwTitleId, dwUserIndex, dwNumSettingIds, pdwSettingIds, *pcbResults, pResults, pResultBufPos);
	if (result) {
		return result;
	}

	if (pResults->dwSettingsLen == 0) {
		pResults->pSettings = 0;
		return ERROR_NOT_FOUND;
	}

	//TODO XUserReadProfileSettings
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

// #5337
DWORD WINAPI XUserWriteProfileSettings(DWORD dwUserIndex, DWORD dwNumSettings, const XUSER_PROFILE_SETTING *pSettings, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (dwNumSettings == 0)
		return ERROR_INVALID_PARAMETER;
	if (!pSettings)
		return ERROR_INVALID_PARAMETER;

	DWORD valid_settings[] = { XPROFILE_TITLE_SPECIFIC1, XPROFILE_TITLE_SPECIFIC2, XPROFILE_TITLE_SPECIFIC3 };
	if (dwNumSettings == 0xFFFFFFFF)
		return ERROR_INVALID_PARAMETER;
	for (DWORD i = 0; i < dwNumSettings; i++) {
		DWORD j = 0;
		while (valid_settings[j++] != pSettings[i].dwSettingId) {
			if (j >= sizeof(valid_settings) / sizeof(DWORD)) {
				return ERROR_INVALID_PARAMETER;
			}
		}
	}
	DWORD result = ValidateSettings(0, dwNumSettings, pSettings);
	if (result) {
		return result;
	}

	for (DWORD i = 0; i < dwNumSettings; i++) {

		int save_file_num = 0;
		if (pSettings[i].dwSettingId == XPROFILE_TITLE_SPECIFIC1) {
			save_file_num = 1;
		}
		else if (pSettings[i].dwSettingId == XPROFILE_TITLE_SPECIFIC2) {
			save_file_num = 2;
		}
		else if (pSettings[i].dwSettingId == XPROFILE_TITLE_SPECIFIC3) {
			save_file_num = 3;
		}

		wchar_t save_file_path[100];

		if (save_file_num == 0) {
			swprintf_s(save_file_path, L".\\profiles\\User_%hs_%08X.dat", xlive_users_info[dwUserIndex]->szUserName, pSettings[i].dwSettingId);
		}
		else {
			swprintf_s(save_file_path, L".\\profiles\\%08X\\User_%hs_%d.dat", xlive_title_id, xlive_users_info[dwUserIndex]->szUserName, save_file_num);
		}

		EnsureDirectoryExists(save_file_path);
		FILE *save_file;
		errno_t err_file = _wfopen_s(&save_file, save_file_path, L"wb");
		if (err_file) {
			addDebugText("Unable to write title save file:");
			addDebugText(save_file_path);
			continue;
		}

		BYTE *xuser_data;
		DWORD xuser_data_len;
		if (WriteXUserData(xuser_data, xuser_data_len, &pSettings[i].data)) {
			fwrite(xuser_data, sizeof(BYTE), xuser_data_len, save_file);
			free(xuser_data);
		}
		else {
			addDebugText("INVALID xuser_data.");
		}

		fclose(save_file);
	}

	//TODO XUserWriteProfileSettings
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

// #5339
DWORD WINAPI XUserReadProfileSettingsByXuid(
	DWORD dwTitleId,
	DWORD dwUserIndexRequester,
	DWORD dwNumFor,
	const XUID *pxuidFor,
	DWORD dwNumSettingIds,
	const DWORD *pdwSettingIds,
	DWORD *pcbResults,
	XUSER_READ_PROFILE_SETTING_RESULT *pResults,
	XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndexRequester >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndexRequester]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (dwNumSettingIds == 0 || dwNumSettingIds > 32)
		return ERROR_INVALID_PARAMETER;
	if (!pdwSettingIds)
		return ERROR_INVALID_PARAMETER;
	if (!pcbResults)
		return ERROR_INVALID_PARAMETER;
	if (*pcbResults && !pResults)
		return ERROR_INVALID_PARAMETER;

	if (dwNumFor == 0 || dwNumFor > 16)
		return ERROR_INVALID_PARAMETER;
	if (!pxuidFor)
		return ERROR_INVALID_PARAMETER;

	DWORD result = GetXUserProfileSettingsBufferLength(dwTitleId, dwNumFor, dwNumSettingIds, pdwSettingIds, pcbResults, pResults);
	if (result) {
		return result;
	}

	BYTE *pResultBufPos = (BYTE*)pResults + sizeof(XUSER_READ_PROFILE_SETTING_RESULT);

	pResults->dwSettingsLen = 0;
	pResults->pSettings = (XUSER_PROFILE_SETTING*)pResultBufPos;
	pResultBufPos += sizeof(XUSER_PROFILE_SETTING) * dwNumSettingIds * dwNumFor;

	for (DWORD i = 0; i < dwNumFor; i++) {
		DWORD userIndex = XLIVE_LOCAL_USER_INVALID;
		for (DWORD j = 0; j < XLIVE_LOCAL_USER_COUNT; j++) {
			if (xlive_users_info[j]->xuid == pxuidFor[i]) {
				userIndex = j;
			}
		}
		result = ReadXUserProfileSettings(dwTitleId, userIndex, dwNumSettingIds, pdwSettingIds, *pcbResults, pResults, pResultBufPos);
		if (result) {
			return result;
		}
	}

	if (pResults->dwSettingsLen == 0) {
		pResults->pSettings = 0;
		return ERROR_NOT_FOUND;
	}

	//TODO XUserReadProfileSettingsByXuid
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

// #5346
VOID XUserEstimateRankForRating()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5377
VOID XUserFindUsers()
{
	TRACE_FX();
	FUNC_STUB();
}
