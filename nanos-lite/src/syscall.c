#include "common.h"
#include "syscall.h"
#include "fs.h"

void sys_yield();
void sys_exit(int code);
size_t sys_write(int fd, void* buf, size_t count);
int sys_brk(uint32_t addr);
int sys_open(const char *pathname, int flags, int mode);
size_t sys_read(int fd, void *buf, size_t len);
size_t sys_lseek(int fd, size_t offset, int whence);
int sys_close(int fd);
int sys_execve(const char *fname, char * const argv[], char *const envp[]);

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
    case SYS_read:{
      c->GPRx = sys_read(a[1], a[2], a[3]);
      break;
    }
    case SYS_open:{
      c->GPRx = sys_open(a[1], a[2], a[3]);
      break;
    }
    case SYS_lseek:{
      c->GPRx = sys_lseek(a[1], a[2], a[3]);
      break;
    }
    case SYS_close:{
      c->GPRx = sys_close(a[1]);
      break;
    }
    case SYS_execve:{
      sys_execve(a[1], a[2], a[3]);
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
  sys_execve("/bin/init", NULL, NULL);
}

size_t sys_write(int fd, void* buf, size_t count){
  /*if(fd == 1 || fd == 2){
    for(int i = 0 ; i < count ; i++){
      _putc(((char *)buf)[i]);
    }
    Log("sys_write is called");
    return count;
  }
  return 0;*/
  return fs_write(fd, buf, count);
}

int sys_brk(uint32_t addr){
  return 0;
}

int sys_open(const char *pathname, int flags, int mode){
  return fs_open(pathname, flags, mode);
}

size_t sys_read(int fd, void *buf, size_t len){
  return fs_read(fd, buf, len);
}

size_t sys_lseek(int fd, size_t offset, int whence){
  return fs_lseek(fd, offset, whence);
}

int sys_close(int fd){
  return fs_close(fd);
}

int sys_execve(const char *fname, char * const argv[], char *const envp[]){
  naive_uload(NULL, fname);
  return 0;
}