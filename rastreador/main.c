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

//Determine 32/64 bits
#if __WORDSIZE == 64
#define CALL(reg) reg.orig_rax
#else
#define REG(reg) reg.orig_eax
#endif

typedef struct node {
    int systemCallId;
    int count;
    struct node * next;
} Node;

struct Node *head = NULL;
struct Node *current = NULL;
void push(Node ** head, int systemCallId);
void increaseCount(Node ** head, int systemCallId);
void printList(Node * head);
void printHorizontal();

//functions
void waitForEnter(char*);
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
          //TODO add process name from ID
          fprintf(stderr, "system call %lld from pid %d\n", CALL(regs), child_pid);
          increaseCount(&listProcesses, CALL(regs));
          
          if(ptrace_request_type == 'V'){
              waitForEnter("Press Enter to continue...\n");
          }
      
          ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
    }

    }else{
      //error: The return of fork() is negative                                                                                                                                

      perror("fork failed");
      _exit(EXIT_FAILURE); //exit failure, hard                                                                                                                                           

    }
     printList(listProcesses);
    return EXIT_SUCCESS; 
} 

void waitForEnter(char* message) {
    if(message != NULL){
      printf("%s", message);
    }
    do {
        char buffer[2];
        fgets(buffer, sizeof(buffer), stdin); // waits for the enter key is pressed
    } while(0);
}

//adds at the end
void push(Node ** head, int systemCallId) {
    Node * newNode;
    newNode = malloc(sizeof(Node));

    newNode->systemCallId = systemCallId;
    newNode->count = 1;
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

void printList(Node * head) {
    waitForEnter("Press Enter to continue and see the report\n");
    Node * current = head;
    printf("\n\n******** START REPORT ********\n");
    printf("System Call ID\tTimes Called\n");
    
    printHorizontal();
    while (current != NULL) {
        //TODO add process name from ID
        printf("%d\t\t%d\n", current->systemCallId, current->count);
        current = current->next;
    }
    printHorizontal();
}

void printHorizontal(){
    for(int i=0;i<30;i++)
    {
        printf("%s", "*");
        if(i == 29){
            printf("\n");
        }
    }
}