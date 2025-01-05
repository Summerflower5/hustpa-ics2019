#include "common.h"
#include "syscall.h"

void sys_yield();
void sys_exit(int code);
size_t sys_write(int fd, void* buf, size_t count);
int sys_brk(uint32_t addr);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:{
      sys_yield();
      c->GPRx = 0;
      break;
    }
    case SYS_exit:{
      sys_exit(a[1]);
      break;
    }
    case SYS_write:{
      c->GPRx = sys_write(a[1], a[2], a[3]);
      break;
    }
    case SYS_brk:{
      c->GPRx = sys_brk(a[1]);
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

void sys_yield(){
  _yield();
}

void sys_exit(int code){
  _halt(code);
}

size_t sys_write(int fd, void* buf, size_t count){
  if(fd == 1 || fd == 2){
    for(int i = 0 ; i < count ; i++){
      _putc(((char *)buf)[i]);
    }
    Log("sys_write is called");
    return count;
  }
  return 0;
}

int sys_brk(uint32_t addr){
  return 0;
}