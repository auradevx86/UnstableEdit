#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING      256
#define IDR_MENU            1000
#define ID_FILE_SHOWMSG     1001
#define ID_FILE_EXIT        1002
#define ID_EDIT_INSERT      2001
#define ID_EDIT_SELECTALL   2002
#define ID_STATUS           2000
#define ID_EDIT             3000
