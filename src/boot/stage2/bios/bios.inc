; Copyright (c) 2026 0xmadumere
; SPDX-License-Identifier: MIT

struc DAP
    .size               resb 1
    .reserved           resb 1
    .num_sectors        resw 1
    .buffer_offset      resw 1
    .buffer_segment     resw 1
    .lba_low            resd 1
    .lba_high           resd 1
endstruc

struc CHS
    .cylinder           resb 1
    .head               resb 1
    .sector             resb 1
    .count              resb 1
    .segment            resw 1
    .offset             resw 1
endstruc