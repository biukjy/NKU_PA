#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char* args)
{
    if(free_==NULL)
	assert(0);
    WP *tmp=free_;
    free_=free_->next;
    tmp->next=NULL;

    bool success;
    strcpy(tmp->expr,args);
    tmp->last_value=expr(tmp->expr,&success);
    assert(success);

    if(head==NULL)
	head=tmp;
    else
    {
	WP *tmp1=head;
	while(tmp1->next!=NULL)
	    tmp1=tmp1->next;
	tmp1->next=tmp;
    }

    return tmp;
}


WP *delete_wp(int num,bool *success)
{
    WP *tmp=head;
    while(tmp!=NULL&&tmp->NO!=num)
        tmp=tmp->next;
    if(tmp==NULL)
        *success=false;
    return tmp;

}



void free_wp(WP* wp)
{
    if(wp==NULL)
    {
	printf("watchpoint is nullptr\n");
	assert(0);
    }
    if(wp==head)
	head=head->next;
    else
    {
	WP *tmp=head;
	while(tmp->next!=wp && tmp!=NULL)
	    tmp=tmp->next;
	tmp->next=tmp->next->next;
    }


    wp->next=free_;
    free_=wp;
    wp->expr[0]='\0';
    wp->last_value=0;

}


bool watch_wp()
{
    bool success;
    int value;
    WP *tmp=head;
    if(head==NULL)
	return false;
    while(tmp!=NULL)
    {
	value=expr(tmp->expr,&success);
	if(value!=tmp->last_value)
	{
	    printf("watchpoint  NO : %d  expr : %s\n",tmp->NO,tmp->expr);
	    printf("oldvalue: %d\t newvalue: %d\n",tmp->last_value,value);
            tmp->last_value=value;
            //tmp=tmp->next;
	    return true;
	}
	tmp=tmp->next;
    }
    return false;
}


void print_wp()
{
    if(head==NULL)
    {
	printf("No watchpoint now\n");
	return ;
    }

    WP *tmp=head;
    while(tmp!=NULL)
    {
	printf("watchpoint  NO : %d  expr : %s\n",tmp->NO,tmp->expr);
        tmp=tmp->next;
    }

}

