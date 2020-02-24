#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>   //char *strstr(const char *haystack, const char *needle)
#include <dirent.h>

int count = 0;
char space[3] = "  ";

struct node {
    int pid;
	char name[50];
	int ppid;
	int depth;
	struct node* child;
	long int space;
	struct node* peer;
};

struct node* findpeertail(struct node* Node) {
	struct node* end = (struct node*)Node;
    while(end->peer != NULL) {
	    end = end->peer;
	}
	return (struct node*)end;
}

int len2n(char *str) {
    int len = 0;
	while (str[len] != '\n' && str[len] != '\0' && str[len] != ' ') {
	    len ++;
	}
	return len;
}

int str2int(char *str) {
    int val = 0;
	int i = 0;
	while(str[i] != '\0' && str[i] != '\n') {
		val *= 10;
		val += str[i] - '0';
		i ++;
	}
	return val;
}

int find_num(char *str) {
    int return_code;

    if(strchr(str, '0')==NULL && strchr(str, '1')==NULL && strchr(str, '2')==NULL && strchr(str, '3')==NULL && strchr(str, '4')==NULL && strchr(str, '5')==NULL && strchr(str, '6')==NULL && strchr(str, '7')==NULL && strchr(str, '8')==NULL && strchr(str, '9')==NULL) {
        return_code = 0;
    }
    else {
        return_code = 1; 
    }
    return return_code;
}

void buildtree(struct node* Node, int mode) {
	struct node* temp;
    if(Node->child == NULL) {
		for(int i = 0; i < Node->depth; i ++) {
		    printf("%s",&space[0]);
		}
		if(mode == 0) {
		    printf("%s\n",&(Node)->name[0]);
		}
		else {
		    printf("%s{%d}\n",&(Node)->name[0],Node->pid);
		}
			// printf("%s{%d}\n",&(Node->name[0]), Node->pid);
		    //printf("%s{%d}--ppid{%d}--depth{%d}\n",&(Node)->name[0],Node->pid,Node->ppid,Node->depth);
		return;
	}
	
	else if(Node->child != NULL) {
		for(int i = 0; i < Node->depth; i ++) {
		    printf("%s",&space[0]);
		}
		if(mode == 0) {
		    printf("%s\n",&(Node)->name[0]);
		}
		else {
		    printf("%s{%d}\n",&(Node)->name[0],Node->pid);
		}
       // printf("%s{%d}\n",&(Node->name[0]), Node->pid);
		temp = (struct node*)Node->child->peer;
	    while(temp != NULL) {
		   buildtree(temp , mode);
		   temp = (struct node*)(temp->peer);
		}
		return;
	}
}

