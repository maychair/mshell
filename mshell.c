#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>

char *getPrompt();
char *parseCommand(char *cmdLine);
bool isBuildInCommand(char *cmd);
void executeBuiltInCommand(char **cmd);
bool isBackgroundJob(cmd){ return false;}
int main()
{
  while (1) {
    int childPid;
    int status;
    char **cmd;
    char *cmdline;
    cmdline = readline(getPrompt()); //从一行输入中解析出命令;
    cmd = parseCommand(cmdline);
    //record command in history file, 参考 history 命令;
    if (!cmd[0]) { // 命令为空的时候不记 history, 不要往下执行;
      free(cmdline);
      free(cmd);
      continue;
    }
    if (isBuildInCommand(cmd[0])) //判断是否是 shell 内建命令，譬如 cd
      executeBuiltInCommand(cmd);
    else {
      childPid = fork();
      if (childPid == 0) {
        execvp(cmd[0], cmd);
      } else {
        if (isBackgroundJob(cmd)) {
          // record in list of background list;
        } else {
          waitpid(childPid, &status, 0); //等待子进程结束;
        }
      }
    }
  }
}


char *getPrompt()
{
  return "$ ";
}

#define MAX_CMD_SIZE 12
#define MSH_TOKEN_DELIM " \t\r\n\a"
char *parseCommand(char *cmdline)
{
  char **cmd = malloc(MAX_CMD_SIZE * sizeof(char*));
  int index = 0;
  char *token;
  token = strtok(cmdline, MSH_TOKEN_DELIM);
  while (token != NULL) {
    cmd[index] = token;
    index++;
    if (index > MAX_CMD_SIZE) {
      fprintf(stdout, "params number out of limits.");
      return NULL;
    }
    token = strtok(cmdline, MSH_TOKEN_DELIM);
  }
  cmd[index] = NULL;
  return cmd;
}

//注册内建函数，通过 builtin_str && builtin_func 调用内建函数；
char *builtin_str[] = {
  "cd",
  "exit",
  "help"
};
int msh_builtin_num = sizeof(builtin_str) / sizeof(char*);
typedef int (*builtin_cb) (char**);

int msh_cd(char **argv)
{
  if (argv[1] == NULL) {
    fprintf(stdout, "msh: expected argument to \"cd\"\n");
  } else {
    if (chdir(argv[1]) != 0) {
      fprintf(stdout, "msh: cannot cd to %s.", argv[1]);
    }
  }
  return 1;
}
int msh_exit(char **argv)
{
  exit(0);
}
int msh_help(char **argv)
{
  int i;
  fprintf(stdout, "Type program names and argument, and hit enter.\n");
  fprintf(stdout, "The following are built in:\n");
  for (i = 0; i < msh_builtin_num; i++) {
    fprintf(stdout, "    %s\n", builtin_str[i]);
  }
  return 1;
}

builtin_cb builtin_func[] = {
  &msh_cd,
  &msh_exit,
  &msh_help
};
//判断是否是内建函数
bool isBuildInCommand(char *cmd)
{
  int i;
  for (i = 0; i < msh_builtin_num; i++) {
    if (strcmp(cmd, builtin_str[i]) == 0) return true;
  }
  return false;
}

void executeBuiltInCommand(char **cmd)
{
  int i;
  for (i = 0; i < msh_builtin_num; i++) {
    if (strcmp(cmd[0], builtin_str[i]) == 0) {
      (*builtin_func[i])(cmd);
    }
  }
}
