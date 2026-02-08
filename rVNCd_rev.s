VERSION = 1
REVISION = 45

.macro DATE
.ascii "08.02.2026"
.endm

.macro VERS
.ascii "rVNCd 1.45"
.endm

.macro VSTRING
.ascii "rVNCd 1.45 (08.02.2026)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: rVNCd 1.45 (08.02.2026)"
.byte 0
.endm
