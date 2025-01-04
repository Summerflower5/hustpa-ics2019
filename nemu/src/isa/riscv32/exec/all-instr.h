#include "cpu/exec.h"

// U
make_EHelper(lui);
make_EHelper(auipc);

//I
make_EHelper(addi);
make_EHelper(jalr);
make_EHelper(I_instr);

//J
make_EHelper(jal);

//R
make_EHelper(R_instr);

//B
make_EHelper(B_instr);

make_EHelper(ld);
make_EHelper(st);
make_EHelper(lh);
make_EHelper(lb);

make_EHelper(inv);
make_EHelper(nemu_trap);
