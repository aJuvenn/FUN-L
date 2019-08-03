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

			FLTerm * evaluatedTerm = flTermEvaluationFromParseTree(tree->data.let.affect, env);
			flParseTreeFree(tree);

			if (evaluatedTerm == NULL){
				fprintf(stderr, "ERROR\n");
				return;
			}

			printf ("Global var %s : ", tree->data.let.variable);
			flTermPrint(evaluatedTerm);
			printf ("\n\n");

			env->globalTerms[env->nbGlobalVar] = evaluatedTerm;
			env->globalVarNames[env->nbGlobalVar] = strdup(tree->data.let.variable);
			env->nbGlobalVar++;


		} else {

			FLTerm * evaluatedTerm = flTermEvaluationFromParseTree(tree, env);
			flParseTreeFree(tree);

			if (evaluatedTerm == NULL){
				fprintf(stderr, "ERROR\n");
				return;
			}

			printf("Evaluated term : ");
			flTermPrint(evaluatedTerm);
			printf("\n");
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

	const char * exprToEvaluate = "let true fun (x y) x in (true true true)";

	FLStreamCursor cursor = exprToEvaluate;

	printf("Expr : %s\n", exprToEvaluate);

	printf("Parsing... \n");
	FLParseTree * tree = flParseTree(cursor);
	printf("Done, parse tree:\n");
	flParseTreePrint(tree);

	printf("\nCreating term... \n");
	FLEnvironment * env = flEnvironmentNew(1000);
	FLTerm * term = flTermFromParseTree(tree, env);
	printf("Done, term:\n");
	flTermPrint(term);

	printf("\nEvaluation...\n");
	FLTerm * evaluatedTerm = flTermEvaluation(term, env);
	printf("Done, evaluated term:\n");
	flTermPrint(evaluatedTerm);
	printf("\n");




	int fd = open("code/simple.funl", O_RDONLY);
	int len = lseek(fd, 0, SEEK_END);
	char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);


	printf("%s", data);

	FLEnvironment * env2 = flEnvironmentNew(1000);
	eval(data, env2);



	return 0;

}

