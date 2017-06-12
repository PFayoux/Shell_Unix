#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "cmd.h"
#include <errno.h>
#include <string.h>

/*
typedef struct commande {*/
/*  char * cmd_initiale;              *//* La chaine initiale tapee par l'utilisateur */
/*  char **cmd_membres;               *//* a chaque case i chaine du membre i */
/*  unsigned int nb_cmd_membres;      *//* Nombre de membres */
/*  char ***cmd_membres_args;         *//* cmd_membres_args[i][j] contient le jieme mot du ieme membre */
/*  unsigned int *nb_membres_args;    *//* Nombre d'arguments par membres */
/*  char ***redirection;              *//* Pour stocker le chemin vers le fichier de redirection */
/*  int **type_redirection;           *//* Pour stocker le type de redirection */
/*} cmd;                                */


/**
*   Affiche les arguments des membres de la commande
*
**/

void aff_membres_args(cmd *c) {

    int index_membres = 0;
    int index_arg = 0;
    /*parcours tout le tableaux des arguments et libère toute la mémoire allouée*/
    while(index_membres<c->nb_cmd_membres) {
        index_arg = 0;
        while(index_arg<c->nb_membres_args[index_membres]) {
            if(c->cmd_membres_args[index_membres][index_arg]!=NULL) {
                printf("%s\n",c->cmd_membres_args[index_membres][index_arg]);
            }
            index_arg++;
        }
        index_membres++;
    }


}
void free_membres_args(cmd *c) {
    int index_membres = 0;
    int index_arg = 0;
    /*parcours tout le tableaux des arguments et libère toute la mémoire allouée*/
    while(index_membres<c->nb_cmd_membres) {
        index_arg = 0;
        while(index_arg<c->nb_membres_args[index_membres]) {
            if(c->cmd_membres_args[index_membres][index_arg]!=NULL) {
                free(c->cmd_membres_args[index_membres][index_arg]);
            }
            index_arg++;
        }
        index_membres++;
    }

}

void save_membres_args(cmd * c, char * buffer_membre_arg, int index_buf, int index_membres) {

    char ** pointer_temp;

    /*allocation of the necessary memory for cmd_membres_args depending of the number of arguments of the current member*/
    pointer_temp=(char**)realloc(c->cmd_membres_args[index_membres],sizeof(char*)*(c->nb_membres_args[index_membres]+1));
    if(pointer_temp!=NULL) {
        c->cmd_membres_args[index_membres]=pointer_temp;
    }
    if(c->cmd_membres_args[index_membres]==NULL) {
        perror("allocation problems for (c->cmd_membres_args) in parse_membres_args fct");
    }

    /*close the string of the buffer*/
    buffer_membre_arg[index_buf]='\0';

    /*allocation of the memory necessary for an argument*/
    if(strlen(buffer_membre_arg)!=0) {
        c->cmd_membres_args[index_membres][c->nb_membres_args[index_membres]]=(char*)malloc(sizeof(char)*(strlen(buffer_membre_arg)+1));
        if(c->cmd_membres_args==NULL) {
            perror("allocation problems for (*c->cmd_membres_args) in parse_membres_args fct");
        }
        /*save the value of the argument*/
        strcpy(c->cmd_membres_args[index_membres][c->nb_membres_args[index_membres]],buffer_membre_arg);
    }
    else {
        /*if the buffer is empty, there is no argument */
        c->cmd_membres_args[index_membres][c->nb_membres_args[index_membres]]=NULL;
    }
    /*increment the numbers of args*/
    c->nb_membres_args[index_membres]++;

}

/**
*   Remplit les champs cmd_args et nb_args_membres
*   cette fonction impose le respect d'une syntaxe particulière pour le shell
*   toute redirection doit être suivie d'un espace
**/
int parse_membres_args(cmd *c) {

    int index=0;
    int index_membres=0;
    int index_buf=0;

    char buffer_membre_arg[256];

    memset(buffer_membre_arg, '\0',256);

    c->cmd_membres_args=(char***)malloc(sizeof(char**)*(c->nb_cmd_membres));
    if(c->cmd_membres_args==NULL) {
        perror("allocation problems for (c->cmd_membres_args) in parse_membres_args fct");
        return -1;
    }

    c->nb_membres_args=(unsigned int*)malloc(sizeof(unsigned int)*(c->nb_cmd_membres));
    if(c->nb_membres_args==NULL) {
        perror("allocation problems for (c->cmd_membres_args) in parse_membres_args fct");
        return -1;
    }
    /*count the number of membres*/
    while(index_membres<c->nb_cmd_membres) {

        c->nb_membres_args[index_membres]=0;
        index=0;
        c->cmd_membres_args[index_membres]=NULL;

        while(c->cmd_membres[index_membres][index]!='\0' && c->cmd_membres[index_membres][index]!='<' &&
                c->cmd_membres[index_membres][index]!='>' && !(c->cmd_membres[index_membres][index]=='2' && c->cmd_membres[index_membres][index+1]=='>')) {

            //if there is a filename between quotes
            if(c->cmd_membres[index_membres][index]=='"' && buffer_membre_arg[0]=='\0'){
                index=index+1;//go to next char
                while(c->cmd_membres[index_membres][index]!='\0' && c->cmd_membres[index_membres][index]!='"'){
                    buffer_membre_arg[index_buf]=c->cmd_membres[index_membres][index];
                    index_buf++;
                    index++;
                }
                index++;//go out the " char
                save_membres_args(c, buffer_membre_arg, index_buf, index_membres);
                index_buf=0;/*reinitialise index_buf for the next use of buffer_membre_arg*/
                memset(buffer_membre_arg, '\0',256);
            }

            if(c->cmd_membres[index_membres][index]!=' ')
            {

                buffer_membre_arg[index_buf]=c->cmd_membres[index_membres][index];
                index_buf++;
            }
            else if(buffer_membre_arg[0]!='\0') {
                save_membres_args(c, buffer_membre_arg, index_buf, index_membres);
                index_buf=0;/*reinitialise index_buf for the next use of buffer_membre_arg*/
                memset(buffer_membre_arg, '\0',256);
            }
            else if(c->nb_membres_args[index_membres]!=0) {
                c->cmd_membres_args[index_membres][c->nb_membres_args[index_membres]]=NULL;
            }
            index++;
        }
        if(buffer_membre_arg[0]!='\0') {
            save_membres_args(c, buffer_membre_arg, index_buf, index_membres);
            index_buf=0;/*reinitialise index_buf for the next use of buffer_membre_arg*/
            memset(buffer_membre_arg, '\0',256);
        }
        //put null at the end
        save_membres_args(c, buffer_membre_arg, index_buf, index_membres);
        c->nb_membres_args[index_membres]--;
        index_membres++;
    }

    return 0;

}

/**
*   Remplit les champs initial_cmd, membres_cmd et nb_membres.
*
**/
void parse_membres(char *chaine,cmd *ma_cmd) {

    size_t nb_cmd_membres=0;
    size_t index=0;
    char *token;
    const char char_pipe = '|';
    char * cpy_chaine = (char*)malloc(sizeof(char)*strlen(chaine)+1);
    if(cpy_chaine==NULL) {
        perror("allocation problems for ma_cmd->cmd_membres in parse_membres fct");
    }
    memset(cpy_chaine,'\0',strlen(chaine)+1);
    strcpy(cpy_chaine,chaine);

    /*save the initial cmd*/
    ma_cmd->cmd_initiale=chaine;

    /*count the number of membres*/
    while(chaine[index]!='\0') {
        if (chaine[index]==char_pipe) {
            nb_cmd_membres++;
        }
        index++;
    }
    nb_cmd_membres++;

    /*save nb of members*/
    ma_cmd->nb_cmd_membres=nb_cmd_membres;


    /* allocation of cmd_members*/
    ma_cmd->cmd_membres=(char**)malloc(sizeof(char*)*nb_cmd_membres);
    if(ma_cmd->cmd_membres==NULL) {
        perror("allocation problems for ma_cmd->cmd_membres in parse_membres fct");
    }

    /* get the first token */
    token = strtok(cpy_chaine, &char_pipe);

    /* walk through other tokens */
    index=0;

    /*if last strtok is not null*/
    if(token !=NULL) {
        /* while there's still some members not set*/
        while( token != NULL ) {
            /*allocation of the members*/
            (ma_cmd->cmd_membres[index])=(char*)malloc(sizeof(char)*strlen(token)+1);
            if(ma_cmd->cmd_membres[index]==NULL) {
                perror("allocation problems for (*ma_cmd->cmd_membres) in parse_membres fct");
            }
            /*save the members*/
            memset(ma_cmd->cmd_membres[index],'\0',strlen(token)+1);
            strcpy((ma_cmd->cmd_membres[index]),token);
            /*go to the next members*/
            token = strtok(NULL, &char_pipe);

            /*go to next index of membre*/
            index++;
        }
    }
    else {
        /*sinon NULL pour éviter des erreurs de free*/
        ma_cmd->cmd_membres[0]=NULL;
        ma_cmd->nb_cmd_membres=0;
    }
    /*free pointer when we don't need it anymore*/
    free(cpy_chaine);

}


void aff_membres(cmd *ma_cmd) {

    int index_membres=0;

    /*print all the members*/
    while(index_membres<ma_cmd->nb_cmd_membres && ma_cmd->cmd_membres[index_membres]!=NULL) {
        printf("%s\n",ma_cmd->cmd_membres[index_membres]);
        index_membres++;
    }

}
void free_membres(cmd *ma_cmd) {

    int index_membres=0;

    /*free all the members*/
    while(index_membres<ma_cmd->nb_cmd_membres) {
        if(ma_cmd->cmd_membres[index_membres]!=NULL) {
            free(ma_cmd->cmd_membres[index_membres]);
        }
        index_membres++;
    }
    /*free the table of members*/
    if(ma_cmd->cmd_membres!=NULL) {
        free(ma_cmd->cmd_membres);
    }



}

void save_redirection(cmd * c, int index_buf, char * buffer_fichier, int index_membres, int i) {

    /*close the string of the buffer*/
    buffer_fichier[index_buf]='\0';

    c->redirection[index_membres][i]=NULL;/*if the buffer is empty, there is no argument */

    /*allocation of the memory necessary for a filename*/
    if(strlen(buffer_fichier)!=0) {
        c->redirection[index_membres][i]=(char*)malloc(sizeof(char)*(strlen(buffer_fichier)+1));
        if(c->redirection==NULL) {
            perror("allocation problems for (*c->cmd_membres_args) in parse_membres_args fct");
        }
        /*save the value of the argument*/
        strcpy(c->redirection[index_membres][i],buffer_fichier);
    }



}

/**
*   Remplit les champs redir et type_redir du membre i
*
**/
void parse_redirection(cmd *c) {

    int index_redir = 0;//parcours le tableau de redirection
    int index=0;//parcours les chaines
    int index_membres=0;//parcours les membres
    int index_buf=0;//parcours le buffer
    int last_redirection = -1;//dernier redirection rencontrer

    char buffer_fichier[256];
    memset(buffer_fichier, '\0',256);

    c->redirection=(char***)malloc(sizeof(char**)*(c->nb_cmd_membres));
    if(c->redirection==NULL) {
        perror("allocation problems for (c->redirection) in parse_redirection fct");
    }

    c->type_redirection=(int**)malloc(sizeof(int*)*(c->nb_cmd_membres));
    if(c->redirection==NULL) {
        perror("allocation problems for (c->type_redirection) in parse_redirection fct");
    }


    /*count the number of membres*/
    while(index_membres<c->nb_cmd_membres) {

        index=0;
        c->redirection[index_membres]=NULL;
        c->redirection[index_membres]=(char**)malloc(sizeof(char*)*3);
        if(c->redirection==NULL) {
            perror("allocation problems for (c->redirection) in parse_redirection fct");
        }

        c->type_redirection[index_membres]=(int*)malloc(sizeof(int)*3);
        if(c->redirection==NULL) {
            perror("allocation problems for (c->type_redirection) in parse_redirection fct");
        }

        while(index_redir<=2) {
            c->type_redirection[index_membres][index_redir]=-1;
            index_redir++;
        }
        index_redir=0;
        /*initialization of the pointer to NULL, avoid error later*/
        c->redirection[index_membres][0]=NULL;
        c->redirection[index_membres][1]=NULL;
        c->redirection[index_membres][2]=NULL;


        while(index<=strlen(c->cmd_membres[index_membres])) {

            if(c->cmd_membres[index_membres][index]==' ') {
                index++;
            }

            if(c->cmd_membres[index_membres][index]=='<') {

                if(c->type_redirection[index_membres][0]!=-1 && last_redirection==0 && c->redirection[index_membres][0]==NULL) {
                    save_redirection(c, index_buf, buffer_fichier, index_membres, 0);
                    memset(buffer_fichier, '\0',256);
                    index_buf=0;
                    printf("Attention vous avez déclarer plusieurs fichiers en entrée, seul le premier sera pris en compte");
                }
                else if(c->type_redirection[index_membres][1]!=-1 && last_redirection==1 && c->redirection[index_membres][1]==NULL) {
                    save_redirection(c, index_buf, buffer_fichier, index_membres, 1);
                    memset(buffer_fichier, '\0',256);
                    index_buf=0;
                }
                else if(c->type_redirection[index_membres][2]!=-1 && last_redirection==2 && c->redirection[index_membres][2]==NULL) {
                    save_redirection(c, index_buf, buffer_fichier, index_membres, 2);
                    memset(buffer_fichier, '\0',256);
                    index_buf=0;
                }
                last_redirection=0;
                c->type_redirection[index_membres][0]=0;
            }
            else if(c->cmd_membres[index_membres][index]=='2' && c->cmd_membres[index_membres][index+1]=='>') {
                if(c->cmd_membres[index_membres][index+2]=='>') {

                    if(c->type_redirection[index_membres][0]!=-1 && last_redirection==0 && c->redirection[index_membres][0]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 0);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                    }
                    else if(c->type_redirection[index_membres][1]!=-1 && last_redirection==1 && c->redirection[index_membres][1]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 1);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                    }
                    else if(c->type_redirection[index_membres][2]!=-1 && last_redirection==2 && c->redirection[index_membres][2]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 2);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                        printf("Attention vous avez déclarer plusieurs fichiers en sortie d'erreur, seul le premier sera pris en compte");
                    }
                    index=index+2;
                    c->type_redirection[index_membres][2]=1;//if there is 2>> it's normal writing output we set 1
                    last_redirection=2;
                }
                else {

                    if(c->type_redirection[index_membres][0]!=-1 && last_redirection==0 && c->redirection[index_membres][0]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 0);
                        memset(buffer_fichier, '\0',256);
                    }
                    else if(c->type_redirection[index_membres][1]!=-1 && last_redirection==1 && c->redirection[index_membres][1]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 1);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                    }
                    else if(c->type_redirection[index_membres][2]!=-1 && last_redirection==2 && c->redirection[index_membres][2]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 2);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                        printf("Attention vous avez déclarer plusieurs fichiers en sortie d'erreur, seul le premier sera pris en compte");
                    }
                    last_redirection=2;
                    c->type_redirection[index_membres][2]=0;//if there is 2> it's normal writing output we set 1
                }
            }
            else if(c->cmd_membres[index_membres][index]=='>') {
                if(c->cmd_membres[index_membres][index+1]=='>') {
                    if(c->type_redirection[index_membres][0]!=-1 && last_redirection==0 && c->redirection[index_membres][0]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 0);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                    }
                    else if(c->type_redirection[index_membres][1]!=-1 && last_redirection==1 && c->redirection[index_membres][1]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 1);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                        printf("Attention vous avez déclarer plusieurs fichiers en sortie, seul le premier sera pris en compte");
                    }
                    else if(c->type_redirection[index_membres][2]!=-1 && last_redirection==2 && c->redirection[index_membres][2]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 2);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                    }

                    index=index+1;
                    last_redirection=1;
                    c->type_redirection[index_membres][1]=1;//if there is >> it's normal writing output we set 1
                }
                else {

                    if(c->type_redirection[index_membres][0]!=-1 && last_redirection==0 && c->redirection[index_membres][0]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 0);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;

                    }
                    else if(c->type_redirection[index_membres][1]!=-1 && last_redirection==1 && c->redirection[index_membres][1]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 1);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                        printf("Attention vous avez déclarer plusieurs fichiers en sortie, seul le premier sera pris en compte");

                    }
                    else if(c->type_redirection[index_membres][2]!=-1 && last_redirection==2 && c->redirection[index_membres][2]==NULL) {
                        save_redirection(c, index_buf, buffer_fichier, index_membres, 2);
                        memset(buffer_fichier, '\0',256);
                        index_buf=0;
                    }
                    last_redirection=1;
                    c->type_redirection[index_membres][1]=0;//if there is > it's normal writing output we set 1
                }
            }
            //if there is still some characterrs after we have set every redirection we stop
            else if(c->cmd_membres[index_membres][index]!=' ' && (c->type_redirection[index_membres][0]!=-1 || c->type_redirection[index_membres][1]!=-1 || c->type_redirection[index_membres][2]!=-1)) {
                buffer_fichier[index_buf]=c->cmd_membres[index_membres][index];
                index_buf++;
            }
            index++;
        }
        if(c->type_redirection[index_membres][0]!=-1 && last_redirection==0 && c->redirection[index_membres][0]==NULL) {
            save_redirection(c, index_buf, buffer_fichier, index_membres, 0);
            memset(buffer_fichier, '\0',256);
            index_buf=0;
        }
        else if(c->type_redirection[index_membres][1]!=-1 && last_redirection==1 && c->redirection[index_membres][1]==NULL) {
            save_redirection(c, index_buf, buffer_fichier, index_membres, 1);
            memset(buffer_fichier, '\0',256);
            index_buf=0;
        }
        else if(c->type_redirection[index_membres][2]!=-1 && last_redirection==2 && c->redirection[index_membres][2]==NULL) {
            save_redirection(c, index_buf, buffer_fichier, index_membres, 2);
            memset(buffer_fichier, '\0',256);
            index_buf=0;
        }
        //reinitialisation for the next member
        last_redirection=-1;
        index_membres++;
    }
}

void free_redirection(cmd *c) {

    int index_membres = 0;
    int index_redir = 0;
    /*parcours tout le tableaux des arguments et libère toute la mémoire allouée*/
    while(index_membres<c->nb_cmd_membres) {
        index_redir = 0;
        while(index_redir<=2) {
            if(c->redirection[index_membres][index_redir]!=NULL) {
                free(c->redirection[index_membres][index_redir]);
            }
            index_redir++;
        }
        index_membres++;
    }

}
void aff_redirection(cmd *c) {

    int index_membres = 0;
    int index_redir = 0;
    /*parcours tout le tableaux des arguments et libère toute la mémoire allouée*/
    while(index_membres<c->nb_cmd_membres) {
        index_redir = 0;
        while(index_redir<=2) {
            if(c->redirection[index_membres][index_redir]!=NULL) {
                printf("\n%s\n index %d\n membres %d \n",c->redirection[index_membres][index_redir],index_redir,index_membres);
                printf("\n type redir : %d \n",c->type_redirection[index_membres][index_redir]);
            }
            index_redir++;
        }
        index_membres++;
    }


}
