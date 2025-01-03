#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int _i2a(int num, char buf[32], int radix){
  static const char s[] = "0123456789abcdef";
  char *dst = buf;
  int n = num , R = radix;
  if(n < 0){
    *dst++ = '-';
    n = -num;
  } 
  if(n < 10){
    *dst++ = s[n];
    *dst = '\0';
  }else {
    char tmp[32];
    char *p = tmp;
    while(n){
      *p++ = s[n % R];
      n = n / R;
    }
    while(p != tmp){
      --p;
      *dst++ = *p;
    }
    *dst = '\0';
  }
  return dst - buf;
}

int _itoa(int num, char buf[32]){
  return _i2a(num, buf, 10);
}

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  size_t pos = 0;
  int format = 0;

  for(; *fmt ; fmt++){
    if(format){
      switch (*fmt)
      {
      case 'd':{
        char buf[32];
        int n = _itoa(va_arg(ap, int), buf);
        memcpy(out+pos, buf, n);
        pos += n;
        format = 0;
        break;
      }
      case 's':{
        const char *s2 = va_arg(ap, char*);
        int n = strlen(s2);
        memcpy(out+pos, s2, n);
        pos += n;
        format = 0; // ignored
        break;
      }
      default:
        break;
      }
    }else if(*fmt == '%'){
      format = 1;
    }else {
      out[pos++] = *fmt;
    }
  }
  out[pos] = '\0';
  return pos;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  int length = vsprintf(out, fmt, vl);
  va_end(vl);
  return length;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
