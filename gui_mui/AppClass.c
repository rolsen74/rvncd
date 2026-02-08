
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct MUI_CustomClass *AppClass;

// --

struct myData
{
	LONG dummy;
};

// --

#ifndef SAVEDS
#warning 12
#endif

#ifndef ASM
#warning 34
#endif

#ifndef REG
#warning 56
#endif

static SAVEDS ASM U32 myDispatcher( REG(a0,struct IClass *cl), REG(a2,Object *obj), REG(a1,Msg msg) )
{
U32 retval;

//	switch( msg->MethodID )
//	{
//		case MUIM_AskMinMax: return(mAskMinMax(cl,obj,(APTR)msg));
//		case MUIM_Draw     : return(mDraw     (cl,obj,(APTR)msg));
//	}

	retval = DoSuperMethodA( cl, obj, msg );

	return( retval );
}

// --

S32 AppClass_Init( struct Config *cfg UNUSED )
{
S32 retval;

	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "AppClass_Init\n" );
	}

	// --

	retval = FALSE;

	AppClass = MUI_CreateCustomClass( 
		NULL, 
		MUIC_Application, 
		NULL, 
		sizeof( struct myData ), 
		myDispatcher
	);

	retval = TRUE;

//bailout:

	return( retval );
}

// --

void AppClass_Free( struct Config *cfg UNUSED )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "AppClass_Free\n" );
	}

	// --

	if ( AppClass )
	{
		MUI_DeleteCustomClass( AppClass );
		AppClass = NULL;
	}
}

// --
