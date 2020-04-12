/*
 * funl__parsing_environment.c
 *
 *  Created on: 1 août 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLEnvironment * flEnvironmentNew(
		const size_t maximumNbLocalVar,
		const size_t maximumNbGlobalVar,
		const size_t allocatedTermPoolSize,
		const size_t maxExecutionStackSize)
{
	FLEnvironment * output;
	FL_SIMPLE_ALLOC(output, 1);

	FL_SIMPLE_ALLOC(output->localVarNameStack, maximumNbLocalVar);
	FL_SIMPLE_ALLOC(output->localVarTermStack, maximumNbLocalVar);

	FL_SIMPLE_ALLOC(output->globalVarNameStack, maximumNbGlobalVar);
	FL_SIMPLE_ALLOC(output->globalVarTermStack, maximumNbGlobalVar);

	FL_SIMPLE_ALLOC(output->allocatedTermPool, allocatedTermPoolSize);
	FL_SIMPLE_ALLOC(output->availableTerms, allocatedTermPoolSize);

	FL_SIMPLE_ALLOC(output->executionStack, maxExecutionStackSize);

	output->localVarStackSize = 0;
	output->maxLocalVarStackSize = maximumNbLocalVar;

	output->globalVarStackSize = 0;
	output->maxGlobalVarStackSize = maximumNbGlobalVar;

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
	flEnvironmentPopVar(env, env->localVarStackSize);

	free(env->localVarNameStack);
	free(env->localVarTermStack);


	/* TODO : free global vars */
	free(env->globalVarNameStack);
	free(env->globalVarTermStack);


	free(env->allocatedTermPool);
	free(env->availableTerms);

	free(env);
}


void flEnvironmentVarId(const FLEnvironment * const env, const char * const varName, size_t * out_id, int * out_isGlobal)
{
	/* Seeking for a local variable */
	for (size_t i = 0 ; i < env->localVarStackSize ; i++){
		if (strcmp(varName, env->localVarNameStack[env->localVarStackSize-i-1]) == 0){
			*out_id = i;
			*out_isGlobal = 0;
			return;
		}
	}

	/* Seeking for a global variable */
	for (size_t i = env->globalVarStackSize ; i > 0 ; i--){
		if (strcmp(varName, env->globalVarNameStack[i-1]) == 0){
			*out_id = i-1;
			*out_isGlobal = 1;
			return;
		}
	}


	*out_id = (size_t) -1;
}


int flEnvironmentPushVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term)
{
	if (env->localVarStackSize == env->maxLocalVarStackSize){
		return EXIT_FAILURE;
	}

	env->localVarNameStack[env->localVarStackSize] = strdup(varName);
	FL_SHARED_TERM_SET_REFERENCE(env->localVarTermStack[env->localVarStackSize], term);
	env->localVarStackSize++;

	return EXIT_SUCCESS;
}


void flEnvironmentPopVar(FLEnvironment * const env, const size_t nbVariables)
{
	for (size_t i = 0 ; i < nbVariables ; i++){
		/* TODO */
		//size_t j = env->localVarStackSize - 1 - i;
		//FL_SHARED_TERM_REMOVE_REFERENCE(env->localVarTermStack[j], env);
		//free(env->localVarNameStack[j]);
	}

	env->localVarStackSize -= nbVariables;
}


int flEnvironmentAddGlobalVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term)
{
	if (env->globalVarStackSize == env->maxGlobalVarStackSize){
		return EXIT_FAILURE;
	}

	env->globalVarNameStack[env->globalVarStackSize] = strdup(varName);
	FL_SHARED_TERM_SET_REFERENCE(env->globalVarTermStack[env->globalVarStackSize], term);
	env->globalVarStackSize++;

	return EXIT_SUCCESS;
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


FLSharedTerm * flEnvironmentExecutionTermFromVarId(const FLEnvironment * const env, const size_t varId, const int isAGlobalVar)
{
	FL_DEBUG_PRINT("Seeking %s variable of id %zu", isAGlobalVar ? "global" : "local", varId);

	if (isAGlobalVar){
		if (varId >= env->globalVarStackSize){
			FL_DEBUG_PRINT("varId (%zu) >= env->globalVarStackSize (%zu)", varId, env->globalVarStackSize);
			return NULL;
		}
		FL_DEBUG_PRINT("Returning %p", env->globalVarTermStack[varId]);
		return env->globalVarTermStack[varId];
	} else {
		if (varId >= env->executionStackSize){
			FL_DEBUG_PRINT("varId (%zu) >= env->executionStackSize (%zu)", varId, env->executionStackSize);
			return NULL;
		}
		FL_DEBUG_PRINT("Returning %p", env->executionStack[env->executionStackSize-1 - varId]);
		return env->executionStack[env->executionStackSize-1 - varId];
	}
}


