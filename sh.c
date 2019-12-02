#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE -1
void lsh_loop();
char* lsh_getline();
char** lsh_split_line(char* line);
int lsh_execute(char** argv);

int main(int argc, char **argv)
{
  lsh_loop();
  return EXIT_SUCCESS;
}

void lsh_loop()
{
  //todo
  char *line;
  char **argv;
  int status;

  do {
    printf("$ ");
    line = lsh_getline();
    argv = lsh_split_line(line);
    status = lsh_execute(argv);
    free(line);
    free(argv);
  } while(status);
}

// #define LSH_BUFSIZE 1024
// char* lsh_getline()
// {
//   int bufsize = LSH_BUFSIZE;
//   int position = 0;
//   char *buffer = malloc(sizeof(char) * bufsize);
//   int c;
//
//   if (!buffer) {
//     fprintf(stderr, "lsh: allocation error\n");
//     exit(EXIT_FAILURE);
//   }
//
//   while (1) {
//     c = getchar();
//     if (c == EOF || c == '\n') {
//       buffer[position] = '\0';
//       return buffer;
//     } else {
//       buffer[position] = c;
//     }
//
//     position++;
//
//     if (position >= bufsize) {
//       bufsize += LSH_BUFSIZE;
//       buffer = realloc(buffer, bufsize);
//       if (!buffer) {
//         fprintf(stderr, "lsh: allocation error\n");
//         exit(EXIT_FAILURE);
//       }
//     }
//   }
// }
char *lsh_getline()
{
  char *line = NULL;
  ssize_t bufsize = 0;
  getline(&line, &bufsize, stdin);
  return line;
}

#define LSH_TOKEN_BUFSIZE 64
#define LSH_TOKEN_DELIM " \t\r\n\a"
char** lsh_split_line(char* line)
{
  int bufsize = LSH_TOKEN_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOKEN_DELIM);
  where (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOKEN_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allcation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOKEN_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int lsh_execute(char** argv)
{
  int i;
  if (args[0] == NULL) {
    return 1;
  }
  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i](args));
    }
  }

  return lsh_launch(args);
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;
  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("lsh");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

// Function Declarations for builtin shell commands;
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
//list of builtin commands, followed by their correponding function.
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int lsh_num_builtins() { return sizeof(builtin_str) / sizeof(char*);}

//builtin function implementations
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args)
{
  int i;
  printf("lsh\n");
  printf("Type program names and argument, and hit enter.\n");
  printf("The following are built in:\n");
  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  printf("Use the man command for imformation on other program.\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0;
}
