#include "cpu/exec.h"

// U
make_EHelper(lui);
make_EHelper(auipc);

//I
make_EHelper(addi);
make_EHelper(jalr);

//J
make_EHelper(jal);

make_EHelper(ld);
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);
