VERSION = 1
REVISION = 44

.macro DATE
.ascii "14.12.2025"
.endm

.macro VERS
.ascii "rVNCd 1.44"
.endm

.macro VSTRING
.ascii "rVNCd 1.44 (14.12.2025)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: rVNCd 1.44 (14.12.2025)"
.byte 0
.endm
