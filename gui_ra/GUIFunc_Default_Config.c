
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

// Make sure only One GUI is included, so reuse name
#ifndef __HAVE__RVNCD__GUI__
#define __HAVE__RVNCD__GUI__
#ifdef GUI_RA

// --
// Set Default Config

void GUIFunc_Default_Config( struct Config *cfg )
{
//	DebugPrintF( "RA: GUIFunc_Default_Config\n" );

	cfg->MainWinState = WINSTAT_Open;
	cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_Main].BusyWin = RAGUI_BusyMainWindow;
	cfg->cfg_WinData[WIN_Main].OpenWin = RAGUI_OpenMainWindow;
	cfg->cfg_WinData[WIN_Main].CloseWin = RAGUI_CloseMainWindow;
	cfg->cfg_WinData[WIN_Main].HandleWin = RAGUI_HandleMainWindow;
	cfg->cfg_WinData[WIN_Main].TimerTick = RAGUI_TimerTick_Main;
	
	cfg->cfg_WinData[WIN_About].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_About].BusyWin = RAGUI_BusyAboutWindow;
	cfg->cfg_WinData[WIN_About].OpenWin = RAGUI_OpenAboutWindow;
	cfg->cfg_WinData[WIN_About].CloseWin = RAGUI_CloseAboutWindow;
	cfg->cfg_WinData[WIN_About].HandleWin = RAGUI_HandleAboutWindow;

	cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Encodings].BusyWin = RAGUI_BusyEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].OpenWin = RAGUI_OpenEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].CloseWin = RAGUI_CloseEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].HandleWin = RAGUI_HandleEncodingsWindow;

	cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_KeyLogger].BusyWin = RAGUI_BusyKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].OpenWin = RAGUI_OpenKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].CloseWin = RAGUI_CloseKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].HandleWin = RAGUI_HandleKeyWindow;

	cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Session].BusyWin = RAGUI_BusySessionWindow;
	cfg->cfg_WinData[WIN_Session].OpenWin = RAGUI_OpenSessionWindow;
	cfg->cfg_WinData[WIN_Session].CloseWin = RAGUI_CloseSessionWindow;
	cfg->cfg_WinData[WIN_Session].HandleWin = RAGUI_HandleSessionWindow;
	cfg->cfg_WinData[WIN_Session].TimerTick = RAGUI_TimerTick_Session;

	cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_PixelFormat].BusyWin = RAGUI_BusyPixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].OpenWin = RAGUI_OpenPixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].CloseWin = RAGUI_ClosePixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].HandleWin = RAGUI_HandlePixelWindow;

	cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_IPEdit].BusyWin = RAGUI_BusyIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].OpenWin = RAGUI_OpenIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].CloseWin = RAGUI_CloseIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].HandleWin = RAGUI_HandleIPEditWindow;

	cfg->cfg_WinData[WIN_Quit].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Quit].BusyWin = RAGUI_BusyQuitWindow;
	cfg->cfg_WinData[WIN_Quit].OpenWin = RAGUI_OpenQuitWindow;
	cfg->cfg_WinData[WIN_Quit].CloseWin = RAGUI_CloseQuitWindow;
	cfg->cfg_WinData[WIN_Quit].HandleWin = RAGUI_HandleQuitWindow;
}

// --

#endif // GUI_RA
#endif // __HAVE__RVNCD__GUI__
