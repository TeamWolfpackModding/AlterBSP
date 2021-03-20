#include "xdefs.h"
#include "xshow.h"
#include "../xlln/DebugText.h"
#include "xlive.h"
#include "../xlln/xlln.h"

// #5206
DWORD WINAPI XShowMessagesUI(DWORD dwUserIndex)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive)
		return ERROR_NOT_LOGGED_ON;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5208
DWORD WINAPI XShowGameInviteUI(DWORD dwUserIndex, const XUID *pXuidRecipients, DWORD cRecipients, LPCWSTR wszUnused)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive)
		return ERROR_NOT_LOGGED_ON;
	if (cRecipients && !pXuidRecipients)
		return ERROR_INVALID_PARAMETER;
	if (!cRecipients && pXuidRecipients)
		return ERROR_INVALID_PARAMETER;
	if (cRecipients > 0x64)
		return ERROR_INVALID_PARAMETER;
	if (wszUnused)
		return ERROR_INVALID_PARAMETER;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5209
DWORD WINAPI XShowMessageComposeUI(DWORD dwUserIndex, const XUID *pXuidRecipients, DWORD cRecipients, LPCWSTR wszText)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive)
		return ERROR_NOT_LOGGED_ON;
	if ((cRecipients && !pXuidRecipients) || (!cRecipients && pXuidRecipients))
		return ERROR_INVALID_PARAMETER;
	if (cRecipients > 0x64)
		return ERROR_INVALID_PARAMETER;
	if (wszText)
		return ERROR_INVALID_PARAMETER;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5210
DWORD WINAPI XShowFriendRequestUI(DWORD dwUserIndex, XUID xuidUser)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive)
		return ERROR_NOT_LOGGED_ON;
	if (!xuidUser)
		return ERROR_INVALID_PARAMETER;
	
	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5212
DWORD WINAPI XShowCustomPlayerListUI(
	DWORD dwUserIndex,
	DWORD dwFlags,
	LPCWSTR lpwszTitle,
	LPCWSTR lpwszDescription,
	const BYTE *pbImage,
	DWORD cbImage,
	const XPLAYERLIST_USER *rgPlayers,
	DWORD cPlayers,
	const XPLAYERLIST_BUTTON *pXButton,
	const XPLAYERLIST_BUTTON *pYButton,
	XPLAYERLIST_RESULT *pResult,
	XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (dwFlags & 0xFFFFFFFE)
		return ERROR_INVALID_PARAMETER;
	if (!lpwszTitle)
		return ERROR_INVALID_PARAMETER;
	if (!lpwszDescription)
		return ERROR_INVALID_PARAMETER;
	if ((cbImage && !pbImage) || (!cbImage && pbImage))
		return ERROR_INVALID_PARAMETER;
	if (cbImage > 0x9000)
		return ERROR_INVALID_PARAMETER;
	if ((cPlayers && !rgPlayers) || (!cPlayers && rgPlayers))
		return ERROR_INVALID_PARAMETER;
	if (cPlayers > 0x64)
		return ERROR_INVALID_PARAMETER;
	if (pXButton && (
		pXButton->dwType < 1 || pXButton->dwType > 6 || ((pXButton->dwType == 5 || pXButton->dwType == 6) && !pXButton->wszCustomText))) {
		return ERROR_INVALID_PARAMETER;
	}
	if (pYButton && (
		pYButton->dwType < 1 || pYButton->dwType > 6 || ((pYButton->dwType == 5 || pYButton->dwType == 6) && !pYButton->wszCustomText))) {
		return ERROR_INVALID_PARAMETER;
	}

	ShowXLLN(XLLN_SHOW_HOME);

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

// #5214
DWORD WINAPI XShowPlayerReviewUI(DWORD dwUserIndex, XUID XuidFeedbackTarget)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive)
		return ERROR_NOT_LOGGED_ON;
	if (!XuidFeedbackTarget)
		return ERROR_INVALID_PARAMETER;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5215
