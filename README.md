# rVNCd – Amiga VNC Server

rVNCd is a native VNC server for **AmigaOS 4**, with ongoing work to support
AmigaOS 3 and multiple graphics systems.

The goal of rVNCd is to provide a fast, flexible, and Amiga-friendly VNC
implementation with minimal overhead and good compatibility with existing
VNC clients.

---

## Supported VNC Protocols

- VNC protocol **v3.3**
- VNC protocol **v3.7**

---

## Authentication

- **None**
  - No authentication required

- **d3des**
  - Weak 8-byte DES-based password (VNC compatible)
  - It is recommended to use the built-in **IP white/black list** for security

---

## Supported Encodings

- **Raw**
  - Uncompressed raw pixel data

- **ZLIB**
  - Raw data compressed using zlib

- **RRE** *(added in v1.42)*
  - Efficient for areas with large regions of the same color  
    (e.g. Shell windows, solid Workbench backdrops)

- **LastRect** *(added in v1.42)*
  - Used to properly terminate framebuffer updates when supported by the client

---

## Cursor Support

- **RichCursor**
  - Cursor image is sent to the client
  - Client handles cursor drawing
  - Custom cursor image up to **64×64 pixels** supported

---

## Graphics Support

- Full **TrueColor** support
- No support for CLUT / pen-based screen modes

The server supports:
- Full screen updates
- Partial updates (only changed areas)

Tile comparison and update logic has been optimized to reduce bandwidth usage.

---

## Clipboard Support

- Clipboard text transfer **both directions**
- Uses an Amiga clipboard hook
- Detects **Amiga + C** automatically

---

## Mouse Support

- Left, Middle, Right mouse buttons
- Scroll wheel support

---

## Keyboard Handling

- Keyboard input supported
- Detects **Cold** and **IceCold** reboot key sequences

---

## Audio / Beep Support

- **Bell / Beep forwarding** *(added in v1.42)*
- Intuition’s `DisplayBeep()` is patched so beeps are sent to the VNC client

---

## GUI, ARexx and Commodity

- Simple GUI for controlling the server
- ARexx interface
- Commodity support for easy access and control

---
