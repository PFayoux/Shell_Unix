#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <readline/history.h>

#include "shell_fct.h"

#define  MYSHELL_CMD_OK 1
#define  MYSHELL_FCT_EXIT -1

int main(int argc, char** argv)
{
    //..........
    int ret = MYSHELL_CMD_OK;
    cmd mycmd;
    char* readlineptr;
    struct passwd* infos;
    char str[1024];
    char str2[256];
    char hostname[256];
    char workingdirectory[256];
    char temp[256];

    //..........
    while(ret != MYSHELL_FCT_EXIT)
    {
        infos=getpwuid(getuid());
        gethostname(hostname, 256);
        getcwd(workingdirectory, 256);

        sprintf(str2, "/home/%s", infos->pw_name);

        if(strstr(workingdirectory, str2)!=NULL){
            strcpy(temp,workingdirectory+strlen(str2));
            memset(workingdirectory,'\0',256);
            workingdirectory[0]='~';
            strcat(workingdirectory,temp);
        }

        sprintf(str, "\n\e[1;91m%s\e[1;33m@\e[1;92m%s:\e[1;94m\n%s$", infos->pw_name, hostname, workingdirectory);

        readlineptr = readline(str);
        if (!readlineptr)
            break;

        // Add input to history.
        add_history(readlineptr);

        //close current context
        if(strstr(readlineptr, "exit")!=NULL){
            ret=MYSHELL_FCT_EXIT;
        }

        parse_membres(readlineptr, &mycmd);
        parse_membres_args(&mycmd);
        parse_redirection(&mycmd);

        exec_commande(&mycmd, infos->pw_name);

        free_membres_args(&mycmd);
        free_redirection(&mycmd);
        free_membres(&mycmd);

        free(readlineptr);
        //..........

    }
    //..........
    return 0;
}
