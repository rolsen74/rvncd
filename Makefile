
###########################################################################

.PHONY: all clean revision install

###########################################################################

CRT			:= newlib
VERSION		:= 1
REVISION	:= 38

###########################################################################

CFLAGS		:= -O2
CFLAGS		+= -W
CFLAGS		+= -Wall
#CFLAGS		+= -Wwrite-strings
CFLAGS		+= -mcrt=$(CRT)

# Use clib memory system insted of Exec
#CFLAGS		+= -DDO_CLIB

LDFLAGS		:= -mcrt=$(CRT)

LIBS		:= -L.

SRCS		:= RVNCd.c
SRCS		+= Action.c
SRCS		+= ARexx.c
SRCS		+= Config.c
SRCS		+= Config_Args.c
SRCS		+= Config_Read.c
SRCS		+= Config_Save.c
SRCS		+= d3des.c
SRCS		+= Encode_Init.c
SRCS		+= Encode_Render.c
SRCS		+= GUI_About.c
SRCS		+= GUI_Encodings.c
SRCS		+= GUI_IPEdit.c
SRCS		+= GUI_KeyLogger.c
SRCS		+= GUI_Main.c
SRCS		+= GUI_Pixel.c
SRCS		+= GUI_Quit.c
SRCS		+= GUI_Session.c
SRCS		+= Log.c
SRCS		+= LogTime.c
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
SRCS		+= _WatchDog.c

SRCS		+= _VNC_v33.c
SRCS		+= _VNC_v37.c

# VNC Startup
SRCS		+= _VNC_1_Version.c
SRCS		+= _VNC_2_Authenticate_v33.c
SRCS		+= _VNC_2_Authenticate_v37.c
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
#CFLAGS		+= -gstabs
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

.PHONY: clean
clean:
	$(RM) $(TARGET) $(TARGET).map $(OBJS)

.PHONY: revision
revision:
	bumprev -v $(VERSION) -r $(REVISION) -n $(TARGET) -b RVNCd

strip:
	@$(LS) $(TARGET)
	@$(STRIP) $(TARGET)
	@$(LS) $(TARGET)

###########################################################################

$(TARGET).o:				Makefile RVNCd.h RVNCd_rev.h
Action.o:					Makefile RVNCd.h
ARexx.o:					Makefile RVNCd.h
Config.o:					Makefile RVNCd.h
Config_Args.o:				Makefile RVNCd.h
Config_Read.o:				Makefile RVNCd.h
Config_Save.o:				Makefile RVNCd.h
d3des.o:					Makefile RVNCd.h
Encode_Init.o:				Makefile RVNCd.h
Encode_Render.o:			Makefile RVNCd.h
GUI_About.o:				Makefile RVNCd.h
GUI_Encodings.o:			Makefile RVNCd.h
GUI_IPEdit.o:				Makefile RVNCd.h
GUI_KeyLogger.o:			Makefile RVNCd.h

GUI_Main.o:					Makefile RVNCd.h \
							GUI_Main_Actions_Program.c \
							GUI_Main_Actions_Server.c \
							GUI_Main_Actions_User.c \
							GUI_Main_Actions.c \
							GUI_Main_List_Black.c \
							GUI_Main_List_White.c \
							GUI_Main_Log.c \
							GUI_Main_Menu.c \
							GUI_Main_Mouse.c \
							GUI_Main_Program.c \
							GUI_Main_Protocols_v33.c \
							GUI_Main_Protocols_v37.c \
							GUI_Main_Protocols.c \
							GUI_Main_Screen.c \
							GUI_Main_Server.c \
							GUI_Main_Stats.c \

GUI_Pixel.o:				Makefile RVNCd.h
GUI_Quit.o:					Makefile RVNCd.h
GUI_Session.o:				Makefile RVNCd.h
Log.o:						Makefile RVNCd.h
LogTime.o:					Makefile RVNCd.h
Misc.o:						Makefile RVNCd.h
NewBuffer.o:				Makefile RVNCd.h
NewBuffer_Cursor.o:			Makefile RVNCd.h
NewBuffer_Raw.o:			Makefile RVNCd.h
NewBuffer_ZLib.o:			Makefile RVNCd.h
Server.o:					Makefile RVNCd.h

_Gfx_Read.o:				Makefile RVNCd.h

_Net_Read.o:				Makefile RVNCd.h

_Net_Send.o:				Makefile RVNCd.h
_Net_Send_Blanker.o:		Makefile RVNCd.h
_Net_Send_ClipBoard.o:		Makefile RVNCd.h
_Net_Send_Pointer.o:		Makefile RVNCd.h
_Net_Send_Session.o:		Makefile RVNCd.h
_Net_Send_Socket.o:			Makefile RVNCd.h

_WatchDog.o:				Makefile RVNCd.h

_VNC_v33.o:					Makefile RVNCd.h
_VNC_v37.o:					Makefile RVNCd.h

_VNC_1_Version.o:			Makefile RVNCd.h
_VNC_2_Authenticate_v33.o:	Makefile RVNCd.h
_VNC_2_Authenticate_v37.o:	Makefile RVNCd.h
_VNC_3_ClientInit.o:		Makefile RVNCd.h
_VNC_4_ServerInit.o:		Makefile RVNCd.h

_VNC_A_SetPixelFormat.o:	Makefile RVNCd.h
_VNC_B_SetEncoding.o:		Makefile RVNCd.h
_VNC_C_UpdateRequest.o:		Makefile RVNCd.h
_VNC_D_Keyboard.o:			Makefile RVNCd.h
_VNC_E_Mouse.o:				Makefile RVNCd.h
_VNC_F_Clipboard.o:			Makefile RVNCd.h

###########################################################################
