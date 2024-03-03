 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
 */

// --

#include "RVNCd.h"

// --

static int	myOpenSystem(	struct Config *cfg );
static int  myOpenSystem2(	struct Config *cfg );
static void myCloseSystem(	struct Config *cfg );
static void Handle_Application( struct Config *cfg );
static int  myOpen_CxBroker( struct Config *cfg );
static void myClose_CxBroker( struct Config *cfg );
static void myHandle_CxBroker( struct Config *cfg );
static void myHandle_Commands( struct Config *cfg );
static void myHandle_Windows( struct Config *cfg );
static void myHandle_Timer( struct Config *cfg );

// --

struct Library *			AppBase				= NULL;
struct ApplicationIFace * 	IApplication		= NULL;

struct Library *			AslBase				= NULL;
struct AslIFace * 			IAsl				= NULL;

struct Library *			CxBase				= NULL;
struct CommoditiesIFace *	ICommodities		= NULL;

struct Library *			DataTypesBase		= NULL;
struct DataTypesIFace *		IDataTypes			= NULL;

struct Library *			DiskfontBase		= NULL;
struct DiskfontIFace *		IDiskfont			= NULL;

struct GfxBase *			GfxBase				= NULL;
struct GraphicsIFace *  	IGraphics			= NULL;

struct IntuitionBase *		IntuitionBase		= NULL;
struct IntuitionIFace * 	IIntuition			= NULL;

struct Library *			KeymapBase			= NULL;
struct KeymapIFace *		IKeymap				= NULL;

struct Library *			P96Base				= NULL;
struct P96IFace *			IP96				= NULL;

struct ZLibBase *			ZBase				= NULL;
struct ZIFace *				IZ					= NULL;

#ifdef NEWLIB
// Newlib do not open Utility for us
struct UtilityBase *		UtilityBase			= NULL;
struct UtilityIFace *		IUtility			= NULL;
#endif

struct MsgPort *			TimerMsgPort		= NULL;
uint32						TimerMsgPortBit		= 0;
struct TimeRequest *		TimerIOReq			= NULL;
struct Device *				TimerBase			= NULL;
struct TimerIFace *			ITimer				= NULL;

// --

struct LayoutIFace * 		ILayout				= NULL;
struct ListBrowserIFace * 	IListBrowser		= NULL;

struct Library *			ButtonBase			= NULL;
struct Library *			BitmapBase			= NULL;
struct Library *			CheckBoxBase		= NULL;
struct Library *			ChooserBase			= NULL;
struct Library *			ClickTabBase		= NULL;
//struct ClassLibrary *		GetFileBase			= NULL;
struct Library *			IntegerBase			= NULL;
struct Library *			LabelBase			= NULL;
struct Library *			LayoutBase			= NULL;
struct Library *			ListBrowserBase		= NULL;
struct Library *			ScrollerBase		= NULL;
struct Library *			StringBase			= NULL;
struct Library *			WindowBase			= NULL;

// --

uint32						AppID				= 0;
static uint32				AppBit				= 0;
static struct MsgPort *		AppPort				= NULL;

struct Task *				ProgramTask			= NULL;
int							ProgramRunning		= FALSE;
int							ProgramInfo			= FALSE;

static struct MsgPort *		CxBrokerMsgPort		= NULL;
static uint32				CxBrokerMsgPortBit	= 0;
static CxObj *				CxBrokerAdr			= 0;
static int32				CxBrokerError		= 0;
//static CxObj *				CxBrokerHotKey		= NULL;

struct MsgPort *			CmdMsgPort			= NULL;
uint32						CmdMsgPortBit		= 0;

struct Hook					WinAppHook;
struct MsgPort *			WinAppPort			= NULL;
uint32						WinAppPortBit		= 0;

struct MsgPort *			WinMsgPort			= NULL;
uint32						WinMsgPortBit		= 0;

struct List 				LogStringList;
static int					LogStringCount		= 0;
struct List 				KeyLoggerList;
static int					KeyLoggerCount		= 0;
struct CommandEncoding *	ActiveEncoding		= NULL;

Class *						ButtonClass			= NULL;
Class *						BitMapClass			= NULL;
Class *						CheckBoxClass		= NULL;
Class *						ChooserClass		= NULL;
Class *						ClickTabClass		= NULL;
Class *						IntegerClass		= NULL;
Class *						LabelClass			= NULL;
Class *						LayoutClass			= NULL;
Class *						ListBrowserClass	= NULL;
Class *						PageClass			= NULL;
Class *						StringClass			= NULL;
Class *						ScrollerClass		= NULL;
Class *						WindowClass			= NULL;

char	ActionBuffer_ProgramStart[MAX_ACTIONBUFFER];
char	ActionBuffer_ProgramStop[MAX_ACTIONBUFFER];
char	ActionBuffer_ServerStart[MAX_ACTIONBUFFER];
char	ActionBuffer_ServerStop[MAX_ACTIONBUFFER];
char	ActionBuffer_UserConnect[MAX_ACTIONBUFFER];
char	ActionBuffer_UserDisconnect[MAX_ACTIONBUFFER];

struct SignalSemaphore		TestSema;
struct SignalSemaphore		ActionSema;


static const USED char *	MyVersion			= VERSTAG;

static struct NewBroker BrokerData =
{
/* nb_Version	*/ NB_VERSION,
/* nb_Name		*/ (STRPTR) "RVNCd",
/* nb_Title		*/ (STRPTR) "RVNCd (c) 2023-2024 Rene W. Olsen",
/* nb_Descr		*/ (STRPTR) "VNC Server",
/* nb_Unique	*/ NBU_NOTIFY | NBU_UNIQUE,
/* nb_Flags		*/ COF_SHOW_HIDE,
/* nb_Pri		*/ 0,
/* nb_Port		*/ 0,
/* nb_ResChannel*/ 0,
};

// --

#ifdef VAL_TEST
int TEST_VAL = 0;
#endif

