#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

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

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_d(char *args);

static int cmd_w(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
        { "si", "Single Step", cmd_si},
        { "info", "Print the state of CPU or monitor", cmd_info},
        { "x", "read memory", cmd_x},
        { "p", "compute expressions", cmd_p},
	{ "d", "delete watchpoint, d N means delete watchpoint whose number is N",cmd_d},
	{ "w", "set watchpoint following a expression", cmd_w}

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args)
{
     if(args == NULL)
       {
          cpu_exec(1);   
       }
     else{
     uint32_t n = 0;
     while(*args != '\0')
     {
          while(*args >= '0' && *args <= '9')
          {
              n = n*10 + (*args - '0');
              args++;
          }
     } 
     cpu_exec(n);
    }
     return 0;
}

static int cmd_info(char *args)
{
  if(args != NULL){
    const char *str[8] = {"eax","ecx","edx","ebx","esp","ebp","esi","edi"};
    if(strcmp(args,"r") == 0){
      int i;
      for(i = 0;i < 8;i++){
         printf("%s\t0x%08x %d\n",str[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
      }
    printf("eip\t0x%08x %d\n",cpu.eip,cpu.eip);
    }
    else if(strcmp(args,"w") == 0){
	info_wp();			//info_wp is declared in watchpoint.c
  }
  return 0;
  }
	else{
		printf("argument needed");
		return 0;
	}
}

static int cmd_x(char *args)
{
  char *N = strtok(NULL, " ");
  uint32_t n = 0;
  sscanf(N,"%d",&n);
  char *address = strtok(NULL, " ");
  swaddr_t addr = 0;
  sscanf(address,"0x%x",&addr);
  int i;
  for(i = 0;i < n;i++)
  {
     if(i % 4 == 0)
     printf("0x%08x: ",addr + 4*i);
     printf("0x%08x ", swaddr_read(addr + 4*i, 4));
     if((i + 1) % 4 == 0)
     printf("\n");
  }
  if(n % 4 != 0)
  printf("\n");

  return 0;
}

static int cmd_p(char *args)
{
	uint32_t result = 0;
	bool success = true;
	result = expr(args,&success);
	if(success == 0)
	printf("Invalid expression %s\n",args);
	else 
        printf("result = %d\n",result);
	return 0;
}

static int cmd_d(char *args){
	int number;
	sscanf(args,"%d",&number);
	Delete_wp(number);
	return 0;
}

static int cmd_w(char *args){
	bool success = true;
	int init_value = expr(args,&success);
	WP *p = new_wp(args,init_value);
	printf("Watchpoint %d: %s\n",p->NO,p->str);
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
