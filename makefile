
###########################################################################

.PHONY: all clean revision install makedirs

###########################################################################
#
# Compile info:
#
# Compile : make SYS=AOS4 GUI=RA GFX=P96
#
# Options:
#
#  SYS = AmigaOS3 (AOS3), AmigaOS4 (AOS4)
#  SYS - 'AOS3' 'AOS4'			-- Target System
#
#  GFX = CGFX/P96 (AOS3/AOS4), GFX54 (AOS4 only)
#  GFX - 'CGFX' 'P96' 'GFX54'	-- Graphics System (for Screen Capture)
#
#  GUI = None (No gui), Reaction (RA AOS4 only)
#  GUI - 'NONE' 'RA' 'MUI'		-- Program GUI
#
#  Extra Options
#
#  AREXX=1	- Compile with ARexx Support
#  CX=1		- Compile with CxBroker Support
#  ZLIB=1	- Compile with ZLib Support
#

VERSION		:= 1
REVISION	:= 45
TARGET		:= rVNCd

# Default
SYS			?= AOS4
GFX			?= GFX54
GUI			?= RA

# Options
AREXX		?= 0
CX			?= 1
ZLIB		?= 0

# Object Dir
OBJDIR		:= obj

# Binary Dir
BINDIR		:= bin

# Compiler Flags
CFLAGS		:= -O2
CFLAGS		+= -Iinc
#CFLAGS		+= -DDEBUG
#CFLAGS		+= -gstabs
#CFLAGS		+= -DSHOW_WORK_TILE

# Linker Flags
LDFLAGS		:=

# Link Libraries
LIBS		:=

# Empty Source File List (Will grow)
SRCS		:=

# Host SYS we compile on
HOSTOS		:= $(shell uname)

###########################################################################

# --

ifeq ($(HOSTOS),AmigaOS)

LS			:= list
RM			:= rm
RMDIR		:= rm -rf
MKDIR		:= mkdir -p

# --

else ifeq ($(HOSTOS),Linux)

LS			:= ls -lort
RM			:= rm
RMDIR		:= rm -rf
MKDIR		:= mkdir -p

# --

else

$(error Unknown Host SYS type: $(HOSTOS).)

endif

###########################################################################
# Set Target System
# -- AmigaOS3

ifeq ($(SYS),AOS3)

  $(info Compiling for AmigaOS3)

  CFLAGS	+= -D_AOS3_
  CFLAGS	+= -Isys_aos3

  LIBS		+= -lamiga

  TARGET	:= $(TARGET)_aos3

  OBJDIR	:= obj/aos3

  SRCDIRS	+= sys_aos3

  OSDIR		:= _aos3

  # Set Compiler
  CC		:= vc -quiet
  LD		:= vlink
  AS		:= vasmm68k_mot

###########################################################################
# Set Target System
# -- AmigaOS4

else ifeq ($(SYS),AOS4)

  $(info Compiling for AmigaOS4)

  CFLAGS	+= -D_AOS4_
  CFLAGS	+= -Isys_aos4
  CFLAGS	+= -DHAVE_APPLIB
  CFLAGS	+= -mcrt=newlib

  LDFLAGS	+= -mcrt=newlib

  SRCS		+= sys_aos4/rVNCd_Application.c
  SRCS		+= sys_aos4/rVNCd_ARexx.c
  SRCS		+= sys_aos4/rVNCd_ZLib.c

  TARGET	:= $(TARGET)_aos4

  OBJDIR	:= obj/aos4

  SRCDIRS	+= sys_aos4

  OSDIR		:= sys_aos4

  # Set Compiler
  ifeq ($(HOSTOS),AmigaOS)

    STRIP	:= strip
    CC		:= gcc

  else

    STRIP	:= ppc-amigaos-strip
    CC		:= ppc-amigaos-gcc

  endif

  # Compiler flags that vbcc don't like
  CFLAGS	+= -W
  CFLAGS	+= -Wall
  CFLAGS	+= -MMD
  CFLAGS	+= -MP

###########################################################################
# Set Target System
# -- unknown

else

  $(error Unknown Target SYS type: $(SYS). Please set SYS to 'AOS3' or 'AOS4')

endif

###########################################################################
# Set Graphic System
# -- Cybergraphics 

ifeq ($(GFX),CGFX)

  $(info Using Cybergraphics)

  CFLAGS	+= -DHAVE_CGFX
  CFLAGS	+= -Igfx_cgfx


  TARGET	:= $(TARGET)_cgfx

  OBJDIR	:= $(OBJDIR)_cgfx

  SRCDIRS	+= gfx_cgfx

###########################################################################
# Set Graphic System
# -- Picasso96 

else ifeq ($(GFX),P96)

  $(info Using Picasso96)

  CFLAGS	+= -DHAVE_P96
  CFLAGS	+= -Igfx_p96

#  CFLAGS	+= -Wno-deprecated-declarations

  TARGET	:= $(TARGET)_p96

  OBJDIR	:= $(OBJDIR)_p96

  SRCDIRS	+= gfx_p96

###########################################################################
# Set Graphic System
# -- Graphics library v54 

else ifeq ($(GFX),GFX54)

  $(info Using Graphics.lib v54)

  CFLAGS	+= -DHAVE_GFX54
  CFLAGS	+= -Igfx_gfx54

  TARGET	:= $(TARGET)_gfx54

  OBJDIR	:= $(OBJDIR)_gfx54

  SRCDIRS	+= gfx_gfx54

###########################################################################
# Set Graphic System
# -- unknown

else

  $(error Unknown Graphics system type: $(GFX). Please set SYS to 'CGFX', 'P96' or 'GFX54')

endif

###########################################################################
# Set Program GUI
# -- Reaction (RA)

ifeq ($(GUI), RA)

  $(info Using Reaction GUI)

  CFLAGS	+= -Igui_ra
  CFLAGS	+= -DGUI_RA

  SRCDIRS	+= gui_ra
  SRCDIRS	+= gui_ra/win_About
  SRCDIRS	+= gui_ra/win_Encodings
  SRCDIRS	+= gui_ra/win_IPEdit
  SRCDIRS	+= gui_ra/win_KeyLogger
  SRCDIRS	+= gui_ra/win_Main
  SRCDIRS	+= gui_ra/win_Pixel
  SRCDIRS	+= gui_ra/win_Quit
  SRCDIRS	+= gui_ra/win_Session

  TARGET	:= $(TARGET)_ra
  OBJDIR	:= $(OBJDIR)_ra
  GUIDIR	:= gui_ra

###########################################################################
# Set Program GUI
# -- Magic User Interface (MUI)

else ifeq ($(GUI), MUI)

  $(info Using Magic User Interface (MUI) GUI)

  CFLAGS	+= -Igui_mui
  CFLAGS	+= -DGUI_MUI

  SRCS		+= gui_mui/AppClass.c

  SRCS		+= gui_mui/GUI.c
  SRCS		+= gui_mui/GUI_About.c
  SRCS		+= gui_mui/GUI_Encodings.c
  SRCS		+= gui_mui/GUI_IPEdit.c
  SRCS		+= gui_mui/GUI_KeyLogger.c
  SRCS		+= gui_mui/GUI_PixelFormat.c
  SRCS		+= gui_mui/GUI_Quit.c
  SRCS		+= gui_mui/GUI_Session.c

  SRCS		+= gui_mui/GUI_Main.c
  SRCS		+= gui_mui/GUI_Main_01_Main.c
  SRCS		+= gui_mui/GUI_Main_02_Program.c
  SRCS		+= gui_mui/GUI_Main_03_Actions.c
  SRCS		+= gui_mui/GUI_Main_04_Screen.c
  SRCS		+= gui_mui/GUI_Main_05_Server.c
  SRCS		+= gui_mui/GUI_Main_06_ServerStat.c
  SRCS		+= gui_mui/GUI_Main_07_Log.c
  SRCS		+= gui_mui/GUI_Main_08_Mouse.c
  SRCS		+= gui_mui/GUI_Main_09_Protocols.c
  SRCS		+= gui_mui/GUI_Main_10_IPs.c

  TARGET	:= $(TARGET)_mui

  OBJDIR	:= $(OBJDIR)_mui

  GUIDIR	:= gui_mui

###########################################################################
# Set Program GUI
# -- No gui (NONE)

else ifeq ($(GUI), NONE)

  $(info GUI Disabled)

  CFLAGS	+= -Igui_none
  CFLAGS	+= -DGUI_NONE

  SRCS		+= gui_none/GUI.c

  TARGET	:= $(TARGET)_nogui

  OBJDIR	:= $(OBJDIR)_nogui

  GUIDIR	:= gui_none

###########################################################################
# Set Program GUI
# -- Unknown

else

  $(error Unknown GUI type: $(GUI). Please set GUI to 'NONE' or 'RA')

endif

###########################################################################
# - Extra Options

ifneq ($(GUI), MUI)
# MUI have ARexx+Cx build in

ifeq ($(AREXX),1)
  $(info Enabling ARexx Support)
  CFLAGS += -DHAVE_AREXX
endif

ifeq ($(CX),1)
  $(info Enabling CxBroker Support)
  CFLAGS	+= -DHAVE_CXBROKER
  SRCS		+= src/CxBroker.c
endif

endif

ifeq ($(ZLIB),1)
  $(info Enabling ZLib Support)
  CFLAGS += -DHAVE_ZLIB
endif

###########################################################################
# - rVNCd -

SRCS		+= src/rVNCd.c
SRCS		+= src/rVNCd_Action.c
SRCS		+= src/rVNCd_Commands.c
SRCS		+= src/rVNCd_Misc.c
SRCS		+= src/rVNCd_System.c
SRCS		+= src/rVNCd_Timer.c

# VNC Server
SRCS		+= src/Beep.c
SRCS		+= src/Config.c
SRCS		+= src/Config_Args.c
SRCS		+= src/Config_Read.c
SRCS		+= src/Config_Read_IP.c
SRCS		+= src/Config_Read_STR.c
SRCS		+= src/Config_Read_VAL.c
SRCS		+= src/Config_Save.c
SRCS		+= src/Config_Save_STR.c
SRCS		+= src/Config_Save_VAL.c
SRCS		+= src/d3des.c
SRCS		+= src/Encode_Init.c
SRCS		+= src/Encode_Render.c
SRCS		+= src/Log.c
SRCS		+= src/LogTime.c
SRCS		+= src/NewBuffer_Update.c
SRCS		+= src/NewBuffer_Update_Tile_Count.c
SRCS		+= src/NewBuffer_Update_Tile_Check.c
SRCS		+= src/NewBuffer_Update_Tile_Fill.c
SRCS		+= src/NewBuffer_Cursor_Update.c
SRCS		+= src/NewBuffer_Cursor_Rich.c
SRCS		+= src/NewBuffer_Cursor_Soft.c
SRCS		+= src/NewBuffer_LastRect.c
SRCS		+= src/NewBuffer_Raw.c
SRCS		+= src/NewBuffer_Raw_8.c
SRCS		+= src/NewBuffer_Raw_16.c
SRCS		+= src/NewBuffer_Raw_32.c
SRCS		+= src/NewBuffer_RRE.c
SRCS		+= src/NewBuffer_RRE_8.c
SRCS		+= src/NewBuffer_RRE_16.c
SRCS		+= src/NewBuffer_RRE_32.c
SRCS		+= src/NewBuffer_ZLib.c
SRCS		+= src/NewBuffer_ZLib_8.c
SRCS		+= src/NewBuffer_ZLib_16.c
SRCS		+= src/NewBuffer_ZLib_32.c
SRCS		+= src/NewBeep_Msg.c
SRCS		+= src/NewEncoding_Msg.c
SRCS		+= src/NewPixelFmt_Msg.c
SRCS		+= src/Server.c
SRCS		+= src/Shell_Buffer.c
SRCS		+= src/Shell_Process.c
SRCS		+= src/Tile_SendBuffer.c
SRCS		+= src/_Gfx_Read.c
SRCS		+= src/_Net_Read.c
SRCS		+= src/_Net_Send.c
SRCS		+= src/_Net_Send_ClipBoard.c
SRCS		+= src/_Net_Send_Pointer.c
SRCS		+= src/_Net_Send_Session.c
SRCS		+= src/_Net_Send_Socket.c
SRCS		+= src/_WatchDog.c

SRCS		+= src/_VNC_v33.c
SRCS		+= src/_VNC_v37.c

# Misc - Support Code
SRCS		+= src/__List.c
SRCS		+= src/__Memory.c
SRCS		+= src/__Port.c

# VNC - Connect
SRCS		+= src/_VNC_1_Version.c
SRCS		+= src/_VNC_2_Authenticate_v33.c
SRCS		+= src/_VNC_2_Authenticate_v37.c
SRCS		+= src/_VNC_3_ClientInit.c
SRCS		+= src/_VNC_4_ServerInit.c

# VNC - Client to Server requests
SRCS		+= src/_VNC_A_SetPixelFormat.c
SRCS		+= src/_VNC_B_SetEncoding.c
SRCS		+= src/_VNC_C_UpdateRequest.c
SRCS		+= src/_VNC_D_Keyboard.c
SRCS		+= src/_VNC_E_Mouse.c
SRCS		+= src/_VNC_F_Clipboard.c

###########################################################################

SRCS		+= $(wildcard $(addsuffix /*.c, $(SRCDIRS)))
SRCS		:= $(sort $(SRCS))

OBJS		:= $(SRCS:%.c=$(OBJDIR)/%.o)
DEPS		:= $(SRCS:%.c=$(OBJDIR)/%.d)

TARGET		:= $(BINDIR)/$(TARGET).exe
all:		makedirs $(TARGET)

-include $(DEPS)

makedirs:
	@$(MKDIR) $(OBJDIR) $(OBJDIR)/$(OSDIR) $(OBJDIR)/$(GUIDIR) $(OBJDIR)/src $(BINDIR)

$(TARGET): $(OBJS)
	@echo "Linking '$@'"
	@$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	$(RMDIR) $(OBJDIR) $(TARGET)

revision:
	bumprev -v $(VERSION) -r $(REVISION) -n rVNCd -b rVNCd

strip:
	@$(LS) $(TARGET)
	@$(STRIP) $(TARGET)
	@$(LS) $(TARGET)

$(OBJDIR)/%.o: %.c
	@echo "Compiling '$@'"
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

###########################################################################
