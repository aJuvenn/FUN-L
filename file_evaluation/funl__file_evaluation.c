/*
 * funl__file_evaluation.c
 *
 *  Created on: 2 sept. 2019
 *      Author: ajuvenn
 */



#include "../funl__include.h"

int flEvaluateGlobalDefinition(const FLParseTree * const tree, FLEnvironment * const env)
{
	FLSharedTerm * globalVarRef;
	int ret;

	if (tree->data.let.recursive){

		globalVarRef = flSharedTermNewRef(NULL, env);

		if (globalVarRef == NULL){
			fprintf(stderr, "ERROR\n");
			return EXIT_FAILURE;
		}

		ret = flEnvironmentAddGlobalVar(env, tree->data.let.variable, globalVarRef);

		if (ret != EXIT_SUCCESS){
			fprintf(stderr, "ERROR\n");
			return ret;
		}
	}

	FLSharedTerm * globalTerm = flSharedTermFromParseTree(tree->data.let.affect, env);

	if (globalTerm == NULL){
		fprintf(stderr, "ERROR\n");
		return EXIT_FAILURE;
	}

	if (tree->data.let.recursive){
		FL_SHARED_TERM_SET_REFERENCE(globalVarRef->ref, globalTerm);
		globalTerm = globalVarRef;
	} else {
		ret = flEnvironmentAddGlobalVar(env, tree->data.let.variable, globalTerm);
		if (ret != EXIT_SUCCESS){
			fprintf(stderr, "ERROR\n");
			return ret;
		}
	}

	ret = flSharedTermEvaluate(globalTerm, env);

	if (ret != EXIT_SUCCESS){
		fprintf(stderr, "ERROR\n");
		return ret;
	}

	return EXIT_SUCCESS;
}


int flEvaluateAndPrintExpression(const FLParseTree * const tree, FLEnvironment * const env)
{
	FLSharedTerm * term = flSharedTermFromParseTree(tree, env);

	if (term == NULL){
		fprintf(stderr, "ERROR\n");
		return EXIT_FAILURE;
	}

	int ret = flSharedTermEvaluate(term, env);

	if (ret != EXIT_SUCCESS){
		fprintf(stderr, "ERROR\n");
		return EXIT_FAILURE;
	}

	printf("Input  expr:\t");
	flParseTreePrint(tree);
	flSharedTermSaveToPDF(term, "evaluated_term.pdf");
	flSharedTermFree(term, env);

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
