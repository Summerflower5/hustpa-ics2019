#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(addi){
  rtl_addi(&id_dest->val, &id_src->val, id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template3(addi);
}

make_EHelper(auipc){
  rtl_addi(&id_dest->val, &cpu.pc, id_src->val);
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(auipc);
}

make_EHelper(jal){
  rtl_addi(&id_dest->val, &cpu.pc, 4);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  rtl_addi(&decinfo.jmp_pc, &cpu.pc, id_src->val);
  rtl_j(decinfo.jmp_pc);
  Log("before: 0x%-8x , after: 0x%-8x , x[rd]: 0x%-8x", cpu.pc, decinfo.jmp_pc, reg_l(id_dest->reg));
  print_asm_template2(jal);
}