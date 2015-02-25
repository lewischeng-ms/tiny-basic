#include "stdafx.h"
#include "basic.h"

void parselblno()
{
  // Get the label number.
  gettoken();
  if (now.token.type != TOKEN_TYPE::INTEGER)
    ERROR("Missing label number")
  int val = atoi(now.token.str);
  if (val <= 0)
    ERROR("Label number must be positive")
  now.lines.linf[now.lines.cno].lblno = val;
}

// Parse program.
void program()
{
  while (getline())
  {
    // Execute current line.
    statement();
    // There's remnant codes left in the line.
    if (*now.lines.p)
      ERROR("Invalid token after the statement");
    // The user called END to terminate the program.
    if (!now.subs.top)
      return;
  }
  // FOR stack's not empty, so users may forget to put NEXTs corresponding to their FORs.
  if (now.fors.top)
    ERROR("NEXT expected to match FOR clause")
  if (now.subs.top)
    ERROR("RETURN or END expected to match GOSUB clause")
}

// Parse statement.
void statement()
{
  int val;
  int vi;
  int i;
  // Parse label number.
  parselblno();
  // Get the statement keyword.
  gettoken();
  if (now.token.type == TOKEN_TYPE::LET)
  { // LET
    // Variable.
    gettoken();
    if (now.token.type != TOKEN_TYPE::IDENTIFIER)
      ERROR("Identifier expected")
    vi = varindex(now.token.str);
    // '='
    gettoken();
    if (now.token.type != TOKEN_TYPE::EQU)
      ERROR("'=' expected")
    // Expression.
    val = expression();
    // Make real assignment.
    now.vars.inf[vi].value = val;
  }
  else if (now.token.type == TOKEN_TYPE::IDENTIFIER)
  { // The same function as above LET. It's an assignment.
    vi = varindex(now.token.str);
    // '='
    gettoken();
    if (now.token.type != TOKEN_TYPE::EQU)
      ERROR("'=' expected")
    // Expression.
    val = expression();
    // Make real assignment.
    now.vars.inf[vi].value = val;
  }
  else if (now.token.type == TOKEN_TYPE::INPUT)
  { // INPUT
    gettoken();
    if (now.token.type != TOKEN_TYPE::IDENTIFIER)
      ERROR("Variable expected")
    vi = varindex(now.token.str);
    scanf("%d", &now.vars.inf[vi].value);
  }
  else if (now.token.type == TOKEN_TYPE::PRINT)
  { // PRINT
    gettoken();
    if (now.token.type == TOKEN_TYPE::STRING)
    // Print a string.
      printf("%s", now.token.str);
    else
    { // Print an expression.
      ungettoken();
      val = expression();
      printf("%d", val);
    }
  }
  else if (now.token.type == TOKEN_TYPE::PRINTLN)
  { // PRINTLN
    gettoken();
    if (now.token.type == TOKEN_TYPE::STRING)
    // Print a string.
      printf("%s\n", now.token.str);
    else
    { // Print an expression.
      ungettoken();
      val = expression();
      printf("%d\n", val);
    }
  }
  else if (now.token.type == TOKEN_TYPE::FOR)
  { // FOR
    // Loop variable.
    gettoken();
    if (now.token.type != TOKEN_TYPE::IDENTIFIER)
      ERROR("Loop variable expected")
    vi = varindex(now.token.str);
    // '='
    gettoken();
    if (now.token.type != TOKEN_TYPE::EQU)
      ERROR("'=' expected")
    // Initial value.
    val = expression();
    now.vars.inf[vi].value = val;
    // TO or DOWNTO.
    gettoken();
    if (now.token.type == TOKEN_TYPE::TO)
      now.fors.inf[now.fors.top].dir = 1;
    else if (now.token.type == TOKEN_TYPE::DOWNTO)
      now.fors.inf[now.fors.top].dir = 0;
    else
      ERROR("'TO' or 'DOWNTO' expected")
    // Final value.
    int mm;
    mm = expression();
    if ((now.fors.inf[now.fors.top].dir && mm < val) || (!now.fors.inf[now.fors.top].dir && mm > val))
      ERROR("Inproper initial and final values that will lead to endless loop")
    if (now.fors.top >= MAX_NESTED_FOR)
      ERROR("Only support 32 nested for")
    // Push the info into the FOR stack.
    now.fors.inf[now.fors.top].var = vi;
    now.fors.inf[now.fors.top].bi = now.lines.cno + 1;
    now.fors.inf[now.fors.top++].mm = mm;
  }
  else if (now.token.type == TOKEN_TYPE::GOSUB)
  {
    int val = expression();
    if (val <= 0)
      ERROR("Label number must be positivie")
    if (now.subs.top >= MAX_SUB_DEPTH)
      ERROR("Only support 32 sub depth")
    now.subs.inf[now.subs.top++].ri = now.lines.cno + 1;
    jump(val);
  }
  else if (now.token.type == TOKEN_TYPE::GOTO)
  {
    int val = expression();
    if (val <= 0)
      ERROR("Label number must be positivie")
    jump(val);
  }
  else if (now.token.type == TOKEN_TYPE::IF)
  {
    int bval = boolexpr();
    gettoken();
    if (now.token.type != TOKEN_TYPE::THEN)
      ERROR("THEN expected")
    int val = expression();
    if (bval)
      jump(val);
  }
  else if (now.token.type == TOKEN_TYPE::NEXT)
  { // NEXT
    // Loop variable.
    gettoken();
    if (now.token.type != TOKEN_TYPE::IDENTIFIER)
      ERROR("Loop variable expected")
    vi = varindex(now.token.str);
    for (i = 0; i < now.fors.top; i++)
      if (vi == now.fors.inf[i].var)
        break;
    if (i == now.fors.top)
      ERROR("Missing FOR clause with this loop variable")
    // Seek to proper file position previously stored.
    if ((now.fors.inf[i].dir && now.vars.inf[vi].value < now.fors.inf[i].mm) || (!now.fors.inf[i].dir && now.vars.inf[vi].value > now.fors.inf[i].mm))
    {
      now.vars.inf[vi].value += (now.fors.inf[i].dir ? 1 : -1);
      now.lines.cno = now.fors.inf[i].bi;
      fseek(now.fp, now.lines.linf[now.lines.cno--].offset, SEEK_SET);
    }
    else
      // Pop.
      now.fors.top = i;
  }
  else if (now.token.type == TOKEN_TYPE::REM)
  { // REM
    // Set the character where p points to '\0' to prevent program() considering the remarks as remnant codes.
    *now.lines.p = '\0';
  }
  else if (now.token.type == TOKEN_TYPE::RETURN)
  {
    if (now.subs.top == 0)
      ERROR("No sub pushed into the stack")
    now.lines.cno = now.subs.inf[--now.subs.top].ri;
    fseek(now.fp, now.lines.linf[now.lines.cno--].offset, SEEK_SET);
  }
  else if (now.token.type == TOKEN_TYPE::END)
  { // END
    now.subs.top = 0;
  }
  else if (now.token.type == TOKEN_TYPE::PUSH)
  { // PUSH
    int val = expression();
    if (now.stk.top >= MAX_STACK_LEN)
      ERROR("Stack overflow")
    now.stk.base[now.stk.top++] = val;
  }
  else if (now.token.type == TOKEN_TYPE::POP)
  { // POP
    gettoken();
    if (now.token.type != TOKEN_TYPE::IDENTIFIER)
      ERROR("Variable expected")
    vi = varindex(now.token.str);
    if (now.stk.top == 0)
      ERROR("Stack empty, cannot pop")
    now.vars.inf[vi].value = now.stk.base[--now.stk.top];
  }
  else
    ERROR("Invalid statement")
}

