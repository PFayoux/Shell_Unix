#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>


typedef struct commande {
	char * cmd_initiale;		    /* La chaine initiale tapee par l'utilisateur */
	char **cmd_membres;				/* a chaque case i chaine du membre i */
	unsigned int nb_cmd_membres;	/* Nombre de membres */
	char ***cmd_membres_args;		/* cmd_membres_args[i][j] contient le jieme mot du ieme membre */
	unsigned int *nb_membres_args;	/* Nombre d'arguments par membres */
	char ***redirection;			/* Pour stocker le chemin vers le fichier de redirection */
	int **type_redirection;			/* Pour stocker le type de redirection */
} cmd;

void aff_membres_args(cmd *c);
void free_membres_args(cmd *c);
int parse_membres_args(cmd *c);
void parse_membres(char *chaine,cmd *ma_cmd);
void aff_membres(cmd *ma_cmd);
void free_membres(cmd *ma_cmd);
void parse_redirection(cmd *c);
void free_redirection(cmd *c);
void aff_redirection(cmd *c);

