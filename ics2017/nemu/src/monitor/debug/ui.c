#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  int n; // uint64_t ?????
  if(args==NULL)
  {
      cpu_exec(1);
      return 0;
  }
  else
  {
      int flag1=sscanf(args,"%d",&n);
      if(flag1<=0)
      {
	  printf("error args int cmd_si\n");
	  return 0;
      }	  
  }
  cpu_exec(n);
  return 0;

}

static int cmd_info(char *args)
{
    char *arg=strtok(args,"");

    if(strcmp(arg,"r")==0)
    {
	for(int i=0;i<8;i++)
	{
	    printf("%s\t0x%x\n",regsl[i],reg_l(i));
	}
	printf("eip\t0x%x\n",cpu.eip);
	
	for(int i=0;i<8;i++)
	{
	    printf("%s\t0x%x\n",regsw[i],reg_w(i));
	}

	for(int i=0;i<8;i++)
	{
	    printf("%s\t0x%x\n",regsb[i],reg_b(i));
	}
    }
    else if(strcmp(arg,"w")==0)
    {
	print_wp();
    }
    else
    {
	printf("error args in cmd_info()\n");

    }
    return 0;
    
}


static int cmd_p(char *args)
{
    bool success;
    int res=expr(args,&success);
    if(success)
	printf("the value of expr:%d\n",res);
    else
	printf("expression error in cmp_p()\n");

    return 0;
}



static int cmd_x(char *args)
{
    char *arg=strtok(args," ");
    char *exprs=strtok(NULL," ");
    int n;
    sscanf(arg,"%d",&n);
    bool flag1=1;
    //bool flag1;
    uint32_t addr=expr(exprs,&flag1);

    if(!flag1)
    {
	printf("wrong expression in cmd_x()\n");
    }

    if(n<0)
    {
	printf("error args in cmd_x()\n");
    }

    for(int i=0;i<n;i++)
    {
	uint32_t tmp=vaddr_read(addr,4);
	printf("0x%x: 0x%08x\n",addr,tmp);
	addr+=4;
    }

    return 0;

}


static int cmd_w(char *args)
{
    WP *tmp=new_wp(args);
    printf("success: set watchpoint %d,expr:%s\n",tmp->NO,tmp->expr);
    return 0;
}


static int cmd_d(char *args)
{
    int num=0;
    sscanf(args,"%d",&num);
    bool success=true;
    WP *tmp=delete_wp(num,&success);
    if(!success)
    {
	printf("watchpoint %d error\n",num);
    }
    else
    {
	printf("delete watchpoint %d : %s\n",tmp->NO,tmp->expr);
	free_wp(tmp);
    }
    
    return 0;
}



static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {"si","'si [N=1]',Step through N instructions",cmd_si},
  {"info","'info <r|w>',Print reg and watchpoints",cmd_info},
  {"x","'x <N> <EXPR>', Dump N 4-bytes start memory address <EXPR>.",cmd_x},
  {"p","'p <EXPR>',calculate the expression <EXPR>",cmd_p},
  {"w","'w <EXPR>',Set new watchpoint <EXPR>",cmd_w},
  {"d","'d <N>',Delete the watchpoint with sequence number N",cmd_d},

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
