#include "paramaters.h"

struct cmd {
	struct cmd* next;
	int begin, end;
	int argc;
	char lredir, rredir;
	char toFile[MAX_PATH_LENGTH], fromFile[MAX_PATH_LENGTH];
	char *args[MAX_ARG_NUM];
	char bgExec; // 后台执行
};

struct cmd cmdinfo[MAX_CMD_NUM];
char cmdStr[MAX_CMD_LENGTH];
int cmdNum, varNum;

int parseCmds(int n) {
	char beginCmd = 0;
	struct cmd* head;

	for (int i=0;i<=n;++i){
		switch(cmdStr[i]) {
			case '&':{
				if (cmdStr[i+1] == '\n' || cmdStr[i+1] == ';') {
					cmdStr[i] = ' ';
					head->bgExec = 1;
				}
			}
			case ';':{
				beginCmd = 0;
				cmdStr[i] = '\0';
				cmdinfo[cmdNum++].end = i;
				break;
			}
			case '\n':{
				cmdStr[i] = '\0';
				cmdinfo[cmdNum++].end = i;
				return 0;
			}
			case ' ': break;
			default: if (!beginCmd) {
				beginCmd = 1;
				head = cmdinfo + cmdNum;
				cmdinfo[cmdNum].begin = i;
			}
		}
	}
}

int main() {
	char *args[128];
	while (1) {
		cmdNum = 0;
		varNum = 0;
		printf("^_^ ");
		fflush(stdin);
		fgets(cmdStr, MAX_CMD_LENGTH, stdin);
		int i=0;
		int n = strlen(cmdStr);
		parseCmds(n);
		
		for (i=0;i<cmdNum;++i) {
			struct cmd *pcmd = cmdinfo + i;
			
			if (!pcmd->args[0])
				continue;

			// 基本命令
			// if (strcmp(pcmd->args[0], "cd") == 0) {
			// 	if (pcmd->args[1])
			// 		chdir(pcmd->args[1]);
			// 	continue;
			// }
			if (strcmp(pcmd->args[0], "pwd") == 0) {
				char wd[4096];
				puts(getcwd(wd, 4096));
				continue;
			}
			if (strcmp(pcmd->args[0], "exit") == 0) {
				return 0;
			}

			// 较高级命令
			pid_t pid = fork();
			if (pid == 0) {
				execvp(pcmd->args[0], args);
				return 255;
			}
			wait(NULL);
		}
		
	}
}