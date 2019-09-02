/*
 * funl__file_evaluation.c
 *
 *  Created on: 2 sept. 2019
 *      Author: ajuvenn
 */



#include "../funl__include.h"

int flEvaluateGlobalDefinition(const FLParseTree * const tree, FLEnvironment * const env)
{
	if (tree->data.let.recursive){
		env->globalTerms[env->nbGlobalVar] = NULL;
		env->globalVarNames[env->nbGlobalVar] = strdup(tree->data.let.variable);
		env->nbGlobalVar++;
	}

	FLTerm * evaluatedTerm = flTermEvaluationFromParseTree(tree->data.let.affect, env);

	if (evaluatedTerm == NULL){
		fprintf(stderr, "ERROR\n");
		return EXIT_FAILURE;
	}

#ifdef FL_PRINT_GLOBAL_DEFINITIONS
	printf ("Global var %s : ", tree->data.let.variable);
	flTermPrettyPrint(evaluatedTerm, env);
	printf ("\n\n");
#endif

	if (tree->data.let.recursive){
		env->globalTerms[env->nbGlobalVar - 1] = evaluatedTerm;
	} else {
		env->globalTerms[env->nbGlobalVar] = evaluatedTerm;
		env->globalVarNames[env->nbGlobalVar] = strdup(tree->data.let.variable);
		env->nbGlobalVar++;
	}

	return EXIT_SUCCESS;
}


int flEvaluateAndPrintExpression(const FLParseTree * const tree, FLEnvironment * const env)
{
	FLTerm * evaluatedTerm = flTermEvaluationFromParseTree(tree, env);

	if (evaluatedTerm == NULL){
		fprintf(stderr, "ERROR\n");
		return EXIT_FAILURE;
	}

	printf("Input  expr:\t");
	flParseTreePrint(tree);
	printf("\nOutput term:\t");
	flTermPrettyPrint(evaluatedTerm, env);
	printf("\n\n");

	flTermFree(evaluatedTerm, env);

	return EXIT_SUCCESS;
}


int flEvaluateString(const char * const str, FLEnvironment * const env)
{
	FLStreamCursor cursor = str;

	while (1){

		FLParseTree * tree = flParseTreeRecursive(&cursor);

		if (tree == NULL){
			fprintf(stderr, "ERROR : Invalid syntax\n");
			return EXIT_FAILURE;
		}

		if (tree->type == FL_PARSE_TREE_END_OF_FILE){
			flParseTreeFree(tree);
			return EXIT_SUCCESS;
		}

		int ret;

		if (flParseTreeIsAGlobalDefinition(tree)){
			ret = flEvaluateGlobalDefinition(tree, env);
		} else {
			ret = flEvaluateAndPrintExpression(tree, env);
		}

		flParseTreeFree(tree);

		if (ret != EXIT_SUCCESS){
			return EXIT_FAILURE;
		}
	}
}



int flEvaluateFile(const char * const path, FLEnvironment * const env)
{
	int fileDescriptor = open(path, O_RDONLY);

	if (fileDescriptor == -1){
		fprintf(stderr, "ERROR : Couldn't read file %s\n", path);
		return EXIT_FAILURE;
	}

	off_t fileLength = lseek(fileDescriptor, 0, SEEK_END);
	char * fileDataString = mmap(0, fileLength, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);

	int ret = flEvaluateString(fileDataString, env);

	munmap(fileDataString, fileLength);

	return ret;
}
