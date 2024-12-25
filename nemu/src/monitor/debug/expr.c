#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_AND,
  TK_OR,
  TK_NOT,
  TK_NEQ,
  TK_NUM,
  TK_HEX_NUM,
  TK_REG,
  TK_NEG,
  TK_POINT

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"\\*", '*'},         // multiply
  {"\\/", '/'},         // divide
  {"\\-", '-'},         // sub
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"&&", TK_AND},       // and
  {"\\|\\|", TK_OR},    // or
  {"!=", TK_NEQ},       // not equal
  {"0[xX][0-9a-fA-F]+", TK_HEX_NUM}, // hex_num
  {"[0-9]+", TK_NUM},   // num
  {"\\$[a-z]+[0-9]*", TK_REG},  // reg
  {"!", TK_NOT},        // not
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

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

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

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
          case TK_NUM:{
            Assert(substr_len < 32, "TK_NUM: substr length too long!");
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str , substr_start , substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          }
          case TK_HEX_NUM:{
            Assert(substr_len < 34, "TK_HEX_NUM: substr length too long!");
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str , substr_start + 2 , substr_len - 2);
            tokens[nr_token].str[substr_len - 2] = '\0';
            nr_token++;
            break;
          }
          case TK_REG:{
            Assert(substr_len < 32, "TK_NUM: substr length too long!");
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str , substr_start + 1 , substr_len - 1);
            tokens[nr_token].str[substr_len - 1] = '\0';
            nr_token++;
            break;
          }
          case '(':
          case ')':
          case '*':
          case '/':
          case '+':
          case '-':
          case TK_EQ:
          case TK_AND:
          case TK_OR:
          case TK_NEQ:
          case TK_NOT:
          default:{
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
          }
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

bool check_parentheses(int start, int end){
  if(tokens[start].type != '(' || tokens[end].type != ')'){
    return false;
  }
  int res = 0;
  for(int i = start+1 ; i < end ; i++){
    if(tokens[i].type == '('){
      res++;
    }else if(tokens[i].type == ')'){
      res--;
    }

    if(res < 0){
      return false;
    }
  }
  return res == 0;
}

int find_main_op(int start, int end){
  int pos = -1;
  int res = 0;
  int priority = 0;

  for(int i = start ; i <= end ; i++){
    int cur_priority = -1;
    if(tokens[i].type == '('){
      res++;
    }else if(tokens[i].type == ')'){
      res--;
    }
    if(res != 0){
      continue;
    }

    switch (tokens[i].type)
    {
    case TK_NOT:{
      cur_priority = 2;
      break;
    }
    case TK_NEG:{
      cur_priority = 2;
      break;
    }
    case TK_POINT:{
      cur_priority = 2;
    }
    case '*':
    case '/':{
      cur_priority = 3;
      break;
    }
    case '+':
    case '-':{
      cur_priority = 4;
      break;
    }
    case TK_EQ:
    case TK_NEQ:{
      cur_priority = 7;
      break;
    }
    case TK_AND:{
      cur_priority = 11;
      break;
    }
    case TK_OR:{
      cur_priority = 12;
      break;
    }
    default:
      break;
    }

    if(priority <= cur_priority){
      priority = cur_priority;
      pos = i;
    }
  }

  Assert((pos != -1 && priority != 0) , "cannot find main operator!");
  return pos;
}

uint32_t eval(int start, int end, bool *success){
  if(start > end){
    printf("start > end , invalid expr!\n");
  }

  if(start == end){
    switch (tokens[start].type)
    {
    case TK_NUM:{
      return (uint32_t)strtol(tokens[start].str , NULL , 10);
    }
    case TK_HEX_NUM:{
      return (uint32_t)strtol(tokens[start].str , NULL , 16);
    }
    case TK_REG:{
      return isa_reg_str2val(tokens[start].str , success);
    }
    default:
      printf("single token with no value!\n");
      return 0;
    }
  }else if(check_parentheses(start , end)){
    return eval(start+1 , end-1 , success);
  }else {
    int pos = find_main_op(start , end);

    if(pos == start || tokens[pos].type == TK_NOT || tokens[pos].type == TK_POINT || tokens[pos].type == TK_NEG){
      uint32_t r_val = eval(pos+1 , end , success);
      switch (tokens[pos].type)
      {
      case TK_NOT:{
        return !r_val;
      }
      case TK_NEG:{
        return -r_val;
      }
      case TK_POINT:{
        return paddr_read(r_val, 4);
      }
      default:
        printf("invalid unary operator\n");
        return 0;
      }
    }

    uint32_t l_val = eval(start , pos-1 , success);
    uint32_t r_val = eval(pos+1 , end , success);

    switch (tokens[pos].type)
    {
    case '+': return l_val + r_val;
    case '-': return l_val - r_val;
    case '*': return l_val * r_val;
    case '/': {
      Assert(r_val != 0 , "Division by zero error!");
      if(r_val == 0){
        printf("Division by zero error!\n");
        return 0;
      }
      return l_val / r_val;
    }
    case TK_EQ: return l_val == r_val;
    case TK_NEQ: return l_val != r_val;
    case TK_AND: return l_val && r_val;
    case TK_OR: return l_val || r_val;

    default:
      printf("invalid operator!\n");
      return 0;
    }
  }
  *success = false;
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
