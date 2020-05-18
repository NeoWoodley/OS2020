#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
  static char line[4096];
  static char func[4] = "int";
  static int count = 0;
  char template[] = "/tmp/tmp-XXXXXX.c";
  int tmp_file = mkstemps(template, 2);
  printf("%s\n", template);


  while (1) {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }
	if(strncmp(func, line, 3) == 0) {
		lseek(tmp_file, 0, SEEK_END);
		write(tmp_file, line, strlen(line));
		char exec_file[] = "gcc";
		int pid = fork();
		if(pid == 0) {
			char libname[64] = "/tmp/lib";
			char suffix[3] = "so";
		    strncat(libname, &template[5], 11);
			strcat(libname, suffix);
	        //printf(".c file: %s\n", template);
	        //printf(".so file: %s\n", libname);
		    execlp(exec_file, "gcc", "-fPIC", "-shared", template, "-o", libname, NULL);
		}
		else {
			sleep(1);
		    printf("OK.\n");
		}
	}
	else {
		char funcbody[256] = "int __expr_wrapper_";
		char index = '0'+ count;
		char index_str[2];
		index_str[0] = index;
		index_str[1] = '\0';
		strcat(funcbody, index_str);
		char funcpart[32] = "() { return ";
		char funcend[3] = ";}";
		strcat(funcbody, funcpart);
		strncat(funcbody, line, strlen(line)-1);
		strcat(funcbody, funcend);

		lseek(tmp_file, 0, SEEK_END);
		write(tmp_file, funcbody, strlen(funcbody));
	    //printf("Expr!\n");
		count ++;
	}
    // printf("Got %zu chars.\n", strlen(line)); // WTF?
  }
}
