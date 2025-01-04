#include "cpu/exec.h"

extern void raise_intr(uint32_t NO, vaddr_t epc);

inline uint32_t read_csr(uint32_t csr){
    switch (csr)
    {
    case 0x100: return decinfo.isa.sstatus; 
    case 0x105: return decinfo.isa.stevc;
    case 0x141: return decinfo.isa.sepc;
    case 0x142: return decinfo.isa.scause;
    default:
        assert(0);
    }
}

inline void write_csr(uint32_t csr, uint32_t val){
    switch (csr)
    {
    case 0x100:{
        decinfo.isa.sstatus = val;
        break;
    }
    case 0x105:{
        decinfo.isa.stevc = val;
        break;
    }
    case 0x141:{
        decinfo.isa.sepc = val;
        break;
    }
    case 0x142:{
        decinfo.isa.scause = val;
        break;
    }
    default:
        break;
    }
}

make_EHelper(csr){
    switch (decinfo.isa.instr.funct3)
    {
    case 0b000:{    //ecall
        raise_intr(reg_l(17), decinfo.seq_pc-4);
        break;
    }
    case 0b001:{    //csrrw
        s0 = read_csr(decinfo.isa.instr.csr);
        write_csr(decinfo.isa.instr.csr, id_src->val);
        rtl_sr(id_dest->reg, &s0, 4);
        break;
    }
    case 0b010:{    //csrrs
        s0 = read_csr(decinfo.isa.instr.csr);
        write_csr(decinfo.isa.instr.csr, s0 | id_src->val);
        rtl_sr(id_dest->reg, &s0, 4);
        break;
    }
    default:
        break;
    }
}