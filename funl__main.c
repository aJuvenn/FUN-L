/*
 * funl__main.c
 *
 *  Created on: 9 janv. 2019
 *      Author: ajuvenn
 */
#include "funl__include.h"


#define FL_MAXIMUM_LOCAL_VARIABLE_NAME_STACK_SIZE 1e2
#define FL_MAXIMUM_GLOBAL_VARIABLE_NAME_STACK_SIZE 1e4
#define FL_MAXIMUM_NB_OF_ALLOCATED_TERMS 1e6
#define FL_MAXIMUM_EXECUTION_STACK_SIZE 1e5


int main(int argc, char * argv[])
{
	static const char * const filePath = "code/test_shared.funl";

	FLEnvironment * env = flEnvironmentNew(
			FL_MAXIMUM_LOCAL_VARIABLE_NAME_STACK_SIZE,
			FL_MAXIMUM_GLOBAL_VARIABLE_NAME_STACK_SIZE,
			FL_MAXIMUM_NB_OF_ALLOCATED_TERMS,
			FL_MAXIMUM_EXECUTION_STACK_SIZE);

	int ret = flEvaluateFile(filePath, env);

	if (ret != EXIT_SUCCESS){
		fprintf(stderr, "ERROR : file evaluation failed.\n");
	}

	flEnvironmentFree(env);

	return EXIT_SUCCESS;
}
