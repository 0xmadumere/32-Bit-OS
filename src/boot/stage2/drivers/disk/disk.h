// Copyright (c) 2026 0xmadumere
// SPDX-License-Identifier: MIT

#pragma once
#include <boot/stage2/include/common.h>


int disk_read(uint32_t lba, uint16_t count, void* address);