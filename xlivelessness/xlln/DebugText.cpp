#include "../xlive/xdefs.h"
#include "DebugText.h"
#include <string>
#include "../xlln/xlln.h"
#include "../xlive/xsocket.h"

static bool initialised_debug_log = false;
static CRITICAL_SECTION xlln_critsec_debug_log;

static bool xlln_log_out = true; // Controls whether a log is being outputted

// for the on screen debug log.
static char** DebugStr;
static int DebugTextArrayLenMax = 160;
static int DebugTextArrayPos = 0;
static bool DebugTextDisplay = false;
static FILE* debugFile = NULL;
static char debug_blarg[0x1000];

static char **blacklist;
static int blacklist_len = 0;
static int blacklist_len_max = 0;

bool addDebugTextBlacklist(char *black_text)
{
	
	if (!xlln_log_out) {
		return false;
	}

	EnterCriticalSection(&xlln_critsec_debug_log);

	if (blacklist_len >= blacklist_len_max) {
		blacklist_len_max += 50;
		blacklist = (char**)realloc(blacklist, sizeof(char*) * blacklist_len_max);
	}
	
	blacklist[blacklist_len++] = black_text;

	LeaveCriticalSection(&xlln_critsec_debug_log);
	return true;

}

int getDebugTextArrayMaxLen() {
	
	if (xlln_log_out) {
		return DebugTextArrayLenMax;
	}

}

void addDebugText(wchar_t* wtext) {
	
	if (xlln_log_out) {
		int lenInput = wcslen(wtext);
		char* text = (char*)malloc(sizeof(char) * lenInput + 1);
		snprintf(text, lenInput + 1, "%ls", wtext);
		addDebugText(text);
		free(text);
	}

}

void addDebugText(const char* text) {
	
	if (xlln_log_out) {
		int buflen = strlen(text) + 1;
		char* text2 = (char*)malloc(sizeof(char) * buflen);
		memcpy(text2, text, sizeof(char) * buflen);
		addDebugText(text2);
		free(text2);
	}

}

static void addDebugTextHelper(char* text) {
	
	if (xlln_log_out) {
		int lenInput = strlen(text);

		char* endChar = strchr(text, '\n');
		if (endChar) {
			lenInput = endChar - text;
		}

		DebugTextArrayPos++;
		if (DebugTextArrayPos >= DebugTextArrayLenMax) {
			DebugTextArrayPos = 0;
		}

		free(DebugStr[DebugTextArrayPos]);
		DebugStr[DebugTextArrayPos] = (char*)malloc(sizeof(char) * lenInput + 1);
		strncpy(DebugStr[DebugTextArrayPos], text, lenInput);
		memset(DebugStr[DebugTextArrayPos] + lenInput, 0, 1);

		if (debugFile != NULL) {
			char* debug_text = (char*)malloc(sizeof(char) * lenInput + 2);
			strncpy(debug_text, text, lenInput);
			memset(debug_text + lenInput, '\n', 1);
			memset(debug_text + lenInput + 1, 0, 1);
			fputs(debug_text, debugFile);
			fflush(debugFile);
			free(debug_text);
		}

		if (endChar) {
			addDebugText(endChar + 1);
		}
	}

}

void addDebugText(char* text) {
	
	if (xlln_log_out) {

		if (!initialised_debug_log)
			return;

		EnterCriticalSection(&xlln_critsec_debug_log);

		addDebugTextHelper(text);

		if (xlln_debug) {
			char* iblarg = debug_blarg;
			for (int i = 0; i < 30; i++) {
				iblarg += snprintf(iblarg, 0x1000, "%s\r\n", getDebugText(i));
			}
			// Putting SetDlgItemText(...) inside the critical section freezes the program for some incredibly stupid reason when you try to login via the XLLN window interface.
			LeaveCriticalSection(&xlln_critsec_debug_log);
			SetDlgItemText(xlln_window_hwnd, MYWINDOW_TBX_TEST, debug_blarg);
		}
		else {
			LeaveCriticalSection(&xlln_critsec_debug_log);
		}
		/*if (getDebugTextDisplay()) {
			for (int i = 0; i < getDebugTextArrayMaxLen(); i++) {
				const char* text = getDebugText(i);

			}
		}*/

	}

}

char* getDebugText(int ordered_index) {
	
	if (xlln_log_out) {

		if (initialised_debug_log) {
			if (ordered_index < DebugTextArrayLenMax) {
				int array_index = ((DebugTextArrayPos - ordered_index) + DebugTextArrayLenMax) % DebugTextArrayLenMax;
				char* result = DebugStr[array_index];
				return result;
			}
		}

		return const_cast<char*>("");

	}

}

void setDebugTextDisplay(bool setOn) {
	
	if (xlln_log_out) {

		DebugTextDisplay = setOn;

	}

}

bool getDebugTextDisplay() {
	
	if (xlln_log_out) {

		return DebugTextDisplay;

	}

}

//SYSTEMTIME t;
//GetLocalTime(&t);
//fwprintf(log_handle, L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

void trace_func(const char *fxname)
{
	
	if (xlln_log_out) {

		EnterCriticalSection(&xlln_critsec_debug_log);
		for (int i = 0; i < blacklist_len; i++) {
			if (strcmp(fxname, blacklist[i]) == 0) {
				LeaveCriticalSection(&xlln_critsec_debug_log);
				return;
			}
		}
		LeaveCriticalSection(&xlln_critsec_debug_log);
		int leng = (30 + strlen(fxname));
		char* guibsig = (char*)malloc(sizeof(char) * leng);
		snprintf(guibsig, leng, "%s()", fxname);
		addDebugText(guibsig);
		free(guibsig);

	}

}

VOID XllnDebugBreak(char* message)
{
	
	if (xlln_log_out) {

		addDebugText(message);
		MessageBox(NULL, message, "Illegal State", MB_OK);
		__debugbreak();

	}

}

VOID XllnDebugBreak(const char* message)
{
	
	if (xlln_log_out) {

		XllnDebugBreak((char*)message);

	}

}

void FUNC_STUB2(const char* func)
{
	
	if (xlln_log_out) {

		char errMsg[200];
		snprintf(errMsg, sizeof(errMsg), "Incomplete XLIVE Stubbed Function: %s", func);
		XllnDebugBreak(errMsg);

	}

}

INT InitDebugLog(DWORD dwInstanceId)
{
	
	if (xlln_log_out) {

		InitializeCriticalSection(&xlln_critsec_debug_log);

		blacklist_len_max = 50;
		blacklist = (char**)malloc(sizeof(char*) * blacklist_len_max);

		DebugStr = (char**)malloc(sizeof(char*) * DebugTextArrayLenMax);
		for (int i = 0; i < DebugTextArrayLenMax; i++) {
			DebugStr[i] = (char*)calloc(1, sizeof(char));
		}
		wchar_t debug_file_path[1024];
		swprintf(debug_file_path, 1024, L"xlive_debug.log");
		debugFile = _wfopen(debug_file_path, L"w");
		char debug_file_path_log[1034];
		sprintf(debug_file_path_log, "PATH: %ws", debug_file_path);
		initialised_debug_log = true;
		addDebugText(debug_file_path_log);
		addDebugText("Initialised Debug Logger.");

		return S_OK;

	}

}

INT UninitDebugLog()
{
	
	if (xlln_log_out) {

		initialised_debug_log = false;
		DeleteCriticalSection(&xlln_critsec_debug_log);

		return S_OK;

	}

}
