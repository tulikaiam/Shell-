#include<signal.h>
#include<stdio.h>
#include<string.h>
#include<error.h>
#include<stdlib.h>

#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#define BUFFER_LEN 100

void pipeHandler(char * args[]){
	// File descriptors
	int filedes[2]; // pos. 0 output, pos. 1 input of the pipe
	int filedes2[2];

	int num_cmds = 0;

	char *command[256];

	pid_t pid;

	int err = -1;
	int end = 0;

	// Variables used for the different loops
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;

	// First we calculate the number of commands (they are separated
	// by '|')
	while (args[l] != NULL){
		if (strcmp(args[l],"|") == 0){
			num_cmds++;
		}
		l++;
	}
	num_cmds++;

	// Main loop of this method. For each command between '|', the
	// pipes will be configured and standard input and/or output will
	// be replaced. Then it will be executed
	while (args[j] != NULL && end != 1){
		k = 0;
		// We use an auxiliary array of pointers to store the command
		// that will be executed on each iteration
		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;
			if (args[j] == NULL){
				// 'end' variable used to keep the program from entering
				// again in the loop when no more arguments are found
				end = 1;
				k++;
				break;
			}
			k++;
		}
		// Last position of the command will be NULL to indicate that
		// it is its end when we pass it to the exec function
		command[k] = NULL;
		j++;

		// Depending on whether we are in an iteration or another, we
		// will set different descriptors for the pipes inputs and
		// output. This way, a pipe will be shared between each two
		// iterations, enabling us to connect the inputs and outputs of
		// the two different commands.
		if (i % 2 != 0){
			pipe(filedes); // for odd i
		}else{
			pipe(filedes2); // for even i
		}

		pid=fork();

		if(pid==-1){
			if (i != num_cmds - 1){
				if (i % 2 != 0){
					close(filedes[1]); // for odd i
				}else{
					close(filedes2[1]); // for even i
				}
			}
			printf("Child process could not be created\n");
			return;
		}
		if(pid==0){
			// If we are in the first command
			if (i == 0){
				dup2(filedes2[1], STDOUT_FILENO);
			}
			// If we are in the last command, depending on whether it
			// is placed in an odd or even position, we will replace
			// the standard input for one pipe or another. The standard
			// output will be untouched because we want to see the
			// output in the terminal
			else if (i == num_cmds - 1){
				if (num_cmds % 2 != 0){ // for odd number of commands
					dup2(filedes[0],STDIN_FILENO);
				}else{ // for even number of commands
					dup2(filedes2[0],STDIN_FILENO);
				}
			// If we are in a command that is in the middle, we will
			// have to use two pipes, one for input and another for
			// output. The position is also important in order to choose
			// which file descriptor corresponds to each input/output
			}else{ // for odd i
				if (i % 2 != 0){
					dup2(filedes2[0],STDIN_FILENO);
					dup2(filedes[1],STDOUT_FILENO);
				}else{ // for even i
					dup2(filedes[0],STDIN_FILENO);
					dup2(filedes2[1],STDOUT_FILENO);
				}
			}

			if (execvp(command[0],command)==err){
				kill(getpid(),SIGTERM);
			}
		}

		// CLOSING DESCRIPTORS ON PARENT
		if (i == 0){
			close(filedes2[1]);
		}
		else if (i == num_cmds - 1){
			if (num_cmds % 2 != 0){
				close(filedes[0]);
			}else{
				close(filedes2[0]);
			}
		}else{
			if (i % 2 != 0){
				close(filedes2[0]);
				close(filedes[1]);
			}else{
				close(filedes[0]);
				close(filedes2[1]);
			}
		}

		waitpid(pid,NULL,0);

		i++;
	}
}


