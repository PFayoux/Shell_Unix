#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>//needed for open() and waitpid()
#include <sys/types.h>//needed for open() and waitpid()
#include <fcntl.h>//needed for O_WRONLY
#include <sys/wait.h>//needed for waitpid()
#include <glob.h>//for glob()
#include "cmd.h"



void stop_child(int signum) {
    printf("alarm : stop child process ID: %d\n", getpid());
    fflush(stdout);
    exit(0);
}

/**
*   qui prend une commande duement initialisée et qui effectue la création des pipes, les fork et les execs correspondants.
*
**/
void exec_commande(cmd * c, char * pw_name) {


    //initialization of 2 int used for loop
    int i=0;
    int j=0;

    pid_t child[c->nb_cmd_membres];//table of PID of child process
    int pipes[c->nb_cmd_membres-1][2];//nb_cmd_membres-1 pipe who will be useful for child process intercommunication

    //initialization of a file_descriptor useful for redirection
    int file_descriptor;

    char str[256];//string who contain the home path


    //create a pipe and a child process for the number of members in the command
    while(i<c->nb_cmd_membres) {


        //create a new pipe in the pipes table, don't create pipe for last child.
        if(i!=c->nb_cmd_membres-1) {
            pipe(pipes[i]);
        }


        //create a new child process , his pid is saved in the child table.
        child[i] = fork();
        if(child[i]==-1) {
            exit(EXIT_FAILURE);//in case of fork() fail
        }

        //executed piece of code of the child process
        if(child[i] == 0) {

            //close all the unused pipe, warning this is really important !
            j=0;
            while(j<i-1 ) {
                close(pipes[j][0]);
                close(pipes[j][1]);
                j++;

            }

            //if there is a file in input
            if(c->redirection[i][0]!=NULL && c->type_redirection[i][0]==0) {
                //the first child can't do that there is no pipes[-1]
                if(child[i]!=child[0]) {
                    close(pipes[i-1][0]);//close unused pipes
                    close(pipes[i-1][1]);
                }
                //open the file
                file_descriptor = open(c->redirection[i][0], O_RDONLY, 0774);
                dup2(file_descriptor,0);//close standard input and set file as input
            }
            else {
                //the first child can't do that there is no pipes[-1]
                if(child[i]!=child[0]) {
                    dup2(pipes[i-1][0],0);//close standart input and set pipes[i-1] as input
                    close(pipes[i-1][0]);//close unused pipes
                    close(pipes[i-1][1]);
                }
            }


            if(c->redirection[i][1]==NULL) {
                //the last child need to don't do that cause we don't want he change is output
                if(i!=c->nb_cmd_membres-1) {
                    dup2(pipes[i][1],1);//close standart output and set pipes[i] as output
                    close(pipes[i][0]);//close the unused pipe
                    close(pipes[i][1]);//close the unused pipe
                }
            }
            else {

                if(i!=c->nb_cmd_membres-1) {
                    close(pipes[i][0]);//close the unused pipe
                    close(pipes[i][1]);//close the unused pipe
                }
                //if it's an erasing writing O_TRUNC
                if(c->type_redirection[i][1]==0) {
                    file_descriptor = open(c->redirection[i][1], O_WRONLY | O_TRUNC | O_CREAT, 0774);
                    dup2(file_descriptor,1);//close standart output and set file as output
                }
                //if we write at the end of the file O_APPEND
                else if(c->type_redirection[i][1]==1) {
                    file_descriptor = open(c->redirection[i][1], O_WRONLY | O_APPEND | O_CREAT, 0774);
                    dup2(file_descriptor,1);//close standart output and set file as output
                }
            }


            if(c->redirection[i][2]!=NULL) {
                //if it's an erasing writing O_TRUNC
                if(c->type_redirection[i][2]==0) {
                    file_descriptor = open(c->redirection[i][2], O_WRONLY | O_TRUNC | O_CREAT, 0774);
                    dup2(file_descriptor,2);//close error output and set file as error output
                }
                //if we write at the end of the file O_APPEND
                else if(c->type_redirection[i][2]==1) {
                    file_descriptor = open(c->redirection[i][2], O_WRONLY | O_APPEND | O_CREAT, 0774);
                    dup2(file_descriptor,2);//close error output and set file as error output
                }
            }

            if(strcmp(c->cmd_membres_args[i][0],"cd")==0) {
                if(strcmp(c->cmd_membres_args[i][1],"~")==0) {
                    sprintf(str, "/home/%s", pw_name);
                    chdir(str);
                }
                else chdir(c->cmd_membres_args[i][1]);
            }
            //need to be the last thing cause replaces the current process image with a new process image, code after will not be executed.
            else {
                //if SIGUSR1 is set the process stop
                alarm(5);
                signal(SIGALRM, stop_child);
                    if(execvp(c->cmd_membres_args[i][0],c->cmd_membres_args[i])==-1) {
                        //ERROR OCCUR
                        perror("execvp");
                        exit(errno);
                    }
            }

        }

        i++;
    }

    //close all the unused pipes in the parent process
    i=0;
    while(i<c->nb_cmd_membres-1) {
        close(pipes[i][0]);
        close(pipes[i][1]);

        i++;
    }

    //wait for the children process to be finish
    i=0;
    while(i<c->nb_cmd_membres) {
        waitpid(child[i], NULL, 0);
        i++;
    }

}
