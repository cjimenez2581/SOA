/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   child-test.c
 * Author: nosfe
 *
 * Created on February 14, 2019, 9:01 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>

/*
 * 
 */
int main(int argc, char** argv) {
    // Some arbitrary work done by the child

    printf ("Child: Hello, World!\n");
     printf ("Printing argsv:\n");

     for(int i=0;i<argc;i++)
    {
        printf("%s\n",argv[i]);
    }
    printf("Child: I'm the baby: %d\n", (int) getpid ());
    printf ("Child: Work completed!\n");
    printf ("Child: Bye now.\n");
    return (EXIT_SUCCESS);
}

