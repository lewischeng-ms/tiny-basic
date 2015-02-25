// basic.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "basic.h"

Session now;

int main(int argc, char* argv[])
{
  printf("Hello! Welcome to the BASIC world!\n\n");
  if (argc == 1)
  {
    printf("Usage: basic [file1] [file2] ... [fileN].\n");
    return 0;
  }
  for (int i = 1; i < argc; i++)
  {
    printf("%s\n", argv[i]);
    interpret(argv[i]);
    printf("\n");
  }
	return 0;
}

void interpret(const char* fn)
{
  // Interpreter-pushed sub, you can think it as sth like main().
  now.subs.top = 1;
  now.subs.inf[0].ri = 0;
  // Current line number initially equals -1(start from 0).
  now.lines.cno = -1;
  // Begin to interpret the new file.
  now.fp = fopen(fn,"r");
  if (!now.fp)
  {
    printf("Error: Unabled to open %s.\n", fn);
    return;
  }
  // Parse the program.
  program();
  // Clean up the old session.
  fclose(now.fp);
  now.stk.top = 0;
  now.fors.top = 0;
  now.lines.cnt = 0;
  for (int i = 0; i < now.vars.cnt; i++)
    if (now.vars.inf[i].name)
      free(now.vars.inf[i].name);
  now.vars.cnt = 0;
}