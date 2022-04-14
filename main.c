#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_INPUT 131072

void ctrlc(int sig);

int main() {
	signal(SIGINT, ctrlc);
	
	char str[MAX_INPUT]; //входная строка
	char *argv[MAX_INPUT/2 + 1];
	
	int synch; //в зависимости от значения этой переменной, новый процесс может запускаться либо синхронно, либо асинхронно
	
	while(1) {
		synch = 1; //по умолчанию синхронно
		printf("$ ");
		if(fgets(str, MAX_INPUT, stdin) == NULL) { printf("fgets error\n"); continue; }
		if(strlen(str) == 1) continue; //если была введена пустая строка
		if(str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0'; //удаление символа новой строки
		if(str[strlen(str) - 1] == '&') { //если в конце строки символ '&', значит новый процесс нужно запускать асинхронно
			str[strlen(str) - 1] = '\0';
			synch = 0;
		}
		argv[0] = strtok(str, " "); //начало разбиения строки на лексемы и запись первой лексемы в массив
		
		if(!strcmp(argv[0], "exit")) break; //выход из цикла, если первое слово в строке - exit
		
		for(int i = 1; (argv[i] = strtok(NULL, " ")) != NULL; i++); //завершение разбиения строки на лексемы
		
		//реализация команды cd
		if(!strcmp(argv[0], "cd")) {
			if(chdir(argv[1])) printf("chdir error\n");
			continue;
		}
		
		//запуск нового процесса
		pid_t pid;
		pid = fork();
		switch(pid) {
		case -1:
			printf("fork failed\n");
			return 1;
		case 0:
			if(execvp(argv[0], argv)) printf("exec error\n");
			break;
		default:
			if(synch) waitpid(pid, NULL, 0);
			break;
		}
	}
	
	return 0;
}

void ctrlc(int sig) {}

