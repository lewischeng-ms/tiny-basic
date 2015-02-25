#include "stdafx.h"
#include "basic.h"

// Put the current token back to the line.
void ungettoken()
{
  now.lines.p = now.lines.l;
}

// Get a token.
void gettoken()
{
  // Skip the leading blankspace.
  while (*now.lines.p == ' ')
    now.lines.p++;
  // Memorize the beginning address in case user wants to unget token.
  now.lines.l = now.lines.p;
  int len;
  // Preset type to INVALID for the sake of some conditions below.
  now.token.type = TOKEN_TYPE::INVALID;
  if (*now.lines.p == '\0')
  { // Reaches the end of the line, return INVALID token.
    // Nothing to do here.
  }
  else if (isalpha(*now.lines.p))
  { // Process identifier and probably keyword.
    len = 0;
    // Read the name of the identifier into the string buffer and make sure not exceed the MAX_TOKEN_LEN.
    while (isalnum(*now.lines.p) && len < MAX_TOKEN_LEN)
      now.token.str[len++] = *now.lines.p++;
    now.token.str[len] = '\0';
    if (len ==  MAX_TOKEN_LEN)
      ERROR("Too many characters in the identifier")
    // See if it's a keyword.
    if (STRCMP(now.token.str, "FOR"))
      now.token.type = TOKEN_TYPE::FOR;
    else if (STRCMP(now.token.str, "TO"))
      now.token.type = TOKEN_TYPE::TO;
    else if (STRCMP(now.token.str, "DOWNTO"))
      now.token.type = TOKEN_TYPE::DOWNTO;
    else if (STRCMP(now.token.str, "NEXT"))
      now.token.type = TOKEN_TYPE::NEXT;
    else if (STRCMP(now.token.str, "GOSUB"))
      now.token.type = TOKEN_TYPE::GOSUB;
    else if (STRCMP(now.token.str, "RETURN"))
      now.token.type = TOKEN_TYPE::RETURN;
    else if (STRCMP(now.token.str, "GOTO"))
      now.token.type = TOKEN_TYPE::GOTO;
    else if (STRCMP(now.token.str, "IF"))
      now.token.type = TOKEN_TYPE::IF;
    else if (STRCMP(now.token.str, "THEN"))
      now.token.type = TOKEN_TYPE::THEN;
    else if (STRCMP(now.token.str, "INPUT"))
      now.token.type = TOKEN_TYPE::INPUT;
    else if (STRCMP(now.token.str, "PRINT"))
      now.token.type = TOKEN_TYPE::PRINT;
    else if (STRCMP(now.token.str, "PRINTLN"))
      now.token.type = TOKEN_TYPE::PRINTLN;
    else if (STRCMP(now.token.str, "LET"))
      now.token.type = TOKEN_TYPE::LET;
    else if (STRCMP(now.token.str, "REM"))
      now.token.type = TOKEN_TYPE::REM;
    else if (STRCMP(now.token.str, "END"))
      now.token.type = TOKEN_TYPE::END;
    else if (STRCMP(now.token.str, "TRUE"))
      now.token.type = TOKEN_TYPE::TRUE;
    else if (STRCMP(now.token.str, "FALSE"))
      now.token.type = TOKEN_TYPE::FALSE;
    else if (STRCMP(now.token.str, "AND"))
      now.token.type = TOKEN_TYPE::AND;
    else if (STRCMP(now.token.str, "OR"))
      now.token.type = TOKEN_TYPE::OR;
    else if (STRCMP(now.token.str, "NOT"))
      now.token.type = TOKEN_TYPE::NOT;
    else if (STRCMP(now.token.str, "PUSH"))
      now.token.type = TOKEN_TYPE::PUSH;
    else if (STRCMP(now.token.str, "POP"))
      now.token.type = TOKEN_TYPE::POP;
    else
      now.token.type = TOKEN_TYPE::IDENTIFIER;
  }
  else if (*now.lines.p == '"')
  { // Process a string.
    len = 0;
    now.lines.p++;
    // Continuing reading until meet '"', '\0', '\n' and make sure not exceed the MAX_TOKEN_LEN.
    while (*now.lines.p != '\0' && *now.lines.p != '"' && *now.lines.p != '\n' && len < MAX_TOKEN_LEN)
      now.token.str[len++] = *now.lines.p++;
    now.token.str[len] = '\0';
    if (len ==  MAX_TOKEN_LEN)
      ERROR("Too many characters in the string")
    // Check if another '"' presents to match the former '"'.
    if (*now.lines.p != '"')
      ERROR("Missing a double quotation mark")
    now.lines.p++;
    now.token.type = TOKEN_TYPE::STRING;
  }
  else if (isdigit(*now.lines.p))
  { // Process a integer number.
    len = 0;
    // Read the whole number into the string buffer and make sure not exceed the MAX_TOKEN_LEN.
    while (isdigit(*now.lines.p) && len < MAX_TOKEN_LEN)
      now.token.str[len++] = *now.lines.p++;
    now.token.str[len] = '\0';
    if (len ==  MAX_TOKEN_LEN)
      ERROR("Too many characters in the integer")
    now.token.type = TOKEN_TYPE::INTEGER;
  }
  else
  { // All about operators or invalid token come to here.
    if (*(now.lines.p + 1))
    { // The operator takes two characters, such as <>, <= >=.
      if (*now.lines.p == '<' && *(now.lines.p + 1) == '>')
        now.token.type = TOKEN_TYPE::NEQ;
      else if (*now.lines.p == '>' && *(now.lines.p + 1) == '=')
        now.token.type = TOKEN_TYPE::BTE;
      else if (*now.lines.p == '<' && *(now.lines.p + 1) == '=')
        now.token.type = TOKEN_TYPE::STE;
      if (now.token.type != TOKEN_TYPE::INVALID)
      {
        now.lines.p += 2;
        return;
      }
    }
    // Operators take only one character.
    if (*now.lines.p == '(')
      now.token.type = TOKEN_TYPE::LPAREN;
    else if (*now.lines.p == ')')
      now.token.type = TOKEN_TYPE::RPAREN;
    else if (*now.lines.p == '{')
      now.token.type = TOKEN_TYPE::BLPAREN;
    else if (*now.lines.p == '}')
      now.token.type = TOKEN_TYPE::BRPAREN;
    else if (*now.lines.p == '=')
      now.token.type = TOKEN_TYPE::EQU;
    else if (*now.lines.p == '<')
      now.token.type = TOKEN_TYPE::ST;
    else if (*now.lines.p == '>')
      now.token.type = TOKEN_TYPE::BT;
    else if (*now.lines.p == '+')
      now.token.type = TOKEN_TYPE::ADD;
    else if (*now.lines.p == '-')
      now.token.type = TOKEN_TYPE::SUB;
    else if (*now.lines.p == '*')
      now.token.type = TOKEN_TYPE::MUL;
    else if (*now.lines.p == '/')
      now.token.type = TOKEN_TYPE::DIV;
    if (now.token.type != TOKEN_TYPE::INVALID)
    {
      now.lines.p++;
      return;
    }
    // Mismatch all the above conditions, then come to here and finally output an error.
    ERROR("Unrecognizable character")
  }
}

// Get a line from the file into the line buffer.
int getline()
{
  // Memorize the start position of the line for the later code jumps.
  int os = ftell(now.fp);
  // Skip the following consecutive '\n'
  char c = fgetc(now.fp);
  while (c == '\n')
    c = fgetc(now.fp);
  if (c == EOF)
    return 0;
  // Read until meet '\n' or EOF
  int len = 0;
  while (c != '\n' && c != EOF)
  {
    now.lines.buf[len++] = c;
    c = fgetc(now.fp);
  }
  now.lines.buf[len] = '\0';
  // Process some info about lines.
  now.lines.p = now.lines.buf;
  now.lines.linf[++now.lines.cno].offset = os;
  if (now.lines.cno >= now.lines.cnt)
    now.lines.cnt = now.lines.cno + 1;
  return 1;
}