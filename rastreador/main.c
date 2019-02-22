/* 
 * File:   main.c
 * Author: nosfe
 *
 * Created on February 14, 2019, 8:16 PM
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h> 
#include <sys/types.h> 
#include <sys/user.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <termios.h>
#include <time.h>
#include <string.h>

#define MAX_STARS  90
//Determine 32/64 bits
#if __WORDSIZE == 64
#define CALL(reg) reg.orig_rax
#else
#define REG(reg) reg.orig_eax
#endif

typedef struct node {
    char* name;
    char* parameters;
    int systemCallId;
    int count;
    struct node * next;
} Node;
//functions
void push(Node ** head, int systemCallId);
void pushContent(Node** head, int systemCallId, char* name, char* parameters);
void increaseCount(Node** head, int systemCallId);
Node* getById(Node** head, int systemCallId);
char* getFormatFileName();
void printList(Node * head);
void deleteList(Node** head);
void printHorizontal();
int mygetch();
void waitForKey(char*);

int main (int argc, char **argv) 
{
    //input logic parameters/variables
    char ptrace_request_type ='\0';
    char *prog_value = NULL;
    char option='\0';
    
    //simulates boolean
    int parameters_read = 0;
    int hasOptParam = 0;

    //processing logic parameters/variables
    int status;
    pid_t child_pid;
    
    //list
     Node * listProcesses = NULL;
  
     //load system call names files
    FILE* file = fopen("systemCalls.csv", "r");//We got this file from
    //http://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/
    char line[1024];
    while (fgets(line, 1024, file)){
        int id =0;
        char* tmp = strdup(line);
        char* field = NULL;
        char* name = NULL;
        field = strsep(&tmp, "|");//https://stackoverflow.com/questions/31377038/split-a-char-array-into-separate-strings
        id = atoi(field);
        name = strsep(&tmp, "|");//name
        field = strsep(&tmp, "|");//content
        pushContent(&listProcesses, id, name, field);
    }
    
    fclose(file);
    //parameter input logic
    while ((option = getopt (argc, argv, "v:V:")) != -1 && parameters_read<1){
      parameters_read++;
      switch (option)
        {
        case 'v':
          ptrace_request_type = 'v';
          prog_value = optarg;
          hasOptParam++;
          break;
        case 'V':
          ptrace_request_type = 'V';
          prog_value = optarg;
          hasOptParam++;
          break;

        case '?':
          if (optopt == 'c')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
            fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
         
        default: //default values
          ptrace_request_type = 'v';
          prog_value = optarg;
          break;
      }
      
    }

    //print to debug input parameters/variables
    printf ("ptrace_request_type = %c, prog_value = %s\n",
          ptrace_request_type, prog_value);

    //processing logic

    printf ("The parent process ID is %d\n", (int) getpid());

    child_pid = fork(); //duplicate                                                                                                                                                
    if( child_pid == 0 ){
      //child: The return of fork() is zero
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      
      if(hasOptParam >= 1){
          execvp (argv[2], argv+3);
      }else{
          execvp (argv[1], argv+2);
      }
    }else if (child_pid > 0){
      //parent: The return of fork() is the process of id of the child                                                                                                         
      while(waitpid(child_pid, &status, 0) && ! WIFEXITED(status)) {
          struct user_regs_struct regs;
          ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
          int systemCallId = CALL(regs);
          increaseCount(&listProcesses, systemCallId);
          
          if(__WORDSIZE == 32){
              //for 32 we can only show the system call ID
              fprintf(stderr, "System call %d from pid %d\n", systemCallId, child_pid);
          }else{
              Node* systemCall = getById(&listProcesses, systemCallId);
              //this is not completely necessary, but it's safe to have it
              if(systemCall != NULL){
                  fprintf(stderr, "System call: %s with Id: %d from pid %d\n", systemCall->name, systemCallId, child_pid);
              }else{
                  fprintf(stderr, "System call with Id: %d from pid %d\n", systemCallId, child_pid);
              }
             
          }
          
          if(ptrace_request_type == 'V'){
              waitForKey("Press any key to continue...\n");
          }
      
          ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
    }

    }else{
      //error: The return of fork() is negative                                                                                                                                

      perror("Fork failed");
      _exit(EXIT_FAILURE); //exit failure, hard                                                                                                                                           

    }
    
    printList(listProcesses);
    deleteList(&listProcesses);
    return EXIT_SUCCESS; 
} 

void waitForKey(char* message) {
    if(message != NULL){
      printf("%s", message);
    }
    mygetch();
}

//adds at the end, normally use when a systemCallId is added but it's not on the file
void push(Node ** head, int systemCallId) {
    Node * newNode;
    newNode = malloc(sizeof(Node));
 
    newNode->systemCallId = systemCallId;
    newNode->count = 1;
    newNode->name = "Unknown name";
    newNode->parameters = "Unknown parameters";
    newNode->next = *head;
    (*head) = newNode;
}

void pushContent(Node** head, int systemCallId, char* name, char* parameters){
    Node * newNode;
    newNode = malloc(sizeof(Node));

    newNode->systemCallId = systemCallId;
    newNode->count = 0;//just filling it, it has not been called...yet or ever
    newNode->name = name;
    parameters[strlen(parameters)-1]=0;//removing the \n
    newNode->parameters = parameters;
    newNode->next = *head;
    (*head) = newNode;
}

void increaseCount(Node ** head, int systemCallId) {
    Node *current;
    int found = 0;
    if (*head == NULL) {
        push(head, systemCallId);
    }
    //firt element, so let's increase
    if ((*head)->systemCallId == systemCallId) {
        found++;
        int count = (*head)->count + 1;
        (*head)->count = count;
    }

    current = (*head)->next;
    while (current) {
        //found, so increase
        if (current->systemCallId == systemCallId) {
            found++;
            int count = current->count + 1;
            current->count = count;
            break;
        }
        current  = current->next;
    }
    //new element
    if(found == 0){
        push(head, systemCallId);
    }
}

Node* getById(Node** head, int systemCallId){
    Node *previous, *current;

    if (*head == NULL) {
        return NULL;
    }

    if ((*head)->systemCallId == systemCallId) {
        return *head;
    }

    previous = current = (*head)->next;
    while (current) {
        if (current->systemCallId == systemCallId) {
            return current;
        }

        previous = current;
        current  = current->next;
    }
    return NULL;
}

char* getFormatFileName(){
    char* format = "Report %d-%d-%d %d:%d:%d.txt";
    char* fileName = malloc (sizeof (char) * 40);;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(fileName, format, tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return fileName;
}

void printList(Node * head) {
    Node * current = head;
    FILE  *report;
    report= fopen(getFormatFileName(),"w");
    waitForKey("Press any key to continue and see the report\n");
    printf("\n\n\t******** START REPORT ********\n\n");
   
    if (report != NULL){
        fprintf(report, "%-15s %-15s %-20s %-50s\n","ID", "Count", "Name", "Parameters");
    }
    
    printf("%-15s %-15s %-20s %-50s\n","ID", "Count", "Name", "Parameters");
    
    while (current != NULL) {
        if(current->count > 0){
            printf("%-15d %-15d %-20s %-50s\n", current->systemCallId, current->count, current->name, current->parameters);
            if (report != NULL){
                fprintf(report, "%-15d %-15d %-20s %-50s\n",current->systemCallId, current->count, current->name, current->parameters);
            }
        }
        current = current->next;
    }
    
    if (fclose(report)!= 0) {
        printf( "Error closing the file Report\n" );
    }
}

void deleteList(Node** head){
    Node* current = *head; 
    Node* next; 
  
   while (current != NULL)  { 
       next = current->next; 
       free(current); 
       current = next; 
   }
   *head = NULL; 
}

void printHorizontal(){
    for(int i=0;i<MAX_STARS;i++){
        printf("%s", "*");
        if(i == MAX_STARS-1){
            printf("\n");
        }
    }
}


//Taken from https://faq.cprogramming.com/cgi-bin/smartfaq.cgi?id=1043284385&answer=1042856625
int mygetch ( ) 
{
  int ch;
  struct termios oldt, newt;
  
  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
  
  return ch;
}