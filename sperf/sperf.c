#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.c>

/*
   int execve(
       const char *filename,
	   char *const argv[],
	   char *const envp[]
   );

   strace -T 显示系统调用所花时间
*/

int main(int argc, char *argv[]) {

  /*
  char *exec_argv[argc];
  for(int i = 0; i < argc-1; i ++) {
      exec_argv[i] = argv[i+1];
  }
  exec_argv[argc-1] = NULL;
  char *exec_envp[] = { "PATH=/bin", NULL, };
  */
  char PATH[128] = "";
  char *path = NULL; 
  char path_name[6] = "PATH=";
  strcpy(PATH, path_name);
  path = getenv("PATH");
  strcat(PATH, path);

  printf("path:%s\n",PATH);
//  char *exec_envp[] = { 0, NULL, };

  /*
  execve("/usr/bin/strace", exec_argv, exec_envp);
  perror(argv[0]);
  exit(EXIT_FAILURE);
  */
  /*
  printf("----------argc:%d------------\n",argc);
  for(int i = 0; i < argc - 1; i ++) {
      printf("i:%d\t%s\n",i, exec_argv[i]);
  }
  printf("-----------------------\n");
	return 0;
*/
/*
  char *exec_argv[] = { "strace", "ls", NULL, };
  char *exec_envp[] = { "PATH=/bin", NULL, };
//  execve("strace",          exec_argv, exec_envp);
//  execve("/bin/strace",     exec_argv, exec_envp);
  execve("/usr/bin/strace", exec_argv, exec_envp);
  perror(argv[0]);
  exit(EXIT_FAILURE);
*/
  return 0;
}
