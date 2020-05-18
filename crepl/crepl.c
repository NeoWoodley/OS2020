#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
  static char line[4096];
  static char func[4] = "int";
  char* tmplib[100];


  while (1) {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }
	if(strncmp(func, line, 3) == 0) {
		char template[] = "/tmp/tmp-XXXXXX.c";
		int tmp_file = mkstemps(template, 2);
		write(tmp_file, line, strlen(line));
		char exec_file[] = "gcc";
		int pid = fork();
		if(pid == 0) {
			char libname[32] = "/tmp/lib";
			char tmp[16];
			char suffix[4] = ".so";
		    strncat(tmp, &template[5], 11);
			strcat(tmp, suffix);
			strcat(libname, tmp);
	        printf(".c file: %s\n", template);
	        printf(".so file: %s\n", libname);

		    //execlp(exec_file, "gcc","-o", "-fPIC", "-shared", template,  );
		}


	    //printf("%s\n", template);
	}
	else {
	    printf("Expr!\n");
	}
    // printf("Got %zu chars.\n", strlen(line)); // WTF?
  }
}
