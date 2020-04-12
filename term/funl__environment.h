/*
 * funl__environment.h
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#include "../funl__include.h"


typedef struct {

	char ** localVarNameStack;
	FLSharedTerm ** localVarTermStack;
	size_t localVarStackSize;
	size_t maxLocalVarStackSize;

	char ** globalVarNameStack;
	FLSharedTerm ** globalVarTermStack;
	size_t globalVarStackSize;
	size_t maxGlobalVarStackSize;

	FLSharedTerm * allocatedTermPool;
	FLSharedTerm ** availableTerms;
	size_t nbAvailableTerms;
	size_t maxNbAvailableTerms;

	FLSharedTerm ** executionStack;
	size_t executionStackSize;
	size_t maxExecutionStackSize;

} FLEnvironment;



FLEnvironment * flEnvironmentNew(
		const size_t maximumNbLocalVar,
		const size_t maximumNbGlobalVar,
		const size_t allocatedTermPoolSize,
		const size_t maxExecutionStackSize);

void flEnvironmentFree(FLEnvironment * env);


void flEnvironmentVarId(const FLEnvironment * const env, const char * const varName, size_t * out_id, int * out_isGlobal);

int flEnvironmentPushVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term);
void flEnvironmentPopVar(FLEnvironment * const env, const size_t nbVariables);
int flEnvironmentAddGlobalVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term);




int flEnvironmentPushExecutionTerm(FLEnvironment * const env, FLSharedTerm * const term);
void flEnvironmentPopExecutionTerm(FLEnvironment * const env, const size_t nbTerms);

FLSharedTerm * flEnvironmentExecutionTermFromVarId(const FLEnvironment * const env, const size_t varId, const int isAGlobalVar);




