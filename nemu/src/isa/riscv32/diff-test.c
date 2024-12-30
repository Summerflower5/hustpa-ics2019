#include "nemu.h"
#include "monitor/diff-test.h"

const char *reg_sl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void ref_isa_display(CPU_state *ref_r){
  printf("-----ref regs-----\n");
  for(int i = 0 ; i < 32 ; i++){
    printf("%-4s      0x%-8x      %-8d\n", reg_sl[i] , ref_r->gpr[i]._32 , ref_r->gpr[i]._32);
  }
  printf("-----dut regs-----\n");
}

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  ref_isa_display(ref_r);
  return false;
}

void isa_difftest_attach(void) {
}
