#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(int i = 0 ; i < len ; i++){
    _putc(((const char*)buf)[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int key = read_key();
  if(key == _KEY_NONE){
    uint32_t time = uptime();
    len = sprintf(buf, "t %d\n", time);
    return len;
  }else {
    if(key & 0x8000){
      len = sprintf(buf, "kd %s\n", keyname[key & ~0x8000]);
    }else {
      len = sprintf(buf, "ku %s\n", keyname[key & ~0x8000]);
    }
  }
  return len;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //memcpy(buf, dispinfo + offset , len);
  len = sprintf(buf, dispinfo + offset);
  Log("dispinfo_read called here");
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  uint32_t y = (offset / 4) / screen_width();
  uint32_t x = (offset / 4) % screen_width();
  draw_rect((uint32_t *)buf, x, y, len / 4 , 1);
  Log("fb_write called here");
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  Log("fbsync_write called here");
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
