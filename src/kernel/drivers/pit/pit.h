// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once

#include <kernel/include/common.h>


void pit_init();
void pit_handler();
void pit_set_freq(uint32_t frequency);
uint32_t pit_get_tick();
uint32_t pit_get_freq(void);