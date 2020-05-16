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
		char template[] = "/tmp/tmp-XXXXXX";
		int tmp_file = mkstemp(template);
		write(tmp_file, line, strlen(line));
	    printf("%s\n", template);
	}
	else {
	    printf("Expr!\n");
	}
    // printf("Got %zu chars.\n", strlen(line)); // WTF?
  }
}
