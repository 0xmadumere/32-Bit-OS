// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define SEG(addr) ((uint16_t)(((uint32_t)(addr)) >> 4) & 0xFFFF)
#define OFF(addr) ((uint16_t)(((uint32_t)(addr)) & 0x000F))