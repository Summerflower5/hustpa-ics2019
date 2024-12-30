#ifndef __NEMU_H__
#define __NEMU_H__

#include "common.h"
#include "memory/memory.h"
#ifdef RISCV
#include "isa/reg.h"
#else
#include "isa/riscv32/include/isa/reg.h"
#endif

extern CPU_state cpu;

#endif
