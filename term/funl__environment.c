/*
 * funl__parsing_environment.c
 *
 *  Created on: 1 août 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLEnvironment * flEnvironmentNew(const size_t maximumNbGlobalVar,
								 const size_t maximumVarNameStackSize,
								 const size_t allocatedTermPoolSize)
{
	FLEnvironment * output;

	FL_SIMPLE_ALLOC(output, 1);
	FL_SIMPLE_ALLOC(output->varNameStack, maximumVarNameStackSize);
	FL_SIMPLE_ALLOC(output->globalTerms, maximumNbGlobalVar);
	FL_SIMPLE_ALLOC(output->globalVarNames, maximumNbGlobalVar);
	FL_SIMPLE_ALLOC(output->allocatedTermPool, allocatedTermPoolSize);
	FL_SIMPLE_ALLOC(output->availableTerms, allocatedTermPoolSize);

	output->varNameStackSize = 0;
	output->nbGlobalVar = 0;

	output->nbAvailableTerms = allocatedTermPoolSize;

	for (size_t i = 0 ; i < allocatedTermPoolSize ; i++){
		output->availableTerms[i] = output->allocatedTermPool + i;
	}

	return output;
}



void flEnvironmentFree(FLEnvironment * env)
{
	for (size_t i = 0 ; i < env->nbGlobalVar ; i++){
		free(env->globalVarNames[i]);
	}

	free(env->varNameStack);
	free(env->globalTerms);
	free(env->globalVarNames);

	free(env->allocatedTermPool);
	free(env->availableTerms);

	free(env);
}



FLTerm * flGetTermFromGlobalId(const FLTermId id, FLEnvironment * const env)
{
	if (id >= env->nbGlobalVar){
		return NULL;
	}

	return flTermCopy(env->globalTerms[id], env);
}



FLTermId flTermIdFromVarName(const char * const varName, const FLEnvironment * const env)
{
	for (size_t i = 0 ; i < env->varNameStackSize ; i++){
		if (strcmp(varName, env->varNameStack[env->varNameStackSize - i-1]) == 0){
			return (FLTermId) i;
		}
	}

	return (FLTermId) -1;
}


FLTermId flGlobalIdFromName(const char * const globalVarName, const FLEnvironment * const env)
{
	for (size_t i = 0 ; i < env->nbGlobalVar ; i++){
		if (strcmp(globalVarName, env->globalVarNames[i]) == 0){
			return (FLTermId) i;
		}
	}

	return (FLTermId) -1;
}




void flTermPrettyPrint(const FLTerm * const term, const FLEnvironment * const env)
{
	if (term == NULL){
		printf("(NULL)");
		return;
	}

	switch (term->type){

	case FL_TERM_VAR_ID:
		printf("%u", term->data.varId);
		return;

	case FL_TERM_GLOBAL_VAR_ID:

		if (term->data.varId >= env->nbGlobalVar){
			printf("<INVALID GLOBAL VARIABLE>");
			return;
		}

		printf("'%s'", env->globalVarNames[term->data.varId]);
		return;

	case FL_TERM_FUN:
		printf("L.");
		flTermPrettyPrint(term->data.funBody, env);
		return;

	case FL_TERM_CALL:
		printf(term->data.call.isACallByName ? "[" : "(");
		flTermPrettyPrint(term->data.call.func, env);
		printf(" ");
		flTermPrettyPrint(term->data.call.arg, env);
		printf(term->data.call.isACallByName ? "]" : ")");
		return;


	case FL_TERM_LET:
		printf("let ");
		flTermPrettyPrint(term->data.let.affect, env);
		printf(" in ");
		flTermPrettyPrint(term->data.let.following, env);
		return;

	default:
		printf("<INVALID TERM>");
		return;
	}
}


