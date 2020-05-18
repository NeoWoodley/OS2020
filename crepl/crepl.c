#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>

int fac(int base, int time) {
   int ret = 1;

   for(int i = 0; i < time; i ++) {
       ret *= base;
   }

   return ret;
}

int main(int argc, char *argv[]) {
  static char line[4096];
  static char func[4] = "int";
  static int count = 0;
  char template[] = "/tmp/tmp-XXXXXX.c";
  int tmp_file = mkstemps(template, 2);
//  printf("%s\n", template);

  char libname[64] = "/tmp/lib";
  char suffix[3] = "so";
  strncat(libname, &template[5], 11);
  strcat(libname, suffix);

  char exec_file[] = "gcc";

  char func_name[32] = "__expr_wrapper_";

  char buf[512];
  memset(buf, '\0', 512);

  while (1) {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }
	if(strncmp(func, line, 3) == 0) {
		int flag = 0;
        char testfile[] = "/tmp/test-XXXXXX.c";
        int test_file = mkstemps(testfile, 2);
		write(tmp_file, line, strlen(line));

		char testlibname[64] = "/tmp/lib";
		char testsuffix[3] = "so";
		strncat(testlibname, &testfile[5], 12);
		strcat(testlibname, testsuffix);

		int fildes[2];

		if(pipe(fildes) != 0) {
		    printf("Pipe failde!\n");
			assert(0);
		}

		int prepid = fork();
		if(prepid == 0) {
			close(fildes[0]);
			dup2(fildes[1], fileno(stderr));
		    execlp(exec_file, "gcc", "-fPIC", "-shared", testfile, "-o", testlibname, NULL);
		}
		else {
		    sleep(1);
			close(fildes[1]);

			dup2(fildes[0], fileno(stdin));

			char* errorfile = fgets(buf, 511, stdin);
			
			assert(error == NULL);
			
		    printf("Done.\n");
		}

        if(flag == 0) {

		    lseek(tmp_file, 0, SEEK_END);
		    write(tmp_file, line, strlen(line));
		    int pid = fork();
		    if(pid == 0) {
		        execlp(exec_file, "gcc", "-fPIC", "-shared", template, "-o", libname, NULL);
		    }
		    else {
			    sleep(1);
		        printf("OK.\n");
		    }
	    }
	}
	else {
		char funcbody[256] = "int __expr_wrapper_";
		char index_str[4];
		memset(index_str, '\0', 4);

		int tmp = count;

		int bitnum = 1;

		while(tmp / 10 > 0) {
		    tmp /= 10;
			bitnum ++;
		}

		tmp = count;
		char bit;
		for(int i = 0; i < bitnum; i ++) {
		    int base = fac(10, bitnum-1-i);
			int num = tmp / base;
			bit = num + '0';
			index_str[i] = bit;

			tmp %= base;
		}

//		printf("num:%s\n", index_str);

		
		strcat(funcbody, index_str);
		strcat(func_name, index_str);
		char funcpart[32] = "() { return ";
		char funcend[4] = ";}\n";
		strcat(funcbody, funcpart);
		strncat(funcbody, line, strlen(line)-1);
		strcat(funcbody, funcend);

//		printf("funcbody:%s\n", funcbody);
//		printf("funcname:%s\n", func_name);

		lseek(tmp_file, 0, SEEK_END);
		write(tmp_file, funcbody, strlen(funcbody));

		int pid = fork();
		if(pid == 0) {
		    execlp(exec_file, "gcc", "-fPIC", "-shared", template, "-o", libname, NULL);
		}
		else {
			sleep(1);
		    void* handle = dlopen(libname, RTLD_LAZY);
			assert(handle != NULL);

			int (*callfunc)();
			
			char* error;

			callfunc = dlsym(handle, func_name);

            error = dlerror();

			if(error != NULL) {
			    printf("Error:%s\n", error);
				exit(1);
			}

			int result = callfunc();

	        printf("%d\n", result);
		    count ++;
            memset(&func_name[15], '\0', 17);

			dlclose(handle);
		}
	}
    // printf("Got %zu chars.\n", strlen(line)); // WTF?
  }
}
