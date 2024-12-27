#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].expr[0] = '\0';
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char *e){
  WP* cur = free_;

  if(cur == NULL){
    printf("No free watchpoint!\n");
    return NULL;
  }

  if(strlen(e) >= 65536){
    printf("Expr too long!\n");
    return NULL;
  }

  free_ = cur->next;
  cur->next = head;
  head = cur;

  strcpy(cur->expr, e);
  bool success = true;
  cur->value = expr(cur->expr, &success);

  if(!success){
    printf("Invalid watchpoint!\n");
    free_wp(cur->NO);
    return NULL;
  }

  return head;
}

void free_wp(int number){
  WP* cur = head;
  WP* pre = NULL;

  while(cur != NULL){
    if(cur->NO == number){
      if(pre == NULL){
        head = cur->next;
      }else {
        pre->next = cur->next;
      }
      cur->next = free_->next;
      free_ = cur;
      return ;
    }
    pre = cur;
    cur = cur->next;
  }
}

void display_watchpoints(){
  WP* cur = head;

  while(cur != NULL){
    printf("Watchpoint %d:\n", cur->NO);
    printf("  expr: %s\n", cur->expr);
    printf("  val:  %u\n", cur->value);
    cur = cur->next;
  }
}

bool check_watchpoint(){
  bool change = false;
  WP* cur = head;

  while(cur != NULL){
    bool success = true;
    uint32_t new_val = expr(cur->expr, &success);

    if(!success){
      printf("Check watchpoint unsuccess!\n");
    }else if(new_val != cur->value){
      printf("Watchpoint %d:\n",cur->NO);
      printf("  expr: %s\n",cur->expr);
      printf("  old_value: %u  ->  new_value: %u\n", cur->value , new_val);
      cur->value = new_val;
      change = true;
    }

    cur = cur->next;
  }
  
  return change;
}
