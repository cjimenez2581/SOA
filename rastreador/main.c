/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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

#include <string.h>

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
void push(Node ** head, int systemCallId);
void pushContent(Node** head, int systemCallId, char* name, char* parameters);
void increaseCount(Node** head, int systemCallId);
Node* getById(Node** head, int systemCallId);
void printList(Node * head);
void deleteList(Node** head);
void printHorizontal();
int mygetch();

//functions
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
    int ret, status;
    pid_t child_pid;
    
    //list
     Node * listProcesses = NULL;// malloc(sizeof(Node));
  
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

    //glue logic to assign 
    //ptrace_request_type assignment to ptrace_request
    //'v'-> PTRACE_SYSCALL
    //'V'-> PTRACE OPTION TO NOT HALT tracee process
    //processing logic could receive prog_value



    //processing logic

    printf ("the main program process ID is %d\n", (int) getpid());

    child_pid = fork(); //duplicate                                                                                                                                                
    if( child_pid == 0 ){
      //child: The return of fork() is zero                                                                                                                                    
      printf("Child: I'm the child: %d\n", child_pid);
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
      
      if(hasOptParam >= 1){
          execvp (argv[2], argv+3);
      }else{
          execvp (argv[1], argv+2);
      }
      

    }
    if (child_pid > 0){
      //parent: The return of fork() is the process of id of the child                                                                                                         
      while(waitpid(child_pid, &status, 0) && ! WIFEXITED(status)) {
          struct user_regs_struct regs;
          ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
          int systemCallId = CALL(regs);
          increaseCount(&listProcesses, systemCallId);
          if(__WORDSIZE == 32){
              //for 32 we can only show the system call ID
              fprintf(stderr, "system call %d from pid %d\n", systemCallId, child_pid);
          }else{
              Node* systemCall = getById(&listProcesses, systemCallId);
              
              fprintf(stderr, "system call: %s with Id: %d from pid %d\n", systemCall->name, systemCallId, child_pid);
          }
          
          if(ptrace_request_type == 'V'){
              waitForKey("Press any key to continue...\n");
          }
      
          ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
    }

    }else{
      //error: The return of fork() is negative                                                                                                                                

      perror("fork failed");
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
    newNode->name = NULL;
    newNode->parameters = NULL;
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

void printList(Node * head) {
    waitForKey("Press any key to continue and see the report\n");
    Node * current = head;
    printf("\n\n******** START REPORT ********\n");
    printf("ID-Count\tSignature\n");
    
    printHorizontal();
    while (current != NULL) {
        if(current->count > 0){
            printf("%d-%d\t    %s(%s)\n", current->systemCallId, current->count, current->name, current->parameters);
        }
        current = current->next;
    }
    printHorizontal();
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
    for(int i=0;i<30;i++){
        printf("%s", "*");
        if(i == 29){
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