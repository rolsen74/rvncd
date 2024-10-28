# Amiga VNC Server

## Protocols:

- Only v3.3 and v3.7 is supported.

## Authentications:

- None: No login needed.

- d3des: Weak 8-byte password, encrypted with DES.

	I suggest using the built-in IP white/black list.

## Tile Encodings:

- Raw: Raw data is sent without any handling.

- ZLIB: Raw data compressed with zlib before sending.

## RichCursor Support:

-  Mouse cursor image sent to the client to let the client handle drawing of the mouse pointer. A custom image up to 64x64 pixels can also be chosen.

## Graphics Modes:

- Full TrueColor support.
- No handling of CLUT modes or pens.

	The server handles requests for parts or the full screen.
	It may ask for all the graphics or only the changed parts.

## Clipboard:

- Sending of clipboard text both ways is supported.
	A clipboard hook is installed on Amiga to detect Amiga+C.

## Mouse:

- Left, Middle, and Right mouse buttons are supported.
- Scroll wheel support.

## Keyboard:

- Keyboard checks for Cold and Icecold reboots.

## GUI, ARexx, and Commodity:

- Simple access to control the server.

## Missing:

Well, there are lots... like screen cycling, more tile encodings are two 
things high on the to-do list.
