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
char envVar[MAX_VAR_NUM][MAX_PATH_LENGTH];

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
	return 0;
}

int getItem(char *dst, char *src, int p) {
	int ct = 0;
	while (src[++p] == ' ');
	if (src[p] == '\n')
		return -1;
	char c;
	while ((c = dst[ct]) && (dst[ct] = src[p])) {
		if (c == ' ' || c == '|' || c == '<' || c == '>' || c == '\n')
			break;
		++ct;
		++p;
	}
	dst[ct] = '\0';
	return p-1;
}

int handleVar(struct cmd *pcmd, int n) {
	char *arg = pcmd->args[n];
	int p_arg = 0, p_var = 0;
	while (arg[p_arg]) {
		if ((arg[p_arg] == '$') && (arg[p_arg-1]!='\\')) {
			if(arg[p_arg+1] == '{')
				p_arg += 2;
			else
				p_arg += 1;
			char *tmp = &envVar[varNum][p_var];
			int ct = 0;
			while ((tmp[ct] = arg[p_arg])) {
				if (tmp[ct] == '}') {
					++ p_arg;
					break;
				}
				if (tmp[ct] == ' ' || tmp[ct] == '\n' || tmp[ct] == '\0')
					break;
				++ct;
				++p_arg;
			}

			tmp[ct] = '\0';
			tmp = getenv(tmp);
			int i = 0;
			while(1){
				if (!(envVar[varNum][p_var++]=tmp[i++]))
					break;
			}
			p_var -= 1;
		} else {
			envVar[varNum][p_var++] = arg[p_arg++];
		}
	}
	envVar[varNum][p_var] = '\0';
	pcmd->args[n] = envVar[varNum++];
	return 0;
}

int parseArgs() {
	char beginItem = 0;
	char beginQuote = 0;
	char beginDoubleQuote = 0;
	char hasVar = 0;
	char c;

	int begin, end;
	struct cmd* pcmd;

	for (int p=0;p<cmdNum;++p) {
		if (beginQuote || beginItem || beginDoubleQuote) {
			return -1;
		}
		pcmd = &cmdinfo[p];
		begin = pcmd->begin;
		end = pcmd->end;
		init(pcmd);

		for (int i=begin;i<end;++i) {
			c = cmdStr[i];
			if((c=='\'') && (cmdStr[i-1])!='\\' && (!beginQuote)) {
				if(beginDoubleQuote) {
					beginItem = 0;
					beginDoubleQuote = 0;
					cmdStr[i] = 0;
					if(hasVar) {
						hasVar = 0;
						handleVar(pcmd, pcmd->argc-1);
					}
				} else {
					beginDoubleQuote = 1;
					pcmd->args[pcmd->argc++] = cmdStr + i + 1;
				}
				continue;
			} else if (beginDoubleQuote) {
				if ((c=='$') && (cmdStr[i-1]!='\\') && (!hasVar))
					hasVar = 1;
				continue;
			}

			if((c=='\'') && (cmdStr[i-1]!='\\')) {
				if(beginQuote) {
					beginItem = 0;
					beginQuote = 0;
					cmdStr[i] = 0;
				} else {
					beginQuote = 1;
					pcmd->args[pcmd->argc++] = cmdStr + i + 1;
				}
				continue;
			} else if(beginQuote) {
				continue;
			}

			if(c=='<' || c=='>' || c=='|') {
				if(beginItem)
					beginItem = 0;
				cmdStr[i] = '\0';
			}
			if(c == '<') {
				if (cmdStr[i+1] == '<') {
					pcmd->lredir += 2;
					cmdStr[i+1] = ' ';
				} else {
					pcmd->lredir += 1;
				}

				int tmp = getItem(pcmd->fromFile, cmdStr, i);
				if (tmp > 0)
					i = tmp;
			} else if (c == '>') {
				if (cmdStr[i+1] == '>') {
					pcmd->rredir += 2;
					cmdStr[i+1] = ' ';
				} else {
					pcmd->rredir += 1;
				}
				int tmp = getItem(pcmd->toFile, cmdStr, i);
				if (tmp > 0)
					i = tmp;
			} else if (c == '|') {
				pcmd->end = i;
				pcmd->next = (struct cmd*)malloc(sizeof(struct cmd));
				pcmd = pcmd->next;
				init(pcmd);
			} else if (c == ' ' || c == '\0') {
				if (beginItem) {
					beginItem = 0;
					cmdStr[i] = '\0';
				}
			} else {
				if (pcmd->begin == -1)
					pcmd->begin = i;
				if (!beginItem) {
					beginItem = 1;
					if((c=='$') && (cmdStr[i-1]=='\\') && (!hasVar))
						hasVar = 1;
					pcmd->args[pcmd->argc++] = cmdStr + i;
				}
			}
			if (hasVar) {
				hasVar = 0;
				handleVar(pcmd, pcmd->argc-1);
			}
		}
		pcmd->end = end;
	}
	return 0;
}

int execInner(struct cmd* pcmd) {
	if (!pcmd->args[0]) 
		return 0;
	if (strcmp(pcmd->args[0], "cd") == 0) {
		struct stat st;
		if (pcmd->args[1]) {
			stat(pcmd->args[1], &st);
			if (S_ISDIR(st.st_mode))
				chdir(pcmd->args[1]);
			else {
				printf("[ERROR]: cd %s: No such directory!\n", pcmd->args[1]);
				return -1;
			}
		}
		//TODO: 没有参数时cd到家目录
		return 0;
	}
	if (strcmp(pcmd->args[0], "pwd") == 0) {
		printf("%s\n", getcwd(pcmd->args[1], MAX_PATH_LENGTH));
		return 0;
	}
	if (strcmp(pcmd->args[0], "unset") == 0) {
		for (int i=0;i<pcmd->argc;++i) {
			unsetenv(pcmd->args[i]);
			return 0;
		}
	}
	if (strcmp(pcmd->args[0], "export") == 0) {
		for (int i=1;i<pcmd->argc;++i) {
			char *val, *p;
			for(p=pcmd->args[i];*p!='=';++p)
				;
			*p='\0';
			val = p + 1;
			setenv(pcmd->args[i], val, 1);
		}
		return 0;
	}
	if (strcmp(pcmd->args[0], "exit") == 0) {
		exit(0);
	}
	return 1;
}

int main() {
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
			// if (strcmp(pcmd->args[0], "pwd") == 0) {
			// 	char wd[4096];
			// 	puts(getcwd(wd, 4096));
			// 	continue;
			// }
			// if (strcmp(pcmd->args[0], "exit") == 0) {
			// 	return 0;
			// }

			// // 较高级命令
			// pid_t pid = fork();
			// if (pid == 0) {
			// 	execvp(pcmd->args[0], args);
			// 	return 255;
			// }
			// wait(NULL);
		}
		
	}
}