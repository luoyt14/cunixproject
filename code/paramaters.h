#pragma once
#ifndef __PARAMATERS_H
#define __PARAMATERS_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 255
#define MAX_PATH_LENGTH 255
#define MAX_BUF_SIZE 4096
#define MAX_ARG_NUM 50
#define MAX_VAR_NUM 50
#define MAX_CMD_NUM 10
#define MAX_VAR_LENGTH 500
#define HISTORYFILE ".history"

#endif