/*
 * funl__parsing_environment.c
 *
 *  Created on: 1 août 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLEnvironment * flEnvironmentNew(const size_t maximumNbGlobalVar,
								 const size_t maximumNbLocalVar,
								 const size_t allocatedTermPoolSize)
{
	FLEnvironment * output;
	FL_SIMPLE_ALLOC(output, 1);

	FL_SIMPLE_ALLOC(output->localVarNameStack, maximumNbLocalVar);
	FL_SIMPLE_ALLOC(output->localTermStack, maximumNbLocalVar);

	FL_SIMPLE_ALLOC(output->globalVarNames, maximumNbGlobalVar);
	FL_SIMPLE_ALLOC(output->globalTerms, maximumNbGlobalVar);

	FL_SIMPLE_ALLOC(output->allocatedTermPool, allocatedTermPoolSize);
	FL_SIMPLE_ALLOC(output->availableTerms, allocatedTermPoolSize);

	output->localVarStackSize = 0;
	output->maxLocalVarStackSize = maximumNbLocalVar;
	output->nbGlobalVar = 0;
	output->maxNbGlobalVar = maximumNbGlobalVar;

	output->nbAvailableTerms = allocatedTermPoolSize;

	for (size_t i = 0 ; i < allocatedTermPoolSize ; i++){
		output->availableTerms[i] = output->allocatedTermPool + i;
	}

	output->maxNbAvailableTerms = allocatedTermPoolSize;

	return output;
}



void flEnvironmentFree(FLEnvironment * env)
{
	for (size_t i = 0 ; i < env->nbGlobalVar ; i++){
		free(env->globalVarNames[i]);
	}

	free(env->localVarNameStack);
	free(env->localTermStack);

	free(env->globalVarNames);
	free(env->globalTerms);

	free(env->allocatedTermPool);
	free(env->availableTerms);

	free(env);
}


FLSharedTerm * flEnvironmentSharedTermFromVarName(const FLEnvironment * const env, const char * const varName)
{
	/* Seeking for a local variable */
	for (size_t i = env->localVarStackSize ; i > 0  ; i--){
		if (strcmp(varName, env->localVarNameStack[i-1]) == 0){
			return env->localTermStack[i-1];
		}
	}

	/* Seeking for a global variable */
	for (size_t i = env->nbGlobalVar ; i > 0 ; i--){
		if (strcmp(varName, env->globalVarNames[i-1]) == 0){
			return env->globalTerms[i-1];
		}
	}

	return NULL;
}

int flEnvironmentPushLocalVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term)
{
	if (env->localVarStackSize == env->maxLocalVarStackSize){
		return EXIT_FAILURE;
	}

	env->localVarNameStack[env->localVarStackSize] = strdup(varName);
	FL_SHARED_TERM_SET_REFERENCE(env->localTermStack[env->localVarStackSize], term);
	env->localVarStackSize++;

	return EXIT_SUCCESS;
}


void flEnvironmentPopLocalVar(FLEnvironment * const env, const size_t nbVariables)
{
	for (size_t i = env->localVarStackSize ; i > 0 ; i--){
		FL_SHARED_TERM_REMOVE_REFERENCE(env->localTermStack[i-1], env);
		free(env->localVarNameStack[i-1]);
	}

	env->localVarStackSize -= nbVariables;
}


int flEnvironmentAddGlobalVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term)
{
	if (env->nbGlobalVar == env->maxNbGlobalVar){
		return EXIT_FAILURE;
	}

	env->globalVarNames[env->nbGlobalVar] = strdup(varName);
	FL_SHARED_TERM_SET_REFERENCE(env->globalTerms[env->nbGlobalVar], term);
	env->nbGlobalVar++;

	return EXIT_SUCCESS;
}


