/*
 * funl__main.c
 *
 *  Created on: 9 janv. 2019
 *      Author: ajuvenn
 */
#include "funl__include.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


void eval(const char * const str, FLEnvironment * const env)
{
	FLStreamCursor cursor = str;

	while (1){

		FLParseTree * tree = flParseTreeRecursive(&cursor);


		if (tree == NULL || tree->type == FL_PARSE_TREE_INVALID){
			break;
		}

		if (tree->type == FL_PARSE_TREE_LET && tree->data.let.following == NULL){

			if (tree->data.let.recursive){
				env->globalTerms[env->nbGlobalVar] = NULL;
				env->globalVarNames[env->nbGlobalVar] = strdup(tree->data.let.variable);
				env->nbGlobalVar++;
			}

			FLTerm * evaluatedTerm = flTermEvaluationFromParseTree(tree->data.let.affect, env);


			if (evaluatedTerm == NULL){
				fprintf(stderr, "ERROR\n");
				return;
			}

			printf ("Global var %s : ", tree->data.let.variable);
			flTermPrettyPrint(evaluatedTerm, env);
			printf ("\n\n");

			if (tree->data.let.recursive){
				env->globalTerms[env->nbGlobalVar - 1] = evaluatedTerm;
			} else {
				env->globalTerms[env->nbGlobalVar] = evaluatedTerm;
				env->globalVarNames[env->nbGlobalVar] = strdup(tree->data.let.variable);
				env->nbGlobalVar++;
			}

			flParseTreeFree(tree);

		} else {

			FLTerm * evaluatedTerm = flTermEvaluationFromParseTree(tree, env);

			if (evaluatedTerm == NULL){
				fprintf(stderr, "ERROR\n");
				return;
			}

			printf("Evaluated term for tree ");
			flParseTreePrint(tree);
			printf(" : ");
			flTermPrettyPrint(evaluatedTerm, env);
			printf("\n\n");

			flParseTreeFree(tree);
			flTermFree(evaluatedTerm);
		}
	}
}


int main(int argc, char * argv[])
{


#ifdef TEST_TOKENIZER
	const char * s = "let rec true fun (x y) let z (+ x y) in z;";

	FLStreamCursor cursor = s;

	FlToken tk;

	do {
		flTokenNext(&cursor, &tk);
		flTokenPrint(&tk);
		printf("\n");

		flTokenFree(&tk);

	} while (tk.type != FL_TOKEN_END_OF_STREAM
			&& tk.type != FL_TOKEN_INVALID);



#endif


	int fd = open("code/simple.funl", O_RDONLY);
	int len = lseek(fd, 0, SEEK_END);
	char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);


	printf("%s", data);

	FLEnvironment * env2 = flEnvironmentNew(100000);
	eval(data, env2);

	flEnvironmentFree(env2);

	munmap(data, len);


	return 0;

}

