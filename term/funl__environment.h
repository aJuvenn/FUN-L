/*
 * funl__environment.h
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#include "../funl__include.h"


typedef struct {

	char ** localVarNameStack;
	FLSharedTerm ** localTermStack;
	size_t localVarStackSize;
	size_t maxLocalVarStackSize;

	char ** globalVarNames;
	FLSharedTerm ** globalTerms;
	size_t nbGlobalVar;
	size_t maxNbGlobalVar;

	FLSharedTerm * allocatedTermPool;
	FLSharedTerm ** availableTerms;
	size_t nbAvailableTerms;
	size_t maxNbAvailableTerms;

} FLEnvironment;



FLEnvironment * flEnvironmentNew(const size_t maximumNbGlobalVar,
								 const size_t maximumNbLocalVar,
								 const size_t allocatedTermPoolSize);

void flEnvironmentFree(FLEnvironment * env);

FLSharedTerm * flEnvironmentSharedTermFromVarName(const FLEnvironment * const env, const char * const varName);


int flEnvironmentPushLocalVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term);
void flEnvironmentPopLocalVar(FLEnvironment * const env, const size_t nbVariables);
int flEnvironmentAddGlobalVar(FLEnvironment * const env, const char * const varName, FLSharedTerm * const term);


