#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  char *exec_argv[] = { argv[1], argv[2], NULL, };
  char *exec_envp[] = { "PATH=/bin", NULL, };
  execve("/usr/bin/strace", exec_argv, exec_envp);
  perror(argv[0]);
  exit(EXIT_FAILURE);
  */
  printf("-----------------------\n");
  for(int i = 0; i < argc; i ++) {
      printf("i:%d\t%s\n",i, argv[i]);
  }
  printf("-----------------------\n");
	return 0;

/*
  char *exec_argv[] = { "strace", "ls", NULL, };
  char *exec_envp[] = { "PATH=/bin", NULL, };
//  execve("strace",          exec_argv, exec_envp);
//  execve("/bin/strace",     exec_argv, exec_envp);
  execve("/usr/bin/strace", exec_argv, exec_envp);
  perror(argv[0]);
  exit(EXIT_FAILURE);
*/
}
