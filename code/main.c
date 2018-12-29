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

void init(struct cmd *pcmd) {
	pcmd->bgExec = 0;
	pcmd->argc = 0;
	pcmd->lredir = 0;
	pcmd->rredir = 0;
	pcmd->next = NULL;
	pcmd->begin = -1;
	pcmd->end = -1;
	for(int i=0;i<MAX_ARG_NUM;++i)
		pcmd->args[i] = NULL;
}

int getInput() {
	int pCmdStr = 0;
	int cur;
	char newline = 1;
	while(newline) {
		cur = MAX_CMD_LENGTH - pCmdStr;
		if (cur <= 0) {
			printf("[ERROR]: The cmd is too long to exec!\n");
			return -1;
		}
		fgets(cmdStr+pCmdStr, cur, stdin);
		newline = 0;
		while(1) {
			if (cmdStr[pCmdStr] == '\\' && cmdStr[pCmdStr+1] == '\n') {
				newline = 1;
				cmdStr[pCmdStr++] = '\0';
				break;
			} else if (cmdStr[pCmdStr] == '\n') {
				break;
			}
			++pCmdStr;
		}
	}
	return pCmdStr;
}

void parseCmds(int n) {
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
		int n = getInput();
		printf("%d\n", n);
		if (n <= 0)
			continue;
		int i=0;
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