//SLUSH - The SLU SHELL
//
//This assignment reads user input, and executes that input as a sequence of
//programs to execute.
//
//Program execution commands have the form:
//
//prog_n [args] ( ... prog_3 [args] ( prog_2 [args] ( prog_1 [args]
//
//For example, "grep slush ( sort -r ( ls -l -a" returns a reverse sorted list
//of all files in the current directory that contain the string "slush" in
//
//See the lab writeup for more details and other requirements.

//start of copied from Ferry




#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

#define bufferSize 255
char buffer [bufferSize];



void sh ( int signum ){
	printf("\n");
	//exit(EXIT_FAILURE);
	memset(buffer, 0, bufferSize);
}

int main( ) {
	while(1){
		memset(buffer, 0, bufferSize);
		void sh(int);
		signal(2, sh);

		fgets( buffer, bufferSize, stdin);
		int length = strlen( buffer );

		if(strncmp(buffer, "cd ", 3) == 0 ){
				char tempBuffer[255];
				int tempBufIt = 0;
				int anotherIt = 3;

				while(1){
					tempBuffer[tempBufIt] = buffer[anotherIt];
					if (tempBuffer[tempBufIt] == '\0')
					{
						break;
					}
					tempBufIt++;
					anotherIt++;
				}
				int tempLength = strlen( tempBuffer);
				tempBuffer[ tempLength- 1] = 0;
				tempLength = strlen(tempBuffer);
				int cdReturn = chdir(tempBuffer);
				if(cdReturn == -1){
					perror("Directory not found");
				}
				continue;
			}

		buffer[ length - 1] = 0;
		length = strlen( buffer );

		//Token parsing
		char* command;
		char* myargv[16];

		command = strtok( buffer, " " );
		if( command == NULL ){
			//After you have a while loop,
			//just continue here instead of exiting
			perror("Empty Command");
			continue;
		}

		myargv[0] = command;

		int i = 1;
		while(1){
			char* result = strtok( NULL, " ");
			if( result == NULL ){
				break;
			}
			myargv[i] = result;
			i++;
		}
		

		myargv[i] = NULL;
		int numArgs = i;


		int doublePipeChecker = 0;
		int pipeChecker = 0;

		for(int i = 0; i < numArgs; i++){
			if(strcmp(myargv[i], "(") == 0){
				pipeChecker++;
			}
			else{
				pipeChecker = 0;
			}
			if(pipeChecker == 2){
				doublePipeChecker = 1;
				break;
			}
		}
		if(doublePipeChecker == 1){
			perror("Invalid null command");
			continue;
		}

		//end of copied from Ferry

		char* commands[16][255];

		int argumentIndex = 0;
		int commandIndex = 0;
		for(int j = 0; j < numArgs; j++){//this loop goes through myargs and breaks it into commands, allowing for easier piping, as everything is stored in a 2D array
			if (strcmp(myargv[j], "(" ) == 0) {
				commands[commandIndex][argumentIndex] = NULL; 
				commandIndex++; 
				argumentIndex = 0;   
			} 
			else {
				commands[commandIndex][argumentIndex] = myargv[j]; 
				argumentIndex++;
			}
		}
		commands[commandIndex][argumentIndex] = NULL;


		for (int i = 0; i <= commandIndex; i++) {
			for (int j = 0; commands[i][j] != NULL; j++) {
			}
		}

		char* commandsCorrectOrder[16][255]; //need to reverse the commands as commands read right to left (via writeup)
		//Note, it would be possible to just pipe backwards, but I believe that would be way harder - Charlie
		int q = 0;
		for(int i = commandIndex; i >= 0; i--){
			for(int j = 0; j < 255; j++){
				if(commands[i][j] == NULL){
					q++;
					break;
				}
				else{
					commandsCorrectOrder[q][j] = commands[i][j];
				}
			}
		}
		commands[q][0] = NULL;
		for (int i = 0; i <= commandIndex; i++) {
			for (int j = 0; commandsCorrectOrder[i][j] != NULL; j++) {
			}
		}

		//start piping
		int oldFD;
		int pipeFD[2];

		int timesToIterate = commandIndex + 1;

		for(int i = 0; i < timesToIterate; i++){
			oldFD = pipeFD[0];
			char* currentCommand[255];
			char* currentCommandStart = commandsCorrectOrder[i][0];
			for(int j = 0; j < 255; j++){
				currentCommand[j] = commandsCorrectOrder[i][j];
				if(currentCommand[j] == NULL){
					break;
				}

			}
			
			if(i != timesToIterate){//first and middle
				int pipeRet = pipe(pipeFD);
				if(pipeRet == -1){
					perror("Error: Pipe Creation Failed\n");
					exit(EXIT_FAILURE);
				}
			}

			int ret = fork();
			
			if (ret == -1){
				perror("Fork Failed");
				exit(1);
			}
			if(ret == 0){//child


				if(i != timesToIterate - 1){//first or middle
					//redirect output to write end of pipe
					dup2(pipeFD[1], STDOUT_FILENO);
				}
				
				if(i != 0){//middle or last
					//redirect input from end of pipe
					dup2(oldFD, STDIN_FILENO);
				}

				if(i == timesToIterate){
					close(pipeFD[1]);
				}

				if(i == 0){
					close(pipeFD[0]);
				}

				int execReturn = execvp(currentCommandStart, currentCommand);	
				if(execReturn == -1){
					perror("prog1: Not found or Invalid null command\n");
					continue;
				}
			}

			if(i == timesToIterate -1){
				waitpid(ret,NULL,0);
			}

			if(i != timesToIterate - 1){
				close(pipeFD[1]);
				memset(currentCommand, 0, bufferSize);
				
			}
			if(i !=0 ){
				close(oldFD);
			}

		}

		for (int i = 0; i < 16; i++){
			memset(commands[i], 0, bufferSize);
			memset(commandsCorrectOrder[i], 0, bufferSize);
		}
		

	}	
	return 0;
}
