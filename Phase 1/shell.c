#include<signal.h>
#include<stdio.h>
#include<string.h>
#include<error.h>
#include<stdlib.h>

#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#define BUFFER_LEN 100
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
   if(strcmp(line, "exit")==0){            //check if command is exit
        break;
    }
 strcpy(line1,line);
    char *token;                  //split command into separate strings
    token = strtok(line," ");
    int i=0;
    while(token!=NULL){
        argv[i]=token;      
        token = strtok(NULL," ");
        i++;
    }
    argv[i]=NULL;                     //set last value to NULL for execvp
    argc=i;                           //get arg count
    printf("%s\n",argv[0]);
    printf("%d\n",sizeof(argv[0]));
   // printf("%zd",fwrite(line,1,sizeof(line),fp)); 
    fprintf(fp,"%s",line);   
    fprintf(fp,"%s","\n");                  
     
  for(i=0; i<argc; i++){
        printf("%s\n", argv[i]);      //print command/args
    }
    strcpy(progpath, path);           //copy /bin/ to file path
    strcat(progpath, argv[0]);   //for example /bin/mkdir
   // printf("%s\n",progpath)   ;     //add program to path

   //Built ins are added separately
    if(strcmp(argv[0],"cd")==0)
    {
    	chdir(argv[1]);
    	printf("cd executed\n");
    }
    
    else if(strcmp(argv[0],"alias")==0)
    {
    	//chdir(argv[1]);
    	printf("alias executed\n");
    	char *token1;   
                 //split command into separate strings
  printf("%s\n",line1);
    token1 = strtok(line1,"=");
    int i=0;
    while(token1!=NULL){
        argv1[i]=token1;      
        token1 = strtok(NULL,"=");
        i++;
    }
    argv1[i]=NULL;                     //set last value to NULL for execvp
    printf("%s\n",argv1[0]);
    printf("%s\n",argv1[1]);
    }
    
    else
    {
    
    int pid= fork();              //fork child

    if(pid==0){               //Child
       
       
       
       
        execvp(progpath,argv);
        fprintf(stderr, "Child process could not do execvp\n");

    }else{                    //Parent
        wait(NULL);
        printf("Child exited\n");
    }
    
    }
}
}

