VERSION = 1
REVISION = 41

.macro DATE
.ascii "29.7.2025"
.endm

.macro VERS
.ascii "rVNCd 1.41"
.endm

.macro VSTRING
.ascii "rVNCd 1.41 (29.7.2025)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: rVNCd 1.41 (29.7.2025)"
.byte 0
.endm
