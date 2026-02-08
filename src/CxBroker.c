
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifdef HAVE_CXBROKER

// --

#include "rVNCd.h"

// --

static struct NewBroker BrokerData =
{
/* nb_Version	*/ NB_VERSION,
/* nb_Name		*/ (STR) "rVNCd",
/* nb_Title		*/ (STR) "rVNCd (c) 2023-2025 Rene W. Olsen",
/* nb_Descr		*/ (STR) "Amiga VNC Server",
/* nb_Unique	*/ NBU_NOTIFY | NBU_UNIQUE,
#ifdef __RVNCD_GUI_H__
/* nb_Flags		*/ COF_SHOW_HIDE,
#else
/* nb_Flags		*/ 0,
#endif // __RVNCD_GUI_H__
/* nb_Pri		*/ 0,
/* nb_Port		*/ 0,
/* nb_ResChannel*/ 0,
};

struct VNCMsgPort			CxBrokerMsgPort;
static CxObj *				CxBrokerAdr			= 0;
static long int				CxBrokerError		= 0;
//CxObj *					CxBrokerHotKey		= NULL;
struct Library *			CxBase				= NULL;
struct CommoditiesIFace *	ICommodities		= NULL;

// --

S32 CxBroker_Init( struct Config *cfg )
{
S32 retval;

	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "CxBroker_Init\n" );
	}

	// --

	CxBase = OpenLibrary( "commodities.library", 53 );
	ICommodities = (PTR) GetInterface( CxBase, "main", 1, NULL );

  	// ICommodities is not needed to run

	if ( ! ICommodities )
	{
		// We can run without the Broker
		Log_PrintF( cfg, LOGTYPE_Warning, "Error opening commodities library (v53), Disabling CxBroker" );
		cfg->cfg_Program_DisableCxBroker = TRUE;
		retval = TRUE;
		goto bailout;
	}
	else
	{
		retval = FALSE;
	}

	if ( cfg->cfg_Program_DisableGUI )
	{
		BrokerData.nb_Flags &= ~COF_SHOW_HIDE;
	}

	// --

	if ( ! MsgPort_Init( & CxBrokerMsgPort ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Failed to create CxBroker MsgPort" );
		goto bailout;
	}

	BrokerData.nb_Port = & CxBrokerMsgPort.vmp_MsgPort;

	CxBrokerAdr = CxBroker( & BrokerData, & CxBrokerError );

	if ( ! CxBrokerAdr )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Failed to create CxBroker" );
		goto bailout;
	}

//	  BrokerHotKey = AddHotKey( "control alt o", 1203 );

	ActivateCxObj( CxBrokerAdr, TRUE );

	retval = TRUE;

bailout:

	return(	retval );
}

// --

void CxBroker_Free( struct Config *cfg UNUSED )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "CxBroker_Free\n" );
	}

	// --

	if ( CxBrokerAdr )
	{
		DeleteCxObjAll( CxBrokerAdr );
		CxBrokerAdr = NULL;
	}

	MsgPort_Free( & CxBrokerMsgPort );

	if ( ICommodities )
	{
		DropInterface( (PTR) ICommodities );
		ICommodities = NULL;
	}

	if ( CxBase )
	{
		CloseLibrary( CxBase );
		CxBase = NULL;
	}
}

// --

void CxBroker_Handle( struct Config *cfg )
{
CxMsg *msg;

#ifdef __RVNCD_GUI_H__
S32 cnt;
#endif // __RVNCD_GUI_H__

	while(( msg = MsgPort_GetMsg( &  CxBrokerMsgPort )))
	{
		switch( CxMsgType( msg ))
		{
//			  case CXM_IEVENT:
//			  {
//				  switch( CxMsgID( msg ))
//				  {
//					  case 1203:
//					  {
//                        if ( MainWindow )
//                        {
//                            ScreenToFront( MainScreen );
//                            ActivateWindow( MainWindow );
//						  }
//                    	  else
//                        {
//							  MainUniconifyWindow();
//						  }
//                    	  break;
//					  }
//
//					  default:
//					  {
//						  break;
//					  }
//				  }
//				  break;
//			  }

			case CXM_COMMAND:
			{
				switch( CxMsgID( msg ))
				{
					case CXCMD_ENABLE:
					case CXCMD_DISABLE:
					{
						// Enable/Disable do not make sence for this driver
						break;
					}

					case CXCMD_APPEAR:
					case CXCMD_UNIQUE:
					{
						// If we get called here, some one if trying to setup a Broker in oure name
						// or the user hit ShowInterface in Exchange
						// --

						if ( DoVerbose > 2 )
						{
							SHELLBUF_PRINTF( "CxBroker_Init\n" );
						}

						// --

						#ifdef __RVNCD_GUI_H__
						if ( ! cfg->cfg_Program_DisableGUI )
						{
							// Check for Open windows

							for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
							{
								if ( cfg->cfg_WinData[cnt].Status == WINSTAT_Open )
								{
									break;
								}
							}

							// Always open one window, if all are closed

							if ( cnt == WIN_LAST )
							{
								cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
							}

							// Now open windows

							for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
							{
								if (( cfg->cfg_WinData[cnt].OpenWin )
								&&	( cfg->cfg_WinData[cnt].Status != WINSTAT_Close ))
								{
									cfg->cfg_WinData[cnt].OpenWin( cfg );
								}
							}
						}
						else
						{
							Log_PrintF( cfg, LOGTYPE_Warning, "Program: GUI Disabled can't open" );
						}
						#endif // __RVNCD_GUI_H__
						break;
					}

					case CXCMD_DISAPPEAR:
					{
						// User hit HideInterface in Exchange
						#ifdef __RVNCD_GUI_H__
						for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
						{
							if (( cfg->cfg_WinData[cnt].CloseWin )
							&&	( cfg->cfg_WinData[cnt].Status != WINSTAT_Close ))
							{
								cfg->cfg_WinData[cnt].CloseWin( cfg );
							}
						}
						#endif // __RVNCD_GUI_H__
						break;
					}

					case CXCMD_KILL:
					{
						Func_Quit( cfg );
						break;
					}

					default:
					{
						break;
					}
				}
				break;
			}

			default:
			{
				break;
			}
		}

		ReplyMsg( (PTR) msg );
	}
}

// --

#endif // HAVE_CXBROKER
