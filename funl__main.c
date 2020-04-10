/*
 * funl__main.c
 *
 *  Created on: 9 janv. 2019
 *      Author: ajuvenn
 */
#include "funl__include.h"


#define FL_MAXIMUM_NB_GLOBAL_VAR 4096
#define FL_MAXIMUM_VARIABLE_NAME_STACK_SIZE 1024
#define FL_MAXIMUM_NB_OF_ALLOCATED_TERMS 1e6


#define TEST_SHARED_TERMS

int main(int argc, char * argv[])
{
#ifndef TEST_SHARED_TERMS

	static const char * const filePath = "code/test_graphviz.funl";

	FLEnvironment * env = flEnvironmentNew(FL_MAXIMUM_NB_GLOBAL_VAR,
			FL_MAXIMUM_VARIABLE_NAME_STACK_SIZE,
			FL_MAXIMUM_NB_OF_ALLOCATED_TERMS);

	int ret = flEvaluateFile(filePath, env);

	if (ret != EXIT_SUCCESS){
		fprintf(stderr, "ERROR : file evaluation failed.\n");
	}

	flEnvironmentFree(env);

#else

	FLEnvironment * env = NULL;


	FLSharedTerm * termToEvaluateArgRef = flSharedTermNewArgRef(NULL, env);
	FLSharedTerm * termToEvaluateRef = flSharedTermNewRef(flSharedTermNewInteger(0, env), env);
	FLSharedTerm * termToEvaluate = flSharedTermNewFun(flSharedTermNewRef(flSharedTermNewIfElse(
			flSharedTermNewRef(flSharedTermNewRef(termToEvaluateArgRef, env), env),
			flSharedTermNewRef(flSharedTermNewIfElse(
					flSharedTermNewRef(flSharedTermNewRef(flSharedTermNewInteger(0, env), env), env),
					flSharedTermNewRef(flSharedTermNewRef(termToEvaluateRef, env), env),
					flSharedTermNewRef(flSharedTermNewRef(flSharedTermNewInteger(3, env), env), env),
					env
			), env),
			flSharedTermNewRef(flSharedTermNewRef(flSharedTermNewInteger(0, env), env), env),
			env
	), env), env);

	FL_SHARED_TERM_MOVE_REFERENCE(termToEvaluateRef->ref, termToEvaluate, env);
	termToEvaluateArgRef->ref = termToEvaluate;

	FLSharedTerm * termToReallyEvaluate = flSharedTermNewCall(termToEvaluate, flSharedTermNewInteger(1, env), 0, env);
	termToReallyEvaluate->nbReferences++;

	flSharedTermSaveToPDF(termToReallyEvaluate, "before.pdf");
	flSharedTermEvaluate(termToReallyEvaluate, env);
	flSharedTermSaveToPDF(termToReallyEvaluate, "after.pdf");

	FL_SHARED_TERM_REMOVE_REFERENCE(termToReallyEvaluate, env);


#endif

	return EXIT_SUCCESS;
}
