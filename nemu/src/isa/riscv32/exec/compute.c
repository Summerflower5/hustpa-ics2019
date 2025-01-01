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

make_EHelper(I_instr){
  switch (decinfo.isa.instr.funct3)
  {
  case 0b000:{  //addi
    rtl_addi(&id_dest->val, &id_src->val, id_src2->val);
    print_asm_template3(addi);
    break;
  }
  case 0b011:{  //sltiu
    rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
    print_asm_template3(sltiu);
    break;
  }
  case 0b100:{  //xori
    rtl_xori(&id_dest->val, &id_src->val, id_src2->val);
    print_asm_template3(xori);
    break;
  }
  case 0b101:{  //srai
    if(decinfo.isa.instr.funct7 == 0x20){ //srai
      rtl_sari(&id_dest->val, &id_src->val, id_src2->val & 0x1f);
      print_asm_template3(srai);
    }else {

    }
    break;
  }
  case 0b111:{  //andi
    rtl_andi(&id_dest->val, &id_src->val, id_src2->val);
    print_asm_template3(andi);
    break;
  }
  default:
    break;
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(auipc){
  rtl_addi(&id_dest->val, &cpu.pc, id_src->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template2(auipc);
}

make_EHelper(jal){
  rtl_addi(&id_dest->val, &cpu.pc, 4);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  rtl_addi(&decinfo.jmp_pc, &cpu.pc, id_src->val);
  Log("before: 0x%-8x , after: 0x%-8x , x[rd]: 0x%-8x", cpu.pc, decinfo.jmp_pc, reg_l(id_dest->reg));
  rtl_j(decinfo.jmp_pc);
  print_asm_template2(jal);
}

make_EHelper(jalr){
  rtl_addi(&id_dest->val, &cpu.pc, 4);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  rtl_addi(&decinfo.jmp_pc, &id_src->val, id_src2->val);
  decinfo.jmp_pc = decinfo.jmp_pc&(~1);
  Log("before: 0x%-8x , after: 0x%-8x , x[rd]: 0x%-8x", cpu.pc, decinfo.jmp_pc, reg_l(id_dest->reg));
  rtl_j(decinfo.jmp_pc);
  print_asm_template3(jalr);
}

make_EHelper(R_instr){
  switch (decinfo.isa.instr.funct3)
  {
  case 0b000:{  // add | sub | mul
    switch (decinfo.isa.instr.funct7)
    {
    case 0x0:{  // add
      rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(add);
      break;
    }
    case 0x20:{ // sub
      rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(sub);
      break;
    }
    
    default:
      break;
    }
    break;
  }
  case 0b001:{
    switch (decinfo.isa.instr.funct7)
    {
    case 0x0:{  //sll
      rtl_shl(&id_dest->val , &id_src->val , &id_src2->val);
      print_asm_template3(sll);
      break;
    }
    
    default:
      break;
    }
    break;
  }
  case 0b011:{  // sltu
    switch (decinfo.isa.instr.funct7)
    {
    case 0x0:{  // sltu
      rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(sltu);
      break;
    }
    
    default:
      break;
    }
    break;
  }
  case 0b100:{  // xor
    switch (decinfo.isa.instr.funct7)
    {
    case 0x0:{  // xor
      rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(xor);
      break;
    }
    
    default:
      break;
    }
    break;
  }
  case 0b101:{  // srl
    switch (decinfo.isa.instr.funct7)
    {
    case 0x0:{  //srl
      rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(srl);
      break;
    }
    
    default:
      break;
    }
    break;
  }
  case 0b110:{  // or
    switch (decinfo.isa.instr.funct7)
    {
    case 0x0:{  // or
      rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(or);
      break;
    }
    
    default:
      break;
    }
    break;
  }
  case 0b111:{  //and
    switch (decinfo.isa.instr.funct7)
    {
    case 0x0:{
      rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(and);
      break;
    }
    
    default:
      break;
    }
    break;
  }
  default:
    break;
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(B_instr){
  decinfo.jmp_pc = cpu.pc + id_dest->val;
  switch (decinfo.isa.instr.funct3)
  {
  case 0b000:{  // beq | beqz
    rtl_jrelop(RELOP_EQ, &id_src->val, &id_src2->val, decinfo.jmp_pc);
    print_asm_template3(beq);
    break;
  }
  case 0b001:{  // bne | bnez
    rtl_jrelop(RELOP_NE, &id_src->val, &id_src2->val, decinfo.jmp_pc);
    print_asm_template3(beq);
    break;
  }
  case 0b100:{  // blt
    rtl_jrelop(RELOP_LT, &id_src->val, &id_src2->val, decinfo.jmp_pc);
    print_asm_template3(blt);
    break;
  }
  case 0b101:{  // bge
    rtl_jrelop(RELOP_GE, &id_src->val, &id_src2->val, decinfo.jmp_pc);
    print_asm_template3(bge);
    break;
  }
  default:
    break;
  }
}