#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;						//the order number of watchpoint
	struct watchpoint *next;
	char str[32];					//save the expression
	int value;
	/* TODO: Add more members if necessary */


} WP;

WP* new_wp(char *args,int val);

void free_wp(WP *wp);

bool Check_wp();

void Delete_wp(int order_number);

void info_wp();
#endif
