#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  /* TODO: Add more token types */
  TK_NEQ,
  TK_HEXNUM,
  TK_NUM,
  TK_REG,
  TK_AND,
  TK_OR,
  TK_NEGATIVE,  //fuhao
  TK_DEREF      //zhizhen

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0[xX][0-9A-Fa-f]+", TK_HEXNUM},
  {"0|[1-9][0-9]*", TK_NUM},
  /*{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip|ax|cx|dx|bx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh|)", TK_REG};*/
  {"\\$[a-zA-Z]+", TK_REG},
  {"\\(", '('},
  {"\\)", ')'},
  {"\\+", '+'},         // plus
  {"-", '-'},
  {"\\*", '*'},
  {"/", '/'},
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},
  {"&&", TK_AND},
  {"\\|\\|",TK_OR},
  {"!",'!'}
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

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
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

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
	  case TK_NOTYPE:
	      break;
	  case TK_NUM:
	  case TK_HEXNUM:
	  case TK_REG:
	      assert(substr_len<32);
	      strncpy(tokens[nr_token].str,substr_start,substr_len);
	      tokens[nr_token].str[substr_len]='\0';
	  default:
	      tokens[nr_token].type=rules[i].token_type;
	      nr_token++;
	      break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

//check '('')' match 
bool check_parentheses(int p,int q)
{
    if(tokens[p].type!='('||tokens[q].type!=')')
    {
	return false;

    }
    int count=0;
    for(int i=p;i<q+1;i++)
    {
	if(tokens[i].type=='(') { count++; }
	else if(tokens[i].type==')')
	{
	    if(count!=0) { count--; }
	    else { return false; }
	}
    }
    if(count==0)
	return true;
    else
	return false;
}


int op_priority(int i)
{
    switch(tokens[i].type)
    {
	case TK_AND:
	case TK_OR:
	    return 0;
	case TK_EQ:
	case TK_NEQ:
	    return 1;
	case '+':
	case '-':
	    return 2;
	case '*':
	case '/':
	    return 3;
	case TK_DEREF:
	case TK_NEGATIVE:
	case '!':
	    return 4;
	default:
	    return 100;
    }
    
}


int find_dominant_op(int p,int q)
{
    int operator=p;
    for(int i=p;i<q+1;i++)
    {
	if(tokens[i].type!=TK_NUM&&tokens[i].type!=TK_HEXNUM&&tokens[i].type!=TK_REG&&tokens[i].type!='('&&tokens[i].type!=')')
	{
	    if(op_priority(i)<=op_priority(operator))
		operator=i;
	}
	else if(tokens[i].type=='(')
	{
	    int count=1;
	    while(count!=0&&++i!=q)
	    {
		if(tokens[i].type=='(')
		    count++;
		else if(tokens[i].type==')')
		    count--;
	    }
	}
    }

    return operator;

}

// uint32_t
int eval(int p, int q)
{
    if(p>q)
    {
	printf("error:p>q in eval(),p=%d,q=%d\n",p,q);
	assert(0);
    }
    else if(p==q)
    {
	int num;
	switch(tokens[p].type)
	{
	    case TK_NUM:
	        sscanf(tokens[p].str,"%d",&num);
		return num;
	    case TK_HEXNUM:
	        sscanf(tokens[p].str,"%x",&num);
		return num;
	    case TK_REG:
	        for(int i=0;i<8;i++)
		{
		    if(strcmp(tokens[p].str+1,regsl[i])==0)
			return reg_l(i);
                    if(strcmp(tokens[p].str+1,regsw[i])==0)
			return reg_w(i);
                    if(strcmp(tokens[p].str+1,regsb[i])==0)
			return reg_b(i);
		}
                if(strcmp(tokens[p].str,"$eip")==0)
                    return cpu.eip;
		else 
		{
		    printf("error in TK_REG --eval()\n");
		    assert(0);
		}
	}
    }
    else if(check_parentheses(p,q))
    {
	return eval(p+1,q-1);
    }
    else
    {
	int op=find_dominant_op(p,q);
	vaddr_t addr;
	int res;
	switch(tokens[op].type)
	{
	    case TK_NEGATIVE:
	        return -eval(p+1,q);
	    case TK_DEREF:
	        addr=eval(p+1,q);
		res=vaddr_read(addr,4);
		printf("addr:0x%x   value:0x%08x\n",addr,res);
		return res;
	    case '!':
	        res=eval(p+1,q);
		if(res!=0)
		    return 0;
		else
		    return 1;
	}
	int val1=eval(p,op-1);
	int val2=eval(op+1,q);
	switch(tokens[op].type)
	{
	    case '+':
	        return val1+val2;
	    case '-':
	        return val1-val2;
	    case '*':
	        return val1*val2;
	    case '/':
	        return val1/val2;
	    case TK_EQ:
	        return val1==val2;
	    case TK_NEQ:
	        return val1!=val2;
	    case TK_AND:
	        return val1&&val2;
	    case TK_OR:
	        return val1||val2;
	    default:
	        assert(0);
	}

    }
    return 0;
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  if(tokens[0].type=='-')
      tokens[0].type=TK_NEGATIVE;
  if(tokens[0].type=='*')
      tokens[0].type=TK_DEREF;
  for(int i=1;i<nr_token;i++)
  {
      if(tokens[i].type=='-')
      {
	  if(tokens[i-1].type!=TK_NUM&&tokens[i-1].type!=')')
	      tokens[i].type=TK_NEGATIVE;
      }
      if(tokens[i].type=='*')
      {
	  if(tokens[i-1].type!=TK_NUM&&tokens[i-1].type!=')')
	      tokens[i].type=TK_DEREF;
      }

  }


  *success=true;
  return eval(0,nr_token-1);

}


