#include<signal.h>
#include<stdio.h>
#include<string.h>
#include<error.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include <termios.h>

#define BUFFER_LEN 100

static char* currentDirectory;
//Shell Prompt
void shellPrompt(){
	// We print the prompt in the form "<user>@<host> <cwd> >"
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}

//Replacing a word in a string with new word
char *replaceWord(const char *s, const char *oldW,
                                 const char *newW)
{
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++)
    {
        if (strstr(&s[i], oldW) == &s[i])
        {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }

    // Making new string of enough length
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);

    i = 0;
    while (*s)
    {
        // compare the substring with the result
        if (strstr(s, oldW) == s)
        {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }

    result[i] = '\0';
    return result;
}

//Handler for pipes
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

//File IO

void fileIO(char * args[], char* inputFile, char* outputFile, int option){

	int err = -1;
  pid_t pid;
	int fileDescriptor; // between 0 and 19, describing the output or input file

	if((pid=fork())==-1){
		printf("Child process could not be created\n");
		return;
	}
	if(pid==0){
		// Option 0: output redirection
		if (option == 0){
			// We open (create) the file truncating it at 0, for write only
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			// We replace de standard output with the appropriate file
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);
		// Option 1: input and output redirection
		}else if (option == 1){
			// We open file for read only (it's STDIN)
			fileDescriptor = open(inputFile, O_RDONLY, 0600);
			// We replace de standard input with the appropriate file
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			// Same as before for the output file
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);
		}

		setenv("parent",getcwd(currentDirectory, 1024),1);

		if (execvp(args[0],args)==err){
			printf("err");
			kill(getpid(),SIGTERM);
		}
	}
	waitpid(pid,NULL,0);
}


int main(){
    char line[BUFFER_LEN];  //get command line
    char line1[BUFFER_LEN];
    char *line2;
    line2=(char*)malloc(100*sizeof(char));
    char* argv[100];        //user command
    char* argv1[100];
    char* path= "/bin/";    //set path at bin
    //    char progpath[20];      //full file path
    char *progpath;
    progpath=(char*)malloc(100*sizeof(char));
    int argc;               //arg count
    FILE *fp=fopen("his.txt","w");
    FILE *pfile=fopen("aliasfile.txt","w");
     printf("\n\n");
     printf("\t\t**********************************************\n");
     printf("\t\t----------------Simple Shell-------------------\n ");
     printf("\t\t**********************************************\n");
     printf("\t\t    Copyright : Swati    Tanya     Tulika\n\n\n");
    currentDirectory = (char*) calloc(1024, sizeof(char));
    setenv("shell",getcwd(currentDirectory, 1024),1);
while(1){

   shellPrompt();  //print shell prompt

   if(!fgets(line, BUFFER_LEN, stdin))
    break;
	 FILE *fp=fopen("his.txt","a");
	 fprintf(fp,"%s",line);
	 fprintf(fp,"%s","\n");
	 fclose(fp);
   size_t length = strlen(line);
   if (line[length - 1] == '\n')
     line[length - 1] = '\0';                              //if user hits CTRL+D break
   if(strcmp(line, "exit")==0)
	  {            //check if command is exit
        break;
    }
    strcpy(line1,line); //Having a copy of input
    strcpy(line2,line);

    //Tokenize
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
    /*for(i=0; i<argc; i++)             // Printing all the tokens
		{
        printf("%s\n", argv[i]);      //print command/args
    }*/
    strcpy(progpath, path);           //copy /bin/ to file path
    //add program to path

    // We look for the special characters and separate the command itself
    // in a new array for the arguments
   char *argv_aux[256];
   int j=0;
   while ( argv[j] != NULL)
    {
     if ( (strcmp(argv[j],">") == 0) || (strcmp(argv[j],"<") == 0) || (strcmp(argv[j],"&") == 0)){
       break;
     }
     argv_aux[j] = argv[j];
     j++;
   }

   //Built ins are added separately
   //CD command
    if(strcmp(argv[0],"cd")==0)
    {
    	chdir(argv[1]);
    	printf("cd executed\n");
    }
		//History

	  else if(argv[0]==27)
		{
			printf("Up arrow");
		}
    //Editor
    else if(strcmp(argv[0],"editor")==0)
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
				char * command[1000];
				strcpy(command,argv1[1]);
				FILE *pfile=fopen("aliasfile.txt","a");
				//printf("%s",argv1[1]);
				fprintf(pfile,"%s",com_name[1]);
				fprintf(pfile,"%s","=");
				fprintf(pfile,"%s",command);
				fprintf(pfile,"%s","\n");
				fclose(pfile);

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
          else if (strcmp(argv[i],"<") == 0)
          {
      				int aux = i+1;
      				if (argv[aux] == NULL || argv[aux+1] == NULL || argv[aux+2] == NULL )
              {
      					printf("Not enough input arguments\n");
      					return -1;
      				}
              else
              {
      					  if (strcmp(argv[aux+1],">") != 0){
      						printf("Usage: Expected '>' and found %s\n",argv[aux+1]);
      						return -2;
      					}
      				}
      				fileIO(argv_aux,argv[i+1],argv[i+3],1);
      				return 1;
    			}
    			// If '>' is detected, we have output redirection.
    			// First we check if the structure given is the correct one,
    			// and if that is the case we call the appropriate method
    			else if (strcmp(argv[i],">") == 0)
          {
    				if (argv[i+1] == NULL)
            {
    					printf("Not enough input arguments\n");

    				}
    				fileIO(argv_aux,NULL,argv[i+1],0);

    			}
        i++;
     }

// No pipe found
      if(sig==0)
       {
         int alias_found=0;
        char *line=NULL;
        size_t len=0;
        ssize_t read;
        char *argv2[100];
        FILE *aliasfile=fopen("aliasfile.txt","r");
        while(read=getline(&line,&len,aliasfile)!=-1)
	      	{
    			    char *token;        //split command into separate strings
    			    token = strtok(line,"="); // the first token is captured here
    			    int i=0;
    			    while(token!=NULL) //All the tokens
    					{
        				argv2[i]=token;
        				token = strtok(NULL,"=");
        				i++;
    			    }
    			   argv2[i]=NULL;
      		  	if(strcmp(argv2[0],argv[0])==0)
      		  	{
      				alias_found=1;
      				//strcpy(argv,argv2[1]);
      		            //fork child
      		  	}
		     }
	if(alias_found==0)
	{

	        int pid= fork();
         if(pid==0)//child
           {
    	      	 strcat(progpath, argv[0]);
               execvp(progpath,argv);
               fprintf(stderr, "Child process could not do execvp\n");
           }
           else
           {                    //Parent
               wait(NULL);
               printf("Child exited\n");
           }
	}
	else
	{

        char *result = NULL;
        printf("%s",argv2[1]);
        argv2[1]=replaceWord(argv2[1],"\n","");
        result = replaceWord(line2,argv[0],argv2[1]);
        char *argv1[100];
        char *token;        //split command into separate strings
        token = strtok(result," "); // the first token is captured here
        int i=0;
        while(token!=NULL) //All the tokens
        {
            argv1[i]=token;
            token = strtok(NULL," ");
            i++;
        }
        argv1[i]=NULL;
        strcpy(progpath, path);           //copy /bin/ to file path
        strcat(progpath, argv1[0]);
        int pid= fork();
        //fork child
        if(pid==0)
        {               //Child
            execvp(progpath,argv1);
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

}
