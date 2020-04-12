/*
 * funl__environment.h
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#include "../funl__include.h"


typedef struct {

	char ** varNameStack;
	FLSharedTerm ** varTermStack;
	size_t varStackSize;
	size_t maxVarStackSize;

	FLSharedTerm * allocatedTermPool;
	FLSharedTerm ** availableTerms;
	size_t nbAvailableTerms;
	size_t maxNbAvailableTerms;

	FLSharedTerm ** executionStack;
	size_t executionStackSize;
	size_t maxExecutionStackSize;

} FLEnvironment;



FLEnvironment * flEnvironmentNew(const size_t maximumNbVar,
								 const size_t allocatedTermPoolSize,
								 const size_t maxExecutionStackSize);

void flEnvironmentFree(FLEnvironment * env);

size_t flEnvironmentVarId(const FLEnvironment * const env, const char * const varName);

int flEnvironmentPushVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term);
void flEnvironmentPopVar(FLEnvironment * const env, const size_t nbVariables);


int flEnvironmentPushExecutionTerm(FLEnvironment * const env, FLSharedTerm * const term);
void flEnvironmentPopExecutionTerm(FLEnvironment * const env, const size_t nbTerms);

FLSharedTerm * flEnvironmentExecutionTermFromVarId(const FLEnvironment * const env, const size_t varId);




