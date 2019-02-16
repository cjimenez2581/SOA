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

#include <stdio.h>
#include <stdlib.h>

#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
 
int main (int argc, char** argv) 
{
    int ret, status;
    pid_t child_pid; 
 
    printf ("the main program process ID is %d\n", (int) getpid()); 
 
 
    child_pid = fork(); //duplicate                                                                                                                                                
    if( child_pid == 0 ){
      //child: The return of fork() is zero                                                                                                                                    
      printf("Child: I'm the child: %d\n", child_pid);
       static char *argv[]={"echo","Foo is my name.",NULL};
       execvp ("./child", argv);

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

