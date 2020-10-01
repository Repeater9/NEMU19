#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, PLUS, LP, RP, DIVIDE, SUBTRACT, MULTIPLY, DECIMAL

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", PLUS},					// plus
	{"==", EQ},					// equal
        {"\\(", LP},					// left parenthese
	{"\\)", RP},					// right parenthese
	{"/", DIVIDE},					// divide
	{"-", SUBTRACT},				// subtract
	{"\\*", MULTIPLY},				// multiply
	{"[0-9]", DECIMAL}				// decimal integer
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case DECIMAL: tokens[nr_token].type = rules[i].token_type;
					if(substr_len > 32)
					printf("error");
					else{
						int j;
						for(j = 0;j < substr_len;j++){
							tokens[nr_token].str[j] = substr_start[j];
						}
					}		
					break;
					case NOTYPE: nr_token--; break;
					default:tokens[nr_token].type = rules[i].token_type; 
						//panic("please implement me");
				}
				nr_token++;

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

int check_parentheses(int p, int q, bool *success){
	int result = 0;
	*success = 0;
	int judge[40] = {0,};
	if(tokens[p].type == LP && tokens[q].type == RP){
		int i; int n = 0;
		for(i = p + 1;i <= q - 1;i++){
			if(tokens[i].type == LP || tokens[i].type == RP){
				judge[n] = tokens[i].type;
				if(n > 0 && (judge[n] == RP && judge[n - 1] == LP)){
					judge[n - 1] = judge[n] = 0;
					n = n - 2;
				}
				n++;
			}
		}
		if(judge[0] == 0){
			result = 1;
			*success = 1;
		}
	}
	return result;
}

int Find_DominantOp(int p,int q){
	int op = -1;
	int i;
	int nr_p = 0;
	int min_rank = 3;
	for(i = q;i >= p;i--){
	   if(tokens[i].type == RP) nr_p++;
           if(tokens[i].type == LP) nr_p--;
	   if(nr_p == 0 && (tokens[i].type == MULTIPLY || tokens[i].type == DIVIDE) && min_rank > 2){			op = i;
		   min_rank = 2;  
           }
	   if(nr_p == 0 && (tokens[i].type == PLUS || tokens[i].type == SUBTRACT) && min_rank > 1){
		   op = i;
		   min_rank = 1;
           }
	}
	return op;
}

uint32_t eval(int p, int q, bool *success){
	if(*success == 0) return 0;
	if(p > q){
		printf("Bad expression");
		return 0;
	}
	else if(p == q){
		if(tokens[p].type != DECIMAL){
			printf("error");
			return 0;
		}
		else{
			uint32_t  n;
			sscanf(tokens[p].str,"%u",&n);
			return n;
		}
	}
	else if(check_parentheses(p,q,success) == true){
		return eval(p + 1,q - 1,success);
	}
	else{
		int op = Find_DominantOp(p,q);
		int value1 = eval(p,op - 1,success);
		int value2 = eval(op + 1,q,success);
		int op_type = tokens[op].type;
		switch(op_type){
			case PLUS : return value1 + value2; break;
			case SUBTRACT : return value1 - value2; break;
			case MULTIPLY: return value1*value2; break;
			case DIVIDE: return value1/value2; break;
			default: assert(0); return 0;
		}
	   }
}
		
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	uint32_t result = 0;
	result = eval(0,nr_token - 1,success);
	//panic("please implement me");
	return result;
}

