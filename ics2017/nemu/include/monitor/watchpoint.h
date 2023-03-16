#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[32];
  int last_value;

} WP;


WP* new_wp(char* args);
void free_wp(WP* wp);
WP *delete_wp(int num,bool *success);

void print_wp();
bool watch_wp();


#endif