int main( int argc, char **argv )
{
struct Config *cfg;
struct Task *task;
char *prgname;
uint32 mask;
BPTR olddir;
BPTR newdir;
int cnt;

	// -- Program Duplication Check

	IExec->Forbid();

	ProgramTask = IExec->FindTask( NULL );
	prgname = ProgramTask->tc_Node.ln_Name;
	ProgramTask->tc_Node.ln_Name = "<NULL>";

	task = IExec->FindTask( "RVNCd" );

	if ( task )
	{
		ProgramTask->tc_Node.ln_Name = prgname;

		IExec->Signal( task, SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E );
		IExec->Permit();
		return( 0 );
	}

	ProgramTask->tc_Node.ln_Name = "RVNCd";

	IExec->Permit();

	// --

	// Disable CTRL+C in c lib
	signal( SIGINT, SIG_IGN );

	// Init Random
	srand( time( NULL ));

	// Set Default Dir to progdir:
	newdir = IDOS->Lock( "progdir:", ACCESS_READ );
	olddir = IDOS->SetCurrentDir( newdir );

	IExec->NewList( & KeyLoggerList );
	IExec->NewList( & LogStringList );

	IExec->InitSemaphore( & ActionSema );
	IExec->InitSemaphore( & TestSema );

	// --

	for( int cnt=1 ; cnt<argc ; cnt++ )
	{
		if ( ! strcmp( argv[cnt], "--mugwall" ))
		{
			IExec->DebugPrintF( "Doing MugWall\n" );
			DoMugWall = TRUE;
			continue;
		}

		#ifdef VAL_TEST
		if ( ! strcmp( argv[cnt], "--mousetest" ))
		{
			TEST_VAL = TRUE;
			continue;
		}
		#endif
	}

	// --

	cfg = Config_Create();

	if ( cfg == NULL )
	{
		printf( "Error creating Config struct\n" );
		goto bailout;
	}

	if ( Config_Parse( cfg, argc, argv ))
	{
		printf( "Error parsing Config file\n" );
		goto bailout;
	}

	if ( myOpenSystem( cfg ))
	{
		printf( "Error opening System 1\n" );
		goto bailout;
	}

	if ( cfg->cfg_LogProgramStart )
	{
		Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Program : v%ld.%ld : Starting up .. ", VERSION, REVISION );
	}

	if ( myOpenSystem2( cfg ))
	{
		printf( "Error opening System 2\n" );
		goto bailout;
	}

	if ( ! cfg->cfg_ProgramDisableARexx )
	{
		if ( ARexx_Setup( cfg ))
		{
			printf( "Error starting ARexx\n" );
			goto bailout;
		}
	}

	if ( ! cfg->cfg_ProgramDisableCxBroker )
	{
		if ( myOpen_CxBroker( cfg ))
		{
			printf( "Error starting CxBroker\n" );
			goto bailout;
		}
	}

	if ( cfg->cfg_ActionsProgramStartEnable )
	{
		DoAction_ProgramStart( cfg );
	}

	if ( ! cfg->cfg_ProgramDisableGUI )
	{
		for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
		{
			if ( cfg->cfg_WinData[cnt].Status != WINSTAT_Close )
			{
				cfg->cfg_WinData[cnt].OpenWin( cfg );
			}
		}
	}

	// -- --

	if ( cfg->cfg_Disk_Settings2.AutoStart )
	{
		StartServer( cfg );
	}

	// -- --

	TimerIOReq->Request.io_Command = TR_ADDREQUEST;
	TimerIOReq->Time.Seconds = 1;
	TimerIOReq->Time.Microseconds = 0;
	IExec->SendIO( (APTR) TimerIOReq );

	ProgramRunning = TRUE;

	while( ProgramRunning )
	{
		mask = IExec->Wait( 
			SIGBREAKF_CTRL_C | 
			SIGBREAKF_CTRL_D | 
			SIGBREAKF_CTRL_E |
			AppBit | 
			ARexxBit | 
			CmdMsgPortBit |
			WinMsgPortBit |
			WinAppPortBit |
			TimerMsgPortBit |
			CxBrokerMsgPortBit
		);

		if ( mask & SIGBREAKF_CTRL_C )
		{
			if ( cfg->UserCount )
			{
				if ( IDOS->TimedDosRequesterTags(
					TDR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
					TDR_ImageType, TDRIMAGE_QUESTION,
					TDR_TitleString, "RVNCd",
					TDR_FormatString, "There are %ld user(s) connected\nAre you sure you want to Quit?",
					TDR_GadgetString, "_No|_Yes",
					TDR_Arg1, cfg->UserCount,
					TAG_END ) == 0 )
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if ( mask & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E ))
		{
			// Someone tryed to start us again.
			cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
			cfg->cfg_WinData[WIN_Main].OpenWin( cfg );
		}

		if ( mask & AppBit )
		{
			Handle_Application( cfg );
		}

		if ( mask & ARexxBit )
		{
			ARexx_Handle( cfg );
		}

		if ( mask & CmdMsgPortBit )
		{
			myHandle_Commands( cfg );
		}

		if (( mask & WinAppPortBit )
		||	( mask & WinMsgPortBit ))
		{
			myHandle_Windows( cfg );
		}

		if ( mask & CxBrokerMsgPortBit )
		{
			myHandle_CxBroker( cfg );
		}

		if ( mask & TimerMsgPortBit )
		{
			myHandle_Timer( cfg );
		}
	}

	if ( ! IExec->CheckIO( (APTR) TimerIOReq ))
	{
		IExec->AbortIO( (APTR) TimerIOReq );
	}
	IExec->WaitIO( (APTR) TimerIOReq );

	// -- --

	StopServer( cfg );

	// -- --

bailout:

	if ( cfg->cfg_ActionsProgramStopEnable )
	{
		DoAction_ProgramStop( cfg );
	}

	if ( cfg->cfg_LogProgramStop )
	{
		Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Program : Shutting down .." );
	}

//	printf( "WhiteListed: %d, Total Accecpted: %d\n", cfg->Connect_WhiteListed, cfg->Connect_Accecpted );
//	printf( "BlackListed: %d, Total Rejected: %d\n", cfg->Connect_BlackListed, cfg->Connect_Rejected );

	for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
	{
		if ( cfg->cfg_WinData[cnt].Status != WINSTAT_Close )
		{
			cfg->cfg_WinData[cnt].CloseWin( cfg );
		}
	}

	myClose_CxBroker( cfg );

	ARexx_Cleanup( cfg );

	myCloseSystem( cfg );

	if ( cfg )
	{
		Config_Delete( cfg );
	}

	// --

	IDOS->SetCurrentDir( olddir );
	IDOS->UnLock( newdir );

	ProgramTask->tc_Node.ln_Name = prgname;

	return( 0 );
}

// --

void PrintProgramHeader( void )
{
	if ( ! ProgramInfo )
	{
		ProgramInfo = TRUE;
		printf( "\n" );
		printf( "  %s (%s)\n", VERS, DATE );
		printf( "    by Rene W. Olsen\n" );
		printf( "\n" );
	}
}

// --

int StopServer( struct Config *cfg )
{
int error;

	error = FALSE;

	myStop_Server( cfg );

	return( error );
}

// --

int StartServer( struct Config *cfg )
{
int error;

	error = myStart_Server( cfg );

	return( error );
}

// --

int RestartServer( struct Config *cfg )
{
int error;

	myStop_Server( cfg );

	error = myStart_Server( cfg );

	return( error );
}

// --

static int myOpenSystem( struct Config *cfg )
{
int error;

	error = TRUE;

	// --

	CmdMsgPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( CmdMsgPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Command MsgPort" );
		goto bailout;
	}

	CmdMsgPortBit = 1UL << CmdMsgPort->mp_SigBit;

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static int myOpenSystem2( struct Config *cfg )
{
int error;

	error = TRUE;

	AppBase = IExec->OpenLibrary( "application.library", 53 );
	IApplication = (APTR) IExec->GetInterface( AppBase, "application", 2, NULL );

	if ( IApplication == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening application library (v53)" );
		goto bailout;
	}

	AslBase = IExec->OpenLibrary( "asl.library", 53 );
	IAsl = (APTR) IExec->GetInterface( AslBase, "main", 1, NULL );

	if ( IAsl == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening asl library (v53)" );
		goto bailout;
	}

	CxBase = IExec->OpenLibrary( "commodities.library", 53 );
	ICommodities = (APTR) IExec->GetInterface( CxBase, "main", 1, NULL );

  	// ICommodities is not needed to run

	if ( ICommodities == NULL )
	{
		cfg->cfg_ProgramDisableCxBroker = TRUE;
		Log_PrintF( cfg, LOGTYPE_Warning, "Error opening commodities library (v53), Disabling CxBroker" );
	}

	DataTypesBase = IExec->OpenLibrary( "datatypes.library", 53 );
	IDataTypes = (APTR) IExec->GetInterface( DataTypesBase, "main", 1, NULL );

	if ( IDataTypes == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening datatypes library (v53)" );
		goto bailout;
	}

	DiskfontBase = IExec->OpenLibrary( "diskfont.library", 53 );
	IDiskfont = (APTR) IExec->GetInterface( DiskfontBase, "main", 1, NULL );

	if ( IDiskfont == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening diskfont library (v53)" );
		goto bailout;
	}

	GfxBase = (APTR) IExec->OpenLibrary( "graphics.library", 53 );
	IGraphics = (APTR) IExec->GetInterface( (APTR) GfxBase, "main", 1, NULL );

	if ( IGraphics == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening graphics library (v53)" );
		goto bailout;
	}

	IntuitionBase = (APTR) IExec->OpenLibrary( "intuition.library", 53 );
	IIntuition = (APTR) IExec->GetInterface( (APTR) IntuitionBase, "main", 1, NULL );

	if ( IIntuition == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening intuition library (v53)" );
		goto bailout;
	}

	KeymapBase = IExec->OpenLibrary( "keymap.library", 53 );
	IKeymap = (APTR) IExec->GetInterface( KeymapBase, "main", 1, NULL );

	if ( IKeymap == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining keymap library (v53)" );
		goto bailout;
	}

	P96Base = (APTR) IExec->OpenLibrary( "Picasso96API.library", 53 );
	IP96 = (APTR) IExec->GetInterface( (APTR) P96Base, "main", 1, NULL );

	if ( IP96 == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining Picasso96 library (v53)" );
		goto bailout;
	}

	UtilityBase = (APTR) IExec->OpenLibrary( "utility.library", 53 );
	IUtility = (APTR) IExec->GetInterface( (APTR) UtilityBase, "main", 1, NULL );

	if ( IUtility == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining utility library (v53)" );
		goto bailout;
	}

	ZBase = (APTR) IExec->OpenLibrary( "z.library", 53 );
	IZ = (APTR) IExec->GetInterface( (APTR) ZBase, "main", 1, NULL );

	if ( IZ == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Warning, "Error opening z.library (v53), Disabling support" );
	}

 	// --- Open Timer

	TimerMsgPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( TimerMsgPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Timer MsgPort" );
		goto bailout;
	}

	TimerMsgPortBit = 1UL << TimerMsgPort->mp_SigBit;

	TimerIOReq = IExec->AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_Size, sizeof( struct TimeRequest ),
		ASOIOR_ReplyPort, TimerMsgPort,
		TAG_END
	);

	if ( TimerIOReq == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Timer IORequest" );
		goto bailout;
	}

	if ( IExec->OpenDevice( "timer.device", 0, (APTR) TimerIOReq, UNIT_MICROHZ ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening Timer Device" );
		goto bailout;
	}

	TimerBase = (APTR) TimerIOReq->Request.io_Device;

	ITimer = (APTR) IExec->GetInterface( (APTR) TimerBase, "main", 1, NULL );

	if ( ITimer == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error failed to obtain Timer Interface" );
		goto bailout;
	}

	// --

	if ( ! cfg->cfg_ProgramDisableGUI )
	{
		BitmapBase = (APTR) IIntuition->OpenClass( "images/bitmap.image", 53, & BitMapClass );

		if ( BitmapBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening label bitmap class" );
			goto bailout;
		}

		ButtonBase = (APTR) IIntuition->OpenClass( "gadgets/button.gadget", 53, & ButtonClass );

		if ( ButtonBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening button gadget class" );
			goto bailout;
		}

		CheckBoxBase = (APTR) IIntuition->OpenClass( "gadgets/checkbox.gadget", 53, & CheckBoxClass );

		if ( CheckBoxBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening checkbox gadget class" );
			goto bailout;
		}

		ChooserBase = (APTR) IIntuition->OpenClass( "gadgets/chooser.gadget", 53, & ChooserClass );

		if ( ChooserBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening chooser gadget class" );
			goto bailout;
		}

		ClickTabBase = (APTR) IIntuition->OpenClass( "gadgets/clicktab.gadget", 53, & ClickTabClass );

		if ( ClickTabBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening clicktab gadget class" );
			goto bailout;
		}

//		GetFileBase = (APTR) IIntuition->OpenClass( "gadgets/getfile.gadget", 53, & GetFileClass );
//
//		if ( GetFileBase == NULL )
//		{
//			goto bailout;
//		}

		LabelBase = (APTR) IIntuition->OpenClass( "images/label.image", 53, & LabelClass );

		if ( LabelBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening label image class" );
			goto bailout;
		}

		// --

		LayoutBase = (APTR) IIntuition->OpenClass( "gadgets/layout.gadget", 53, & LayoutClass );

		if ( LayoutBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening layout gadget class" );
			goto bailout;
		}

		ILayout = (APTR) IExec->GetInterface( (APTR) LayoutBase, "main", 1, NULL );

		if ( ILayout == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error failed to obtain Layout Interface" );
			goto bailout;
		}

		PageClass = ILayout->PAGE_GetClass();

		// --

		ListBrowserBase = (APTR) IIntuition->OpenClass( "gadgets/listbrowser.gadget", 53, & ListBrowserClass );

		if ( ListBrowserBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening listbrowser gadget class" );
			goto bailout;
		}

		IListBrowser = (APTR) IExec->GetInterface( (APTR) ListBrowserBase, "main", 1, NULL );

		if ( IListBrowser == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error failed to obtain ListBrowser Interface" );
			goto bailout;
		}

		// --

		IntegerBase = (APTR) IIntuition->OpenClass( "gadgets/integer.gadget", 53, & IntegerClass );

		if ( IntegerBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening integer gadget class" );
			goto bailout;
		}

		// --

		ScrollerBase = (APTR) IIntuition->OpenClass( "gadgets/scroller.gadget", 53, & ScrollerClass );

		if ( ScrollerBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening scroller gadget class" );
			goto bailout;
		}

		// --

		StringBase = (APTR) IIntuition->OpenClass( "gadgets/string.gadget", 53, & StringClass );

		if ( StringBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening string gadget class" );
			goto bailout;
		}

		// --

		WindowBase = (APTR) IIntuition->OpenClass( "window.class", 53, & WindowClass );

		if ( WindowBase == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening window class" );
			goto bailout;
		}

	}

	// --

	AppID = IApplication->RegisterApplication( "RVNCd",
		REGAPP_LoadPrefs, FALSE,
		REGAPP_SavePrefs, FALSE,
		TAG_END
	);

	if ( AppID == 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to register application" );
		goto bailout;
	}

	IApplication->GetApplicationAttrs( AppID,
		APPATTR_Port, & AppPort,
		TAG_END
	);

	if ( AppPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error failed to obtain App MsgPort" );
		goto bailout;
	}

	AppBit = 1UL << AppPort->mp_SigBit;

	// --

	WinMsgPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( WinMsgPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Window MsgPort" );
		goto bailout;
	}

	WinMsgPortBit = 1UL << WinMsgPort->mp_SigBit;

	// --

	WinAppPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( WinAppPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Window AppPort" );
		goto bailout;
	}

	WinAppPortBit = 1UL << WinAppPort->mp_SigBit;

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static void myCloseSystem( struct Config *cfg UNUSED )
{
APTR node;

	// --

	if ( ActiveEncoding )
	{
		myFree( ActiveEncoding );
		ActiveEncoding = NULL;
	}

	while(( node = IExec->RemHead( & KeyLoggerList )))
	{
		myFree( node );
	}

	while(( node = IExec->RemHead( & LogStringList )))
	{
		myFree( node );
	}

	if ( WinAppPort )
	{
		IExec->FreeSysObject( ASOT_PORT, WinAppPort );
		WinAppPortBit = 0;
		WinAppPort = NULL;
	}

	if ( WinMsgPort )
	{
		IExec->FreeSysObject( ASOT_PORT, WinMsgPort );
		WinMsgPortBit = 0;
		WinMsgPort = NULL;
	}

	if ( CmdMsgPort )
	{
		IExec->FreeSysObject( ASOT_PORT, CmdMsgPort );
		CmdMsgPortBit = 0;
		CmdMsgPort = NULL;
	}

	// --

	if ( AppID )
	{
		IApplication->UnregisterApplication( AppID, TAG_END );
		AppPort = NULL;
		AppBit = 0;
		AppID = 0;
	}

	// -- Close GUI

	if ( WindowBase )
	{
		IIntuition->CloseClass( (APTR) WindowBase );
		WindowClass = NULL;
		WindowBase = NULL;
	}

	if ( StringBase )
	{
		IIntuition->CloseClass( (APTR) StringBase );
		StringClass = NULL;
		StringBase = NULL;
	}

	if ( ScrollerBase )
	{
		IIntuition->CloseClass( (APTR) ScrollerBase );
		ScrollerClass = NULL;
		ScrollerBase = NULL;
	}

	if ( IntegerBase )
	{
		IIntuition->CloseClass( (APTR) IntegerBase );
		IntegerClass = NULL;
		IntegerBase = NULL;
	}

	if ( IListBrowser )
	{
		IExec->DropInterface( (APTR) IListBrowser );
		IListBrowser = NULL;
	}

	if ( ListBrowserBase )
	{
		IIntuition->CloseClass( (APTR) ListBrowserBase );
		ListBrowserClass = NULL;
		ListBrowserBase = NULL;
	}

	if ( ILayout )
	{
		IExec->DropInterface( (APTR) ILayout );
		ILayout = NULL;
	}

	if ( LayoutBase )
	{
		IIntuition->CloseClass( (APTR) LayoutBase );
		LayoutClass = NULL;
		LayoutBase = NULL;
	}

	if ( LabelBase )
	{
		IIntuition->CloseClass( (APTR) LabelBase );
		LabelClass = NULL;
		LabelBase = NULL;
	}

//	if ( GetFileBase )
//	{
//		IIntuition->CloseClass( (APTR) GetFileBase );
//		GetFileClass = NULL;
//		GetFileBase = NULL;
//	}

	if ( ClickTabBase )
	{
		IIntuition->CloseClass( (APTR) ClickTabBase );
		ClickTabClass = NULL;
		ClickTabBase = NULL;
	}

	if ( ChooserBase )
	{
		IIntuition->CloseClass( (APTR) ChooserBase );
		ChooserClass = NULL;
		ChooserBase = NULL;
	}

	if ( CheckBoxBase )
	{
		IIntuition->CloseClass( (APTR) CheckBoxBase );
		CheckBoxClass = NULL;
		CheckBoxBase = NULL;
	}

	if ( ButtonBase )
	{
		IIntuition->CloseClass( (APTR) ButtonBase );
		ButtonClass = NULL;
		ButtonBase = NULL;
	}

	if ( BitmapBase )
	{
		IIntuition->CloseClass( (APTR) BitmapBase );
		BitMapClass = NULL;
		BitmapBase = NULL;
	}

	// -- Close Libraries and Devices

	if ( ITimer )
	{
		IExec->DropInterface( (APTR) ITimer );
		ITimer = NULL;
	}

	if ( TimerBase )
	{
		IExec->CloseDevice( (APTR) TimerIOReq );
		IExec->FreeSysObject( ASOT_IOREQUEST, TimerIOReq );
		TimerBase = NULL;
	}

	if ( TimerMsgPort )
	{
		IExec->FreeSysObject( ASOT_PORT, TimerMsgPort );
		TimerMsgPortBit = 0;
		TimerMsgPort = NULL;
	}

	// --

	if ( IZ )
	{
		IExec->DropInterface( (APTR) IZ );
		IZ = NULL;
	}

	if ( ZBase )
	{
		IExec->CloseLibrary( (APTR) ZBase );
		ZBase = NULL;
	}

	#ifdef NEWLIB

	if ( IUtility )
	{
		IExec->DropInterface( (APTR) IUtility );
		IUtility = NULL;
	}

	if ( UtilityBase )
	{
		IExec->CloseLibrary( (APTR) UtilityBase );
		UtilityBase = NULL;
	}

	#endif

	if ( IP96 )
	{
		IExec->DropInterface( (APTR) IP96 );
		IP96 = NULL;
	}

	if ( P96Base )
	{
		IExec->CloseLibrary( (APTR) P96Base );
		P96Base = NULL;
	}

	if ( IKeymap )
	{
		IExec->DropInterface( (APTR) IKeymap );
		IKeymap = NULL;
	}

	if ( KeymapBase )
	{
		IExec->CloseLibrary( KeymapBase );
		KeymapBase = NULL;
	}

	if ( IIntuition )
	{
		IExec->DropInterface( (APTR) IIntuition );
		IIntuition = NULL;
	}

	if ( IntuitionBase )
	{
		IExec->CloseLibrary( (APTR) IntuitionBase );
		IntuitionBase = NULL;
	}

	if ( IGraphics )
	{
		IExec->DropInterface( (APTR) IGraphics );
		IGraphics = NULL;
	}

	if ( GfxBase )
	{
		IExec->CloseLibrary( (APTR) GfxBase );
		GfxBase = NULL;
	}

	if ( IDiskfont )
	{
		IExec->DropInterface( (APTR) IDiskfont );
		IDiskfont = NULL;
	}

	if ( DiskfontBase )
	{
		IExec->CloseLibrary( DiskfontBase );
		DiskfontBase = NULL;
	}

	if ( IDataTypes )
	{
		IExec->DropInterface( (APTR) IDataTypes );
		IDataTypes = NULL;
	}

	if ( DataTypesBase )
	{
		IExec->CloseLibrary( DataTypesBase );
		DataTypesBase = NULL;
	}

	if ( ICommodities )
	{
		IExec->DropInterface( (APTR) ICommodities );
		ICommodities = NULL;
	}

	if ( CxBase )
	{
		IExec->CloseLibrary( CxBase );
		CxBase = NULL;
	}

	if ( IAsl )
	{
		IExec->DropInterface( (APTR) IAsl );
		IAsl = NULL;
	}

	if ( AslBase )
	{
		IExec->CloseLibrary( AslBase );
		AslBase = NULL;
	}

	if ( IApplication )
	{
		IExec->DropInterface( (APTR) IApplication );
		IApplication = NULL;
	}

	if ( AppBase )
	{
		IExec->CloseLibrary( AppBase );
		AppBase = NULL;
	}
}

// --

static void Handle_Application( struct Config *cfg UNUSED )
{
struct ApplicationMsg *msg;

	while(( msg = (APTR) IExec->GetMsg( AppPort )))
	{
		switch( msg->type )
		{
			case APPLIBMT_Quit:
			{
				ProgramRunning = FALSE;
				break;
			}

			default:
			{
				break;
			}
		}

		IExec->ReplyMsg( (APTR) msg );
	}
}

// --

static void myHandle_Timer( struct Config *cfg )
{
	// -- Remove Request

	IExec->WaitPort( TimerMsgPort );
	IExec->GetMsg( TimerMsgPort );

	// -- Restart Timer

	TimerIOReq->Request.io_Command = TR_ADDREQUEST;
	TimerIOReq->Time.Seconds = 1;
	TimerIOReq->Time.Microseconds = 0;
	IExec->SendIO( (APTR) TimerIOReq );

	// -- 

	myGUI_TimerTick( cfg );
}

// --

static int myOpen_CxBroker( struct Config *cfg UNUSED )
{
int error;

	// --

	if ( ICommodities == NULL )
	{
		error = FALSE;
		goto bailout;
	}

	if ( cfg->cfg_ProgramDisableGUI )
	{
		BrokerData.nb_Flags &= ~COF_SHOW_HIDE;
	}

	// --

	error = TRUE;

	CxBrokerMsgPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( CxBrokerMsgPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Failed to create CxBroker MsgPort" );
		goto bailout;
	}

	CxBrokerMsgPortBit = 1UL << CxBrokerMsgPort->mp_SigBit;

	BrokerData.nb_Port = CxBrokerMsgPort;

	CxBrokerAdr = ICommodities->CxBroker( &BrokerData, &CxBrokerError );

	if ( CxBrokerAdr == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Failed to create CxBroker" );
		goto bailout;
	}

//	  BrokerHotKey = AddHotKey( "control alt o", 1203 );

	ICommodities->ActivateCxObj( CxBrokerAdr, TRUE );

	error = FALSE;

bailout:

	return(	error );
}

// --

static void myClose_CxBroker( struct Config *cfg UNUSED )
{
	if ( CxBrokerAdr )
    {
		ICommodities->DeleteCxObjAll( CxBrokerAdr );
		CxBrokerAdr = NULL;
	}

	if ( CxBrokerMsgPort )
    {
		IExec->FreeSysObject( ASOT_PORT, CxBrokerMsgPort );
		CxBrokerMsgPortBit = 0;
		CxBrokerMsgPort = NULL;
	}
}

// --

static void myHandle_CxBroker( struct Config *cfg UNUSED )
{
CxMsg *msg;
int cnt;
	
	while(( msg = (APTR) IExec->GetMsg( CxBrokerMsgPort )))
	{
		switch( ICommodities->CxMsgType( msg ))
		{
//			  case CXM_IEVENT:
//			  {
//				  switch( ICommodities->CxMsgID( msg ))
//				  {
//					  case 1203:
//					  {
//                        if ( MainWindow )
//                        {
//                            IIntuition->ScreenToFront( MainScreen );
//                            IIntuition->ActivateWindow( MainWindow );
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
				switch( ICommodities->CxMsgID( msg ))
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

						if ( ! cfg->cfg_ProgramDisableGUI )
						{
							// Check for Open windows

							for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
							{
								if ( cfg->cfg_WinData[cnt].Status != WINSTAT_Close )
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
								if ( cfg->cfg_WinData[cnt].Status != WINSTAT_Close )
								{
									cfg->cfg_WinData[cnt].OpenWin( cfg );
								}
							}
						}
						else
						{
							Log_PrintF( cfg, LOGTYPE_Warning, "Program: GUI Disabled can't open" );
						}
						break;
					}

					case CXCMD_DISAPPEAR:
					{
						// User hit HideInterface in Exchange
						for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
						{
							if ( cfg->cfg_WinData[cnt].Status != WINSTAT_Close )
							{
								cfg->cfg_WinData[cnt].CloseWin( cfg );
							}
						}
						break;
					}

					case CXCMD_KILL:
					{
						if ( cfg->UserCount )
						{
							if ( IDOS->TimedDosRequesterTags(
								TDR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
								TDR_ImageType, TDRIMAGE_QUESTION,
								TDR_TitleString, "RVNCd",
								TDR_FormatString, "There are %ld user(s) connected\nAre you sure you want to Quit?",
								TDR_GadgetString, "_No|_Yes",
								TDR_Arg1, cfg->UserCount,
								TAG_END ) == 0 )
							{
								ProgramRunning = FALSE;
							}
						}
						else
						{
							ProgramRunning = FALSE;
						}
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

		IExec->ReplyMsg( (APTR) msg );
	}
}

// --

static int myCmd_HandleKey( struct Config *cfg UNUSED, APTR msgptr )
{
struct CommandKey *msg;
int doFree;

	IExec->AddTail( & KeyLoggerList, msgptr );
	KeyLoggerCount++;

	while( MAX_KeyListNodes < KeyLoggerCount )
	{
		msg = (APTR) IExec->RemHead( & KeyLoggerList );
		myFree( msg );
		KeyLoggerCount--;
	}

	myGUI_AddKeyMessage( cfg, msgptr );

	doFree = FALSE;

	return( doFree );
}

// --

static int myCmd_HandleLogString( struct Config *cfg UNUSED, APTR msgptr )
{
struct CommandLogString *msg;
int doFree;

	IExec->AddTail( & LogStringList, msgptr );
	LogStringCount++;

	while( MAX_LogListNodes < LogStringCount )
	{
		msg = (APTR) IExec->RemHead( & LogStringList );
		myFree( msg );
		LogStringCount--;
	}

	myGUI_AddLogStringMessage( cfg, msgptr );

	doFree = FALSE;

	return( doFree );
}

// --

static int myCmd_HandleEncoding( struct Config *cfg UNUSED, APTR msgptr )
{
int doFree;

	if ( ActiveEncoding )
	{
		myFree( ActiveEncoding );
	}

	ActiveEncoding = msgptr;

	if ( cfg->cfg_WinData[WIN_Encodings].Status == WINSTAT_Open )
	{
		myGUI_EncodingMessage( cfg, msgptr );
	}

	doFree = FALSE;

	return( doFree );
}

// --

static void myHandle_Commands( struct Config *cfg )
{
struct CommandMessage *msg;
int doFree;

	while( TRUE )
	{
		msg = (APTR) IExec->GetMsg( CmdMsgPort );

		if ( msg == NULL )
		{
			break;
		}

		doFree = TRUE;

		switch( msg->cm_Command )
		{
			case CMD_Key:
			{
				doFree = myCmd_HandleKey( cfg, msg );
				break;
			} 

			case CMD_PxlFmt:
			{
				myGUI_PxlFmtMessage( cfg, (APTR) msg );
				break;
			}

			case CMD_Session:
			{
				myGUI_SessionMessage( cfg, (APTR) msg );
				break;
			} 

			case CMD_Encoding:
			{
				doFree = myCmd_HandleEncoding( cfg, msg );
				break;
			}

			case CMD_LogString:
			{
				doFree = myCmd_HandleLogString( cfg, msg );
				break;
			}

			case CMD_RefreshGUI:
			{
				myGUI_RefreshMessage( cfg, (APTR) msg );
				break;
			} 

			case CMD_ConnectStats:
			{
				myGUI_RefreshStats( cfg, (APTR) msg );
				break;
			} 

			default:
			{
				break;
			}
		}

		if ( doFree )
		{
			myFree( msg );
		}
	}
}

// --

static void myHandle_Windows( struct Config *cfg )
{
	#if 0

// Problem as we remove the imsg from the MsgPort
// Then calling the WinObject with WM_HANDLEINPUT will fail.
// If its was a classic Intuition Window and not a Reaction
// window, we would handle each idcmp message one at a time.

struct IntuiMessage *msg;
void (*func)( struct Config *cfg );

	while( TRUE )
	{
		msg = (APTR) IExec->GetMsg( WinMsgPort );

		if ( msg == NULL )
		{
			break;
		}

		if (( msg->IDCMPWindow ) && (( func = (APTR) msg->IDCMPWindow->UserData )))
		{
			func( cfg );
		}

		IExec->ReplyMsg( & msg->ExecMessage );
	}

	#else

int cnt;

	for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
	{
		if ( cfg->cfg_WinData[cnt].HandleWin )
		{
			cfg->cfg_WinData[cnt].HandleWin( cfg );
		}
	}

	#endif
}

// --

#ifdef FUNCLOG

struct SignalSemaphore FuncLogSema;

void FuncLogAdd( char *txt )
{
BPTR h;

	if ( txt == NULL )
	{
		return;
	}

	h = IDOS->Open( "progdir:FuncLog.txt", MODE_READWRITE );

	if ( h == 0 )
	{
		return;
	}

	IDOS->Write( h, txt, strlen( txt ));

	IDOS->Close( h );
}

#endif
