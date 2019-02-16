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
#include <unistd.h> 
#include <sys/wait.h>
 
int main (int argc, char **argv) 
{
    //input logic parameters/variables
    char ptrace_request_type ="";
    char *prog_value = NULL;
    char option="";
    int parameters_read = 0;


    //processing logic parameters/variables
    int ret, status;
    pid_t child_pid; 
 

    //parameter input logic
    while ((option = getopt (argc, argv, "v:V:")) != -1 && parameters_read<1){
      parameters_read++;
      switch (option)
        {
        case 'v':
          ptrace_request_type = 'v';
          prog_value = optarg;
          break;
        case 'V':
          ptrace_request_type = 'V';
          prog_value = optarg;
          break;

        case '?':
          if (optopt == 'c')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
            fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
            return 1;
         
        default:
          abort ();
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
       //static char *argv[]={"echo","Foo is my name.",NULL};
       execvp (argv[1], argv+1);

    }
    if (child_pid > 0){
      //parent: The return of fork() is the process of id of the child                                                                                                         

      printf("Parent: I'm the parent: %d\n", (int) getpid ());
       if ((ret = waitpid (child_pid, &status, 0)) == -1)
           printf ("parent:error\n");

      if (ret == child_pid)
          printf ("Parent: Child process waited for.\n");

    }else{
      //error: The return of fork() is negative                                                                                                                                

      perror("fork failed");
      _exit(EXIT_FAILURE); //exit failure, hard                                                                                                                                           

    }
    return EXIT_SUCCESS; 
} 

