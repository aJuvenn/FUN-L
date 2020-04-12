/*
 * funl__parsing_environment.c
 *
 *  Created on: 1 août 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"




FLEnvironment * flEnvironmentNew(const size_t maximumNbVariables,
								 const size_t allocatedTermPoolSize,
								 const size_t maxExecutionStackSize)
{
	FLEnvironment * output;
	FL_SIMPLE_ALLOC(output, 1);

	FL_SIMPLE_ALLOC(output->varNameStack, maximumNbVariables);
	FL_SIMPLE_ALLOC(output->varTermStack, maximumNbVariables);

	FL_SIMPLE_ALLOC(output->allocatedTermPool, allocatedTermPoolSize);
	FL_SIMPLE_ALLOC(output->availableTerms, allocatedTermPoolSize);

	FL_SIMPLE_ALLOC(output->executionStack, maxExecutionStackSize);

	output->varStackSize = 0;
	output->maxVarStackSize = maximumNbVariables;

	output->nbAvailableTerms = allocatedTermPoolSize;

	for (size_t i = 0 ; i < allocatedTermPoolSize ; i++){
		output->availableTerms[i] = output->allocatedTermPool + i;
	}

	output->maxNbAvailableTerms = allocatedTermPoolSize;

	output->executionStackSize = 0;
	output->maxExecutionStackSize = maxExecutionStackSize;

	return output;
}



void flEnvironmentFree(FLEnvironment * env)
{
	flEnvironmentPopVar(env, env->varStackSize);

	free(env->varNameStack);
	free(env->varTermStack);

	free(env->allocatedTermPool);
	free(env->availableTerms);

	free(env);
}


size_t flEnvironmentVarId(const FLEnvironment * const env, const char * const varName)
{
	/* Seeking for a local variable */
	for (size_t i = 0 ; i < env->varStackSize ; i++){
		if (strcmp(varName, env->varNameStack[env->varStackSize-i-1]) == 0){
			return i;
		}
	}

	return (size_t) -1;
}


int flEnvironmentPushVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term)
{
	if (env->varStackSize == env->maxVarStackSize){
		return EXIT_FAILURE;
	}

	env->varNameStack[env->varStackSize] = strdup(varName);
	FL_SHARED_TERM_SET_REFERENCE(env->varTermStack[env->varStackSize], term);
	env->varStackSize++;

	return EXIT_SUCCESS;
}


void flEnvironmentPopVar(FLEnvironment * const env, const size_t nbVariables)
{
	for (size_t i = 0 ; i < nbVariables ; i++){
		size_t j = env->varStackSize - 1 - i;
		FL_SHARED_TERM_REMOVE_REFERENCE(env->varTermStack[j], env);
		free(env->varNameStack[j]);
	}

	env->varStackSize -= nbVariables;
}


int flEnvironmentPushExecutionTerm(FLEnvironment * const env, FLSharedTerm * const term)
{
	if (env->executionStackSize ==  env->maxExecutionStackSize){
		return EXIT_FAILURE;
	}

	/* TODO : add reference ? */
	env->executionStack[env->executionStackSize++] = term;

	return EXIT_SUCCESS;
}


void flEnvironmentPopExecutionTerm(FLEnvironment * const env, const size_t nbTerms)
{
	/* TODO : remove references ? */
	env->executionStackSize -= nbTerms;
}


FLSharedTerm * flEnvironmentExecutionTermFromVarId(const FLEnvironment * const env, const size_t varId)
{
	if (varId >= env->varStackSize){
		return NULL;
	}

	return env->executionStack[env->varStackSize-1 - varId];
}

