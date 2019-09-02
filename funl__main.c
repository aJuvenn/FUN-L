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


int main(int argc, char * argv[])
{
	static const char * const filePath = "code/lambda_calculus.funl";

	FLEnvironment * env = flEnvironmentNew(FL_MAXIMUM_NB_GLOBAL_VAR,
										   FL_MAXIMUM_VARIABLE_NAME_STACK_SIZE,
										   FL_MAXIMUM_NB_OF_ALLOCATED_TERMS);

	int ret = flEvaluateFile(filePath, env);

	if (ret != EXIT_SUCCESS){
		fprintf(stderr, "ERROR : file evaluation failed.\n");
	}

	flEnvironmentFree(env);

	return ret;
}