// Jump to the statement with label number 'lblno'
void jump(int lblno)
{
  int i;
  for (i = 0; i < now.lines.cnt; i++)
  {
    if (now.lines.linf[i].lblno == lblno)
      break;
  }
  if (i == now.lines.cnt)
  { // No statement labeled 'lblno'
    while (getline())
    {
      parselblno();
      if (now.lines.linf[now.lines.cno].lblno == lblno)
      {
        ungettoken();
        // Execute current line.
        statement();
        // There's remnant codes left in the line.
        if (*now.lines.p)
          ERROR("Invalid token after the statement");
        return;
      }
    }
    ERROR("Specified label number not found")
  }
  now.lines.cno = i - 1;
  fseek(now.fp, now.lines.linf[i].offset, SEEK_SET);
}

// Evaluate an expression.
int expression()
{
  // Process sign mark.
  int sign1 = 1;
  gettoken();
  if (now.token.type == TOKEN_TYPE::INVALID)
    ERROR("Term expected")
  else if (now.token.type == TOKEN_TYPE::ADD)
    sign1 = 1;
  else if (now.token.type == TOKEN_TYPE::SUB)
    sign1 = -1;
  else
    ungettoken();
  // Get term1.
  int val1;
  val1 = sign1 * term();
  // Get operator.
  gettoken();
  int op = now.token.type;
  // Loop until all + or - have been processed.
  while (op == TOKEN_TYPE::ADD || op == TOKEN_TYPE::SUB)
  {
    int val2 = term();
    if (op == TOKEN_TYPE::ADD)
      val1 += val2;
    else
      val1 -= val2;
    gettoken();
    op = now.token.type;
  }
  // Unget the last non-operator token back into the line.
  if (op != TOKEN_TYPE::INVALID)
    ungettoken();
  return val1;
}

// Evaluete a term.
int term()
{
  int val1;
  // Check if there is something left in the line.
  gettoken();
  if (now.token.type == TOKEN_TYPE::INVALID)
    ERROR("Item expected")
  else
    ungettoken();
  // Get term1.
  val1 = item();
  // Get operator.
  gettoken();
  int op = now.token.type;
  // Loop until all * or / have been processed.
  while (op == TOKEN_TYPE::MUL || op == TOKEN_TYPE::DIV)
  {
    int val2 = item();
    if (op == TOKEN_TYPE::MUL)
      val1 *= val2;
    else
      val1 /= val2;
    gettoken();
    op = now.token.type;
  }
  // Unget the last non-operator token back into the line.
  if (op != TOKEN_TYPE::INVALID)
    ungettoken();
  return val1;
}