int main(int argc, char *argv[]) {

  DIR* dir;
  struct dirent* entry;

  static struct node infolib[10];

  //dir = opendir("/proc");
  dir = opendir("./TESTM1");
  assert(dir != NULL);

  char _V[13]="-V --version";
  char _n[18]="-n --numeric-sort";
  char _p[15]="-p --show-pids";
  char _ppid[5]="PPid";

  while((entry=readdir(dir))) {
        if(find_num(entry->d_name) == 0) {
            // printf("File %s :Not a process file!\n", entry->d_name);
        }
        else {
           // printf("File %s :Process!\n", entry->d_name);
			infolib[count].pid = str2int(entry->d_name);
            //infolib[count].name = entry->d_name;
            count ++;
        }
  }

  for(int i = 0; i < count; i ++){
      infolib[i].depth = 0;
	  infolib[i].peer = NULL;
	  infolib[i].child = NULL;
	  for(int j = 0; j < 50; j ++) {
	      infolib[i].name[j] = '\0';
	  }
  }

  // printf("Total process number:%d\n",count);

  closedir(dir);

  char* path = (char*)malloc(32*sizeof(char));
  char* name = (char*)malloc(50*sizeof(char));
  char* ppid = (char*)malloc(32*sizeof(char));
  char* head;
  FILE* file;
  for(int i = 0; i < count; i ++) {
	  sprintf(path,"./TESTM1/%d/status",infolib[i].pid);
      file = fopen(path, "r");
	  assert(file != NULL);
      if(file) {
		  //printf("----------\n");
	      fgets(name, 49, file);
		  head = name;
		  while(*head != ' ') {
		      head ++;
		  }
		  while(*head == ' ') {
		      head ++;
		  }
		  strcat(&infolib[i].name[0], head);
		  infolib[i].name[len2n(infolib[i].name)] = '\0';
	//	  printf("Name:%s\n",(char*)&infolib[i].name[0]);
      for(int j = 0; j < 6; j ++) {
	      fgets(ppid, 32, file);
		  if(strstr(ppid, &_ppid[0]) != NULL) {
	//		  printf("%s",ppid);
		      break;
		  }
	  }   
	  head = ppid;
      while(*head != ' ') {
	      head ++;
	  }
	  while(*head == ' ') {
	      head ++;
	  }
	  //printf("-----%d: %s",i, ppid);
	  infolib[i].ppid=str2int(head);
	  } 	  

  }
  free(path);
  free(name);
  free(ppid);

  /*
  for(int i = 0; i < count; i ++) {
       printf("name:%s, pid:%d, ppid:%d\n", infolib[i].name, infolib[i].pid, infolib[i].ppid);
  }
  */

  //struct node* temp = (struct node*)malloc(sizeof(struct node));
  struct node* temp;
  for(int i = 0; i < count; i ++) {
	  for(int j = 0; j < count; j ++) {
	      if(infolib[i].child == NULL && infolib[j].ppid == infolib[i].pid) {
//			  printf("-----S1\n");
		      infolib[i].child = (struct node*)&infolib[j];
			  infolib[j].depth = infolib[i].depth + 1; 
		  }
		  else if(infolib[i].child != NULL && infolib[j].ppid == infolib[i].pid) {
//			  printf("-----S2\n");
			  if(infolib[i].child->peer == NULL) {
//			      printf("-----S2-1\n");
			      infolib[i].child->peer = (struct node*)&infolib[j];
			  }
			  else if(infolib[i].child->peer != NULL){
//			      printf("-----S2-2\n");
                  temp = findpeertail((struct node*)&(infolib[i].child->peer));
			      temp->peer = (struct node*)&infolib[j];
//				  printf("@@@@@%s\n",temp->peer->name);
			  }
			  infolib[j].depth = infolib[i].depth + 1; 
		  }
		  else {
//		      printf("-----S3\n");
		  }
//		  printf("i=%d,j=%d,lib[%d].depth=%d, lib[%d].child:%p, lib[%d].depth=%d\n",i,j,i,infolib[i].depth,i,infolib[i].child,j,infolib[j].depth); 
//		  printf("name_i:%s, pid_i:%d, ppid_i:%d, depth_i:%d, child:%p, peer:%p\nname_j:%s, pid_j:%d, ppid_j:%d, depth_j:%d, child:%p, peer:%p\n",&infolib[i].name[0],infolib[i].pid,infolib[i].ppid,infolib[i].depth,infolib[i].child,infolib[i].peer,&infolib[j].name[0],infolib[j].pid,infolib[j].ppid,infolib[j].depth,infolib[j].child,infolib[j].peer);
//		  printf("----------------------------------------------------------------------------------\n");
	  }
  }

  for(int i = 0; i < count; i ++) {
	  printf("i:%d\n",i);
      if(infolib[i].child == NULL) {
	       printf("CHILD NULL!\n");
	  }
      if(infolib[i].peer == NULL) {
	       printf("PEER NULL!\n");
	  }
	  if(infolib[i].depth == 0) {
	      printf("DEPTH 0!\n");
	  }
	  printf("-------------------------------\n");
  }

  buildtree(&infolib[0],0);

  /*
  assert(infolib[2].child == NULL);
  assert(infolib[0].child == NULL);
  assert(infolib[1].peer == NULL);
  assert(infolib[2].peer == NULL);
  assert(infolib[4].peer == NULL);
  assert(infolib[3].child == NULL);
  */

 // assert(infolib[0].child->peer != NULL);

  /*
  temp = infolib[0].child;
  while(temp != NULL) {
	  printf("Thread:%s Pid:%d PPid:%d\n", temp->name, temp->pid, temp->ppid);
	  temp = temp->peer;
  }
  */

  /*
  for(int i = 0; i < 1; i ++) {
	  if(infolib[i].depth == 0) {
		  assert(infolib[i].child->peer != NULL);
		  printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].name, infolib[i].pid, infolib[i].ppid);
		  printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].child->name, infolib[i].child->pid, infolib[i].child->ppid);
		  printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].child->peer->name, infolib[i].child->peer->pid, infolib[i].child->peer->ppid);
		  if(infolib[i].child->peer->peer == NULL) {
		      printf("Yes!!!!!!!!!!!!!!!!\n");
		  }
		  else {
		      printf("No?????????????????\n");
		  
		  }
		  //printf("Thread:%s Pid:%d PPid:%d\n", infolib[i].child->peer->peer->name, infolib[i].child->peer->peer->pid, infolib[i].child->peer->peer->ppid);
  //        buildtree((struct node*)&infolib[i].child->peer);
		  printf("-------------------------------------------------------------\n");
	  }
//	  printf("%s{%d}--ppid{%d}--depth{%d}\n",&infolib[i].name[0], infolib[i].pid, infolib[i].ppid, infolib[i].depth);
  }
  */

  /*
  for(int i = 0; i < count; i ++){
      if(infolib[i].peer == NULL) {
	      printf("%d: YES!\n",i);
	  }
	  else {
	      printf("%d: NO!\n",i);
	  }
  }
  */
  

  /*
  for(int i = 0; i < count; i ++){
	  if(infolib[i].depth == 0) {
          buildtree(&infolib[i]);
	  }
	  //printf("%d: ppid: %d pid: %d\n", i, infolib[i].ppid, infolib[i].pid);
  }
  */
  
  /*
  struct node* test_node = (struct node*)malloc(sizeof(struct node));
  test_node = &infolib[63];
  while(test_node->peer != NULL) {
	  printf("Thread:%s Pid:%d PPid:%d\n", test_node->name, test_node->pid, test_node->ppid);
      test_node = test_node->peer; 
  }
  */
/* 
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
*/
  
  /*
  switch(argc) {
	  case 1: {
				      for(int i = 0; i < count; i ++) {
					      if(infolib[i].ppid == 0) {
						      buildtree(&infolib[i], 0);
						  }
					  }
				  break;
			  }
  
      case 2: {

				  if(strstr(&_V[0], argv[1]) != NULL) {
				      printf("Pstree-lite\nCopyright by Gan Chen\n");
				  }
				  else if(strstr(&_p[0], argv[1]) != NULL) {
				      for(int i = 0; i < count; i ++) {
					      if(infolib[i].ppid == 0) {
						      buildtree(&infolib[i], 1);
						  }
					  }
				  }
				  else if(strstr(&_n[0], argv[1]) != NULL) {
				      for(int i = 0; i < count; i ++) {
					      if(infolib[i].ppid == 0) {
						      buildtree(&infolib[i], 0);
						  }
					  }
				  
				  }

			      break;
			  }

	  case 3: {
				  int v = 0;
				  int p = 0;
//				  int n;

				  for(int i = 1; i < 3; i ++) {
				      if(strstr(&_V[0], argv[i]) != NULL) {
					      v = 1;
					  }

					  if(strstr(&_p[0], argv[i]) != NULL) {
					      p = 1;
					  }

//					  if(strstr(&_n[0], argv[i] != NULL)) {
//					      n = 1;
//					  }
				  }

				  if(v == 1) {
				      printf("Pstree-lite\nCopyright by Gan Chen\n");
				  }
			  
				  else if(p == 1) {
				      for(int i = 0; i < count; i ++) {
					      if(infolib[i].ppid == 0) {
						      buildtree(&infolib[i], 1);
						  }
					  }
				  }
				  
				  else {
				      for(int i = 0; i < count; i ++) {
					      if(infolib[i].ppid == 0) {
						      buildtree(&infolib[i], 0);
						  }
					  }
				  }

				  break;
			  }

	  default: assert(0);
  }
*/


  return 0;
  
}
