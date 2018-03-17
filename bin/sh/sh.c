#include <stdlib.h>
#include <unistd.h>
#include <sys/defs.h>
#include <dirent.h>
#include <signal.h>

int main(int argc, char **argv)
{
	char buff[250], curdir[250], path[250]; 
	char* arg1 = NULL;
	char* arg2 = NULL;
	int isBg = 0;
	memset(path, 0, 250);
	strcpy(path, "/rootfs/bin/");
	memset(curdir, 0, 250);
	strcpy(curdir, "/");
	malloc(1);
	while(1){
		printf("[SBUnix %s]", curdir);
		memset(buff, 0, 250);
		isBg = 0;
		/*
					char c;
					do{
					scanf("%c", &c);
					printf("%d", c);
					}while(c!='\n');
			*/
		scanf("%s", buff);
		if(strlen(buff)==0||buff[0]=='\n') continue;
		if(buff[strlen(buff)-1] == '&'){
			isBg = 1;
			buff[strlen(buff)-1] = '\0';
		}

		char *cmd = strtok(buff, " ");
		if(strcmp(cmd, "ls") == 0) {
			DIR *dirp = NULL;
			arg1 = strtok(NULL, " ");

			if(arg1){
				dirp = opendir(arg1);
			}else{
				dirp = opendir(curdir);
			}
			if(dirp != NULL && dirp->filenode){
				struct dirent *dp;
				while((dp=readdir(dirp))!=NULL){
					printf("%s\t", dp->name);
				}
				printf("\n");
			}else{
				printf("No such file or directory\n");
			}
		}
		else if(strcmp(cmd, "ps") == 0) {
			listprocess();
			printf("\n");
		}
		else if(strcmp(cmd, "echo") == 0) {
			arg1 = strtok(NULL, " ");
			if(strcmp(arg1, "$PATH") == 0) printf("%s", path);
			while((arg1=strtok(NULL, " "))!= NULL){
				printf("%s", arg1);
			}
			printf("\n");
		}
		else if(strcmp(cmd, "clear") == 0){
			clear(); 
		}
		else if(strcmp(cmd, "pwd") == 0){
			printf("%s\n", curdir);
		}
		else if(strcmp(cmd, "cd") == 0){
			arg1 = strtok(NULL, " ");    
			if(!arg1){// move to /
				strcpy(curdir, "/");
			}else if(strcmp(arg1, ".") == 0){

			}else if(strcmp(arg1, "..") == 0 && strlen(curdir)> 2){
				int i=  strlen(curdir);
				while(curdir[--i]!='/');
				curdir[i] = '\0';
			}else{
				DIR *dirp;
				char temp[250];
				if(arg1[0]=='/'){
					dirp = opendir(arg1);
				}
				else{ 	
					strcpy(temp, curdir);
					strcat(temp, "/");
					strcat(temp, arg1);
					dirp = opendir(temp);
				}
				if(dirp == NULL){
					printf("No such file or directory\n");
					continue;
				}
				if(dirp->filenode && arg1[0]=='/'){
					strcpy(curdir, arg1);
					chdir(curdir);
				}
				else if(dirp->filenode){
					strcpy(curdir, temp);
					chdir(curdir);
				}
				else printf("No directory found\n");
			}
		}
		else if(strcmp(cmd, "export") == 0){
			arg1 = strtok(NULL, "="); 
			if(strcmp(arg1, "PATH") == 0){
				arg2 = strtok(NULL, "="); 
				strcpy(path, arg2);
			}else{
				printf("only path export is supported\n");
			}
		}
		else if(strcmp(cmd, "cat") == 0){
			arg1 = strtok(NULL, " ");
			int fd;
			char temp[250]; 
			if(arg1[0]=='/')
				fd = open(arg1, 0);
			else{ 
				strcpy(temp, curdir);
				strcat(temp, "/");
				strcat(temp, arg1);
				fd = open(temp, 0);
			}

			if(fd == -1){
				printf("No such file or directory\n");
			}else{
				char readbuff[100];
				while(read(fd, readbuff, sizeof(readbuff))> 0){
					printf("%s", readbuff);
				}
				close(fd);
			}   
		}
		else if(strcmp(cmd, "kill") == 0){
			arg1 = strtok(NULL, " ");
			if(arg1[1] == '9'){
				arg2 = strtok(NULL, " "); 
				kill(atoi(arg2), 0);
			}else{
				printf("only kill -9 commnad is supported\n");
			}
		}
		else if(strcmp(cmd, "help") == 0){
			int fd = open("/rootfs/etc/help", 0);
			char readbuff[100];
			while(read(fd, readbuff, sizeof(readbuff))>0) printf("%s", readbuff);
			close(fd); 
		}
		else if(strcmp(cmd, "sh") == 0){
			arg1 = strtok(NULL, " ");
			int fd;
			char temp[250]; 
			if(arg1[0]=='/')
				fd = open(arg1, 0);
			else{ 
				strcpy(temp, curdir);
				strcat(temp, "/");
				strcat(temp, arg1);
				fd = open(temp, 0);
			}

			if(fd == -1) 
				printf("No such file or directory\n");
			else{
				char readbuff[250];
				readline(fd, readbuff);
				if(readbuff[0]!='#'&&readbuff[1]!='!')
					printf("Not a  valid shell file\n");
				else{
					while(readline(fd, readbuff)>0){
						char temp1[250]; 
						char isBg = 0;
						if(readbuff[strlen(readbuff)-1] == '&'){
							isBg = 1;
							readbuff[strlen(readbuff)-1] = '\0';
						}

						char* t =  strtok(readbuff, " ");
						if(readbuff[0]=='/')
							strcpy(temp1, t);
						else{ 
							strcpy(temp1, path);
							strcat(temp1, t);
						}
						char* argv[100];
						int i=0;
						while((arg2=strtok(NULL, " ")) != NULL){
							argv[i++] = arg2;
						} 
						argv[i] = NULL;

						int pid = fork();
						if(pid != 0){
							if(isBg != 1)
								waitpid(pid, NULL, 0);
						}else{
							execvpe(temp1, argv, NULL);
						}
					}
				}
			}
		}else{
			char temp[250]; 
			if(cmd[0]=='/')
				strcpy(temp, cmd);
			else{ 
				strcpy(temp, path);
				strcat(temp, cmd);
			}
			int fd = open(temp, 0);
			if(fd==-1){
				printf("No such file or directory\n");
				continue;
			}
			close(fd);

			char* argv[100];
			int i=0;
			while((arg2=strtok(NULL, " ")) != NULL){
				argv[i++] = arg2;
			} 
			argv[i] = NULL;
			int pid = fork();
			if(pid != 0){
				if(isBg != 1) waitpid(pid, NULL, 0);
			}else{
				execvpe(temp, argv, NULL);
			}
		}
	}
	return 0;
}