// Evaluate an item.
int item()
{
  int val = 0;
  // Check if there is something left in the line.
  gettoken();
  if (now.token.type == TOKEN_TYPE::IDENTIFIER)
  {
    int vi = varindex(now.token.str);
    val = now.vars.inf[vi].value;
  }
  else if (now.token.type == TOKEN_TYPE::INTEGER)
  {
    val = atoi(now.token.str);
  }
  else if (now.token.type == TOKEN_TYPE::LPAREN)
  {
    // After '(', there is a new expression.
    val = expression();
    // Check if ')' presents to match '('.
    gettoken();
    if (now.token.type != TOKEN_TYPE::RPAREN)
      ERROR("Parenthesis mismatch")
  }
  else
    ERROR("Interger, identifier or expression expected")
  return val;
}

// Evaluate a boolean expression.
int boolexpr()
{
  // Process NOT
  int rev1 = 0;
  gettoken();
  if (now.token.type == TOKEN_TYPE::INVALID)
    ERROR("Boolean term expected")
  else if (now.token.type == TOKEN_TYPE::NOT)
    rev1 = 1;
  else
    ungettoken();
  // Get term1.
  int val1;
  val1 = boolterm();
  if (rev1) val1 = !val1;
  // Get operator.
  gettoken();
  int op = now.token.type;
  // Loop until all + or - have been processed.
  while (op == TOKEN_TYPE::OR)
  {
    int rev2 = 0;
    gettoken();
    if (now.token.type == TOKEN_TYPE::INVALID)
      ERROR("Boolean term expected")
    else if (now.token.type == TOKEN_TYPE::NOT)
      rev2 = 1;
    else
      ungettoken();
    int val2 = boolterm();
    val1 = val1 || (rev2 ? !val2 : val2);
    gettoken();
    op = now.token.type;
  }
  // Unget the last non-operator token back into the line.
  if (op != TOKEN_TYPE::INVALID)
    ungettoken();
  return val1;
}

// Evaluate a boolean term.
int boolterm()
{
  int val1;
  // Check if there is something left in the line.
  gettoken();
  if (now.token.type == TOKEN_TYPE::INVALID)
    ERROR("Boolean item expected")
  else
    ungettoken();
  // Get term1.
  val1 = boolitem();
  // Get operator.
  gettoken();
  int op = now.token.type;
  // Loop until all * or / have been processed.
  while (op == TOKEN_TYPE::AND)
  {
    int val2 = boolitem();
    val1 = val1 && val2;
    gettoken();
    op = now.token.type;
  }
  // Unget the last non-operator token back into the line.
  if (op != TOKEN_TYPE::INVALID)
    ungettoken();
  return val1;
}

// Evaluate a boolean item.
int boolitem()
{
  int val = 0;
  // Check if there is something left in the line.
  gettoken();
  if (now.token.type == TOKEN_TYPE::TRUE)
  {
    val = 1;
  }
  else if (now.token.type == TOKEN_TYPE::FALSE)
  {
    val = 0;
  }
  else if (now.token.type == TOKEN_TYPE::BLPAREN)
  {
    // After '{', there is a new expression.
    val = boolexpr();
    // Check if '}' presents to match '{'.
    gettoken();
    if (now.token.type != TOKEN_TYPE::BRPAREN)
      ERROR("Parenthesis in boolean expression mismatch")
  }
  else
  {
    ungettoken();
    // Calculate comparing expression.
    int lhs = expression();
    gettoken();
    int op = now.token.type;
    int rhs = expression();
    switch (op)
    {
    case TOKEN_TYPE::BT:
      val = lhs > rhs;
      break;
    case TOKEN_TYPE::BTE:
      val = lhs >= rhs;
      break;
    case TOKEN_TYPE::EQU:
      val = lhs == rhs;
      break;
    case TOKEN_TYPE::NEQ:
      val = lhs != rhs;
      break;
    case TOKEN_TYPE::ST:
      val = lhs < rhs;
      break;
    case TOKEN_TYPE::STE:
      val = lhs <= rhs;
      break;
    default:
      ERROR("Undefined comparing expression")
      break;
    }
  }
  return val;
}

// Get the index from the name of the variable, and create it if not exists.
int varindex(const char* name)
{
  // Find the index if the variale existed.
  for (int i = 0; i < now.vars.cnt; i++)
    if (STRCMP(name, now.vars.inf[i].name))
      return i;
  // Variable count reaches the maximum?
  if (now.vars.cnt == MAX_VAR_COUNT)
    ERROR("Only supports up to 64 variables")
  // Allocate space for name string.
  now.vars.inf[now.vars.cnt].name = (char*)malloc(sizeof(char) * MAX_TOKEN_LEN);
  strncpy(now.vars.inf[now.vars.cnt].name, name, MAX_TOKEN_LEN - 1);
  return now.vars.cnt++;
}