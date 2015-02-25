#pragma once

#define ERROR(MSG) { \
  printf("Error: " MSG ", (%d, %d).", \
  now.lines.linf[now.lines.cno].lblno, \
  (now.lines.p - now.lines.buf + 1)); \
  exit(1); }
#define STRCMP(STR1, STR2) (!strncmp(STR1, STR2, MAX_TOKEN_LEN))

#define MAX_TOKEN_LEN 64
#define MAX_LINE_LEN 256
#define MAX_LINE_COUNT 4096
#define MAX_VAR_COUNT 64
#define MAX_NESTED_FOR 32
#define MAX_SUB_DEPTH 32
#define MAX_STACK_LEN 32

namespace TOKEN_TYPE
{
enum
{
  INVALID,
  INTEGER,
  STRING,
  IDENTIFIER,
  LPAREN,   // '('
  RPAREN,   // ')'
  BLPAREN,  // '{'
  BRPAREN,  // '}'
  EQU,      // '='
  NEQ,      // '<>'
  BT,       // '>'
  ST,       // '<'
  BTE,      // '>='
  STE,      // '<='
  ADD,      // '+'
  SUB,      // '-'
  MUL,      // '*'
  DIV,      // '/'
  AND,      // LOGICAL AND
  OR,       // LOGICAL OR
  NOT,      // LOGICAL NOT
  TRUE,
  FALSE,
  FOR,
  TO,
  DOWNTO,
  NEXT,
  GOSUB,
  RETURN,
  GOTO,
  IF,
  THEN,
  INPUT,
  PRINT,
  PRINTLN,
  LET,
  REM,
  END,
  PUSH,
  POP
};
}

struct Token
{
  int type;
  char str[MAX_TOKEN_LEN];
};

struct VariableInfo
{
  char* name;
  int value;
};

struct LineInfo
{
  int lblno;
  int offset;
};

struct ForInfo
{
  int var;
  int dir;
  int mm;
  // The index of the line where the body begins.
  int bi;
};

struct SubInfo
{
  // The index of the line where the sub returns.
  int ri;
};

struct Session
{
  // File.
  FILE* fp;
  // Line.
  struct
  {
    LineInfo linf[MAX_LINE_COUNT];
    char buf[MAX_LINE_LEN];
    char* p;
    char* l; // last p used in ungettoken
    int cno;
    int cnt;
  } lines;
  // Variable.
  struct
  {
    VariableInfo inf[MAX_VAR_COUNT];
    int cnt;
  } vars;
  // For Clause.
  struct
  {
    ForInfo inf[MAX_NESTED_FOR];
    int top;
  } fors;
  // Sub Clause.
  struct
  {
    SubInfo inf[MAX_SUB_DEPTH];
    int top;
  } subs;
  // Stack.
  struct
  {
    int base[MAX_STACK_LEN];
    int top;
  } stk;
  // Token.
  Token token;
};

extern Session now;

void interpret(const char* fn);
void ungettoken();
void gettoken();
int getline();
void parselblno();
void program();
void statement();
void jump(int lblno);
int expression();
int term();
int item();
int boolexpr();
int boolterm();
int boolitem();
int varindex(const char* name);