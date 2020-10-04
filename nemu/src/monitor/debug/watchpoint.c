#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char *args, int val){
	if(free_ == NULL){
	  assert(0);
	}
	WP *p = free_;
	free_ = free_->next;
	int len = strlen(args);
	if(len > 32){
		printf("error!\n");
		return NULL;
	}
	int i;
	for(i = 0;i < len;i++)
	p->str[i] = args[i];		//copy args to p->str;
	p->value = val;
	if(head == NULL){
		head = p;
		head->next = NULL;
	}
 	else{
		p->next = head->next;
		head->next = p;
	}
	return p;
}

void free_wp(WP *wp){
	if(wp->NO == head->NO)		//if wp is just head
		head = head->next;
	else{
		WP *p = head;
		while(p->next != NULL && (p->next->NO != wp->NO))
			p = p->next;
		if(p->next == NULL)
		printf("error\n");
		else{
			p->next = p->next->next;
		}
	}
	int i;
	for(i = 0;i < 32;i++)
		wp->str[i] = '\0';	//clean wp->str
	wp->value = 0;			//set wp->value
	wp->next = free_;		//Insert wp to free_
	free_ = wp;
}

bool Check_wp(){
	bool result = true;
	bool success = true;
	WP *p = head;
	while(p != NULL){
		int new_value = expr(p->str,&success);	//compute new value by function expr
		if(success == false)
			printf("expression error\n");
		if(new_value != p->value){
			result = false;
			printf("Watchpoint %d: %s\n",p->NO,p->str);
			printf("Old value is: %d\n",p->value);
			printf("New value is: %d\n",new_value);
			p->value = new_value;
		}
		p = p->next;
	}
	return result;
}

void Delete_wp(int order_number){
	WP *p = &wp_pool[order_number];
	free_wp(p);	
}

void info_wp(){
	WP *p = head;
	while(p != head){
		printf("Watchpoint %d: %s = %d\n",p->NO,p->str,p->value);
		p = p->next;
	}	
}