DWORD WINAPI XShowGuideUI(DWORD dwUserIndex)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5216
DWORD WINAPI XShowKeyboardUI(DWORD dwUserIndex, DWORD dwFlags, LPCWSTR wseDefaultText, LPCWSTR wszTitleText, LPCWSTR wszDescriptionText, LPWSTR wszResultText, DWORD cchResultText, XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;
	if (dwFlags && !(dwFlags & 0xE0000080))
		return ERROR_INVALID_PARAMETER;
	if (!wszResultText)
		return ERROR_INVALID_PARAMETER;
	if (!cchResultText)
		return ERROR_INVALID_PARAMETER;
	if (!pXOverlapped)
		return ERROR_INVALID_PARAMETER;

	//TODO XShowKeyboardUI dwFlags (like VKBD_DEFAULT)
	ShowXLLN(XLLN_SHOW_HOME);

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

// #5218
VOID XShowArcadeUI()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5250
DWORD WINAPI XShowAchievementsUI(DWORD dwUserIndex)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5252
DWORD WINAPI XShowGamerCardUI(DWORD dwUserIndex, XUID XuidPlayer)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState == eXUserSigninState_NotSignedIn)
		return ERROR_NOT_LOGGED_ON;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5260
DWORD WINAPI XShowSigninUI(DWORD cPanes, DWORD dwFlags)
{
	TRACE_FX();
	if (!(!(dwFlags & 0x40FFFC) && (!(dwFlags & XSSUI_FLAGS_LOCALSIGNINONLY) || !(dwFlags & XSSUI_FLAGS_SHOWONLYONLINEENABLED)))) {
		return ERROR_INVALID_PARAMETER;
	}

	// Number of users to sign in.
	if (!cPanes)
		return ERROR_INVALID_PARAMETER;

	ShowXLLN(XLLN_SHOW_LOGIN);

	//TODO XShowSigninUI
	return ERROR_SUCCESS;
	// no users signed in with multiplayer privilege. if XSSUI_FLAGS_SHOWONLYONLINEENABLED is flagged?
	return ERROR_FUNCTION_FAILED;
}

// #5266
DWORD WINAPI XShowMessageBoxUI(
	DWORD dwUserIndex,
	LPCWSTR wszTitle,
	LPCWSTR wszText,
	DWORD cButtons,
	LPCWSTR *pwszButtons,
	DWORD dwFocusButton,
	DWORD dwFlags,
	MESSAGEBOX_RESULT *pResult,
	XOVERLAPPED *pXOverlapped)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (dwFlags & 0xFFF0FFFC)
		return ERROR_INVALID_PARAMETER;
	if (!pResult)
		return ERROR_INVALID_PARAMETER;
	if (!pXOverlapped)
		return ERROR_INVALID_PARAMETER;
	if (!wszTitle)
		return ERROR_INVALID_PARAMETER;
	if (!wszText)
		return ERROR_INVALID_PARAMETER;
	if (dwFlags & 0x30000)//(XMB_VERIFYPASSCODEMODE | XMB_PASSCODEMODE)
		return ERROR_INVALID_PARAMETER;
	if (cButtons) {
		if (cButtons > 3 || dwFocusButton > cButtons - 1)
			return ERROR_INVALID_PARAMETER;
		if (!pwszButtons)
			return ERROR_INVALID_PARAMETER;
		for (DWORD i = 0; i >= cButtons; i++) {
			if (!pwszButtons[i]) {
				return ERROR_INVALID_PARAMETER;
			}
		}
	}
	
	pResult->dwButtonPressed = MessageBoxW(NULL, wszText, wszTitle, MB_OKCANCEL);

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
	return ERROR_CANCELLED;
}

// #5271
DWORD WINAPI XShowPlayersUI(DWORD dwUserIndex)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive)
		return ERROR_NOT_LOGGED_ON;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5275
DWORD WINAPI XShowFriendsUI(DWORD dwUserIndex)
{
	TRACE_FX();
	if (dwUserIndex >= XLIVE_LOCAL_USER_COUNT)
		return ERROR_NO_SUCH_USER;
	if (xlive_users_info[dwUserIndex]->UserSigninState != eXUserSigninState_SignedInToLive)
		return ERROR_NOT_LOGGED_ON;

	ShowXLLN(XLLN_SHOW_HOME);
	return ERROR_SUCCESS;
}

// #5299
VOID XShowGuideKeyRemapUI()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5365
VOID XShowMarketplaceUI()
{
	TRACE_FX();
	FUNC_STUB();
}

// #5366
VOID XShowMarketplaceDownloadItemsUI()
{
	TRACE_FX();
	FUNC_STUB();
}
