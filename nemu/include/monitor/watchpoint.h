#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[65536];
  uint32_t value;

} WP;

WP* new_wp(char *e);
void free_wp(int number);
void display_watchpoints();
bool check_watchpoint();

#endif
