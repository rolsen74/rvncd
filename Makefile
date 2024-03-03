
###########################################################################

.PHONY: all clean revision install

###########################################################################

CRT			:= newlib
VERSION		:= 1
REVISION	:= 29

###########################################################################

CFLAGS		:= -O2
CFLAGS		+= -W
CFLAGS		+= -Wall
#CFLAGS		+= -Wwrite-strings
CFLAGS		+= -mcrt=$(CRT)

# Use clib memory system insted of Exec
#CFLAGS		+= -DDO_CLIB

#CFLAGS		+= -DVAL_TEST

#CFLAGS		+= -DFUNCLOG

LDFLAGS		:= -mcrt=$(CRT)

LIBS		:= -L.

SRCS		:= RVNCd.c
SRCS		+= Action.c
SRCS		+= ARexx.c
SRCS		+= Config.c
SRCS		+= Config_Args.c
SRCS		+= Config_File.c
SRCS		+= Config_Save.c
SRCS		+= Config__Actions.c
SRCS		+= Config__Log.c
SRCS		+= Config__Program.c
SRCS		+= Config__Server.c
SRCS		+= Config__ServerCfg.c
SRCS		+= Config__BlackList.c
SRCS		+= Config__WhiteList.c
SRCS		+= d3des.c
SRCS		+= Encode_Init.c
SRCS		+= Encode_Render.c
SRCS		+= GUI_About.c
SRCS		+= GUI_Encodings.c
SRCS		+= GUI_KeyLogger.c
SRCS		+= GUI_Main.c
SRCS		+= GUI_Pixel.c
SRCS		+= GUI_Session.c
SRCS		+= Log.c
SRCS		+= Misc.c
SRCS		+= NewBuffer.c
SRCS		+= NewBuffer_Cursor.c
SRCS		+= NewBuffer_Raw.c
SRCS		+= NewBuffer_ZLib.c
SRCS		+= Server.c
SRCS		+= _Gfx_Read.c
SRCS		+= _Net_Read.c
SRCS		+= _Net_Send.c
SRCS		+= _Net_Send_Blanker.c
SRCS		+= _Net_Send_ClipBoard.c
SRCS		+= _Net_Send_Pointer.c
SRCS		+= _Net_Send_Session.c
SRCS		+= _Net_Send_Socket.c

SRCS		+= _VNC.c

# VNC Startup
SRCS		+= _VNC_1_Version.c
SRCS		+= _VNC_2_Authenticate.c
SRCS		+= _VNC_3_ClientInit.c
SRCS		+= _VNC_4_ServerInit.c

# VNC - Client to Server requests
SRCS		+= _VNC_A_SetPixelFormat.c
SRCS		+= _VNC_B_SetEncoding.c
SRCS		+= _VNC_C_UpdateRequest.c
SRCS		+= _VNC_D_Keyboard.c
SRCS		+= _VNC_E_Mouse.c
SRCS		+= _VNC_F_Clipboard.c

###########################################################################

ifeq ($(OS),Windows_NT)
	detected_os := Windows
else
	detected_os := $(shell uname)
endif

# --

ifeq ($(detected_os),Windows)

LS			:= dir
RM			:= del
TARGET		:= RVNCd.exe
CC			:= gcc
STRIP		:= strip

# --

else ifeq ($(detected_os),AmigaOS)

LS			:= list
RM			:= delete
CFLAGS		+= -gstabs
TARGET		:= RVNCd
CC			:= gcc
STRIP		:= strip

# --

else ifeq ($(detected_os),Linux)

LS			:= ls -lort
RM			:= rm
#CFLAGS		+= -gstabs
TARGET		:= RVNCd
CC			:= ppc-amigaos-gcc
STRIP		:= ppc-amigaos-strip

endif

###########################################################################
# Nothing should need changing below this line

all:		$(TARGET)

OBJS		:= $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS) -Wl,--cref,-M,-Map=$@.map

#	 $(CC) $(LDFLAGS) -o $(TARGET).debug $(OBJS) $(LIBS) -Wl,--cref,-M,-Map=$@.map
#	 $(STRIP) --strip-all -R.comment $(TARGET).debug -o $(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(TARGET).debug $(TARGET).map $(OBJS)

.PHONY: revision
revision:
	bumprev -v $(VERSION) -r $(REVISION) -n $(TARGET) -b RVNCd

strip:
	@$(LS) $(TARGET)
	@$(STRIP) $(TARGET)
	@$(LS) $(TARGET)

###########################################################################

$(TARGET).o:			Makefile RVNCd.h RVNCd_rev.h
Action.o:				Makefile RVNCd.h
ARexx.o:				Makefile RVNCd.h
BufferRender.o:			Makefile RVNCd.h
Config.o:				Makefile RVNCd.h
Config_Args.o:			Makefile RVNCd.h
Config_File.o:			Makefile RVNCd.h
Config_Save.o:			Makefile RVNCd.h
Config__Actions.o:		Makefile RVNCd.h
Config__BlackList.o:	Makefile RVNCd.h
Config__Log.o:			Makefile RVNCd.h
Config__Program.o:		Makefile RVNCd.h
Config__Server.o:		Makefile RVNCd.h
Config__ServerCfg.o:	Makefile RVNCd.h
Config__WhiteList.o:	Makefile RVNCd.h
d3des.o:				Makefile RVNCd.h
Encode_Init.o:			Makefile RVNCd.h
Encode_Render.o:		Makefile RVNCd.h
GUI_About.o:			Makefile RVNCd.h
GUI_Encodings.o:		Makefile RVNCd.h
GUI_KeyLogger.o:		Makefile RVNCd.h
GUI_Main.o:				Makefile RVNCd.h
GUI_Pixel.o:			Makefile RVNCd.h
GUI_Session.o:			Makefile RVNCd.h
Log.o:					Makefile RVNCd.h
Misc.o:					Makefile RVNCd.h
NewBuffer.o:			Makefile RVNCd.h
NewBuffer_Cursor.o:		Makefile RVNCd.h
NewBuffer_Raw.o:		Makefile RVNCd.h
NewBuffer_ZLib.o:		Makefile RVNCd.h
Server.o:				Makefile RVNCd.h

_Gfx_Read.o:			Makefile RVNCd.h

_Net_Read.o:			Makefile RVNCd.h

_Net_Send.o:			Makefile RVNCd.h
_Net_Send_Blanker.o:	Makefile RVNCd.h
_Net_Send_ClipBoard.o:	Makefile RVNCd.h
_Net_Send_Pointer.o:	Makefile RVNCd.h
_Net_Send_Session.o:	Makefile RVNCd.h
_Net_Send_Socket.o:		Makefile RVNCd.h

_VNC.o:					Makefile RVNCd.h

_VNC_1_Version.o:		Makefile RVNCd.h
_VNC_2_Authenticate.o:	Makefile RVNCd.h
_VNC_3_ClientInit.o:	Makefile RVNCd.h
_VNC_4_ServerInit.o:	Makefile RVNCd.h

_VNC_A_SetPixelFormat.o:Makefile RVNCd.h
_VNC_B_SetEncoding.o:	Makefile RVNCd.h
_VNC_C_UpdateRequest.o:	Makefile RVNCd.h
_VNC_D_Keyboard.o:		Makefile RVNCd.h
_VNC_E_Mouse.o:			Makefile RVNCd.h
_VNC_F_Clipboard.o:		Makefile RVNCd.h

###########################################################################