int main(){
    char line[BUFFER_LEN];  //get command line
    char line1[BUFFER_LEN];
    char* argv[100];        //user command
    char* argv1[100];
    char* path= "/bin/";    //set path at bin
    char progpath[20];      //full file path
    int argc;               //arg count
FILE *fp=fopen("/home/student/his.txt","a");
while(1){

   printf("Shell \xC9 01FB15ECS320|324|327>> ");                   //print shell prompt

   if(!fgets(line, BUFFER_LEN, stdin))
    break;
   size_t length = strlen(line);
   if (line[length - 1] == '\n')
     line[length - 1] = '\0';                              //if user hits CTRL+D break
   if(strcmp(line, "exit")==0)
	  {            //check if command is exit
        break;
    }
    strcpy(line1,line); //Having a copy of input
    char *token;        //split command into separate strings
    token = strtok(line," "); // the first token is captured here
    int i=0;
    while(token!=NULL) //All the tokens
		{
        argv[i]=token;
        token = strtok(NULL," ");
        i++;
    }
    argv[i]=NULL;                     //set last value to NULL for execvp
    argc=i;                           //get arg count
    //printf("%s\n",argv[0]);
    //printf("%d\n",sizeof(argv[0]));
   // printf("%zd",fwrite(line,1,sizeof(line),fp));
    //fprintf(fp,"%s",line);
    //fprintf(fp,"%s","\n");
    for(i=0; i<argc; i++)             // Printing all the tokens
		{
        printf("%s\n", argv[i]);      //print command/args
    }
    strcpy(progpath, path);           //copy /bin/ to file path
    strcat(progpath, argv[0]);        //for example /bin/mkdir
   // printf("%s\n",progpath)   ;     //add program to path

   //Built ins are added separately
	 //CD command
    if(strcmp(argv[0],"cd")==0)
    {
    	chdir(argv[1]);
    	printf("cd executed\n");
    }

    //Editor
    else if(strcmp(argv[0],"edit")==0)
    {
      printf("Executing your Python script...");
    	system("python editor_new.py");
    }
		//Aliasing -->alias ll= "ls -al"
    else if(strcmp(argv[0],"alias")==0)
    {
        	//chdir(argv[1]);
        printf("alias executed\n");
        char *token1;
                     //split command into separate strings
        printf("%s\n",line1);
        token1 = strtok(line1,"=");
        int i=0;
        while(token1!=NULL)
				{
            argv1[i]=token1;
            token1 = strtok(NULL,"=");
            i++;
        }
        argv1[i]=NULL;                     //set last value to NULL for execvp
        printf("%s\n",argv1[0]);
        printf("%s\n",argv1[1]);

				char * com_name[200];
				char * a_com=strtok(argv1[0]," ");
				i=0;
				while(a_com!=NULL)
				{
						com_name[i]=a_com;
						a_com = strtok(NULL," ");
						i++;
				}
				//printf("%s\n",com_name[1]); ll (Short name)
				//printf("%s\n",argv1[1]); actual command to be executed ..ex -ls -l

			  FILE *pfile=fopen("aliasfile.txt","w");
				//printf("%s",argv1[1]);
				fprintf(pfile,argv1[1]);

	}
  //Pipe and Normal execution
   else
    {
      i=0;
      int sig=0;
  		//Let's check for | symbol
  		while (argv[i] != NULL)
      {
        //Sample pipe command cat shell.c | grep 327 ---Line number where 327 is present
    	    if (strcmp(argv[i],"|") == 0)
          {
              sig=1;
      				pipeHandler(argv);
          }
  			// If '>' is detected, we have output redirection.
  			// First we check if the structure given is the correct one,
  			// and if that is the case we call the appropriate method
        i++;
     }
		 // No pipe found
      if(sig==0)
       {
          int pid= fork();              //fork child
           if(pid==0)
           {               //Child
               execvp(progpath,argv);
               fprintf(stderr, "Child process could not do execvp\n");
           }
           else
           {                    //Parent
               wait(NULL);
               printf("Child exited\n");
           }
       }
  	}

}
}
