/*
 * This file is part of the Sofa project
 * Copyright (c) 2018 Manuel Deneu.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "Sofa.h"


#ifndef SOFA_TESTS_ONLY
#include <sel4/types.h>

#else
#include <stdint.h>
typedef void time_manager_t;
typedef int (*timeout_cb_fn_t)(uintptr_t token);
#endif

#include "Bootstrap.h"
#include <stdint.h>

typedef struct _Thread Thread;

int TimerInit(KernelTaskContext* ctx , seL4_CPtr notifCap);

int TimerAllocAndRegister(time_manager_t *tm , uint64_t period_ns, uint64_t start, uint32_t id, timeout_cb_fn_t callback, uintptr_t token);

int TimerAllocAndRegisterOneShot(time_manager_t *tm , uint64_t rel_ns, uint32_t id,  timeout_cb_fn_t callback, uintptr_t token);

OSError TimerCancelFromThread( Thread* thread) NO_NULL_POINTERS;

uint64_t GetCurrentTime(void);


