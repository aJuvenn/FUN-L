/*
 * funl__environment.c
 *
 *  Created on: 3 avr. 2019
 *      Author: ajuvenn
 */


#include "../funl__include.h"




void flEnvironmentInit(FLEnvironment * const env, const FLStackOffset maxStackSize, const FLEnvironmentId maxEnvSize)
{
	FL_SIMPLE_ALLOC(env->closureLinks, maxEnvSize + 1);
	FL_SIMPLE_ALLOC(env->indexStack, maxEnvSize);
	FL_SIMPLE_ALLOC(env->stack, maxStackSize);

	env->indexStackSize = maxEnvSize;

	for (FLEnvironmentId i = 0 ; i < maxEnvSize ; i++){
		env->indexStack[i] = (maxEnvSize - i);
	}

	env->currentId = 0;
	env->stackSize = 0;
	env->maxStackSize = maxStackSize;
}



void flEnvironmentStackTerm(FLEnvironment * const env, FLTerm * const term)
{
	if (env->stackSize == env->maxStackSize){
		fprintf(stderr, "FUNL-FATAL ERROR: STACK OVERFLOW\n");
		exit(EXIT_FAILURE);
	}

	env->stack[env->stackSize++] = (FLClosure) {term, env->currentId};
}



void flEnvironmentStackClosure(FLEnvironment * const env)
{
	if (env->indexStackSize == 0){
		fprintf(stderr, "FUNL-L FATAL ERROR: ENVIRONMENT IS FULL\n");
		exit(EXIT_FAILURE);
	}

	FLEnvironmentId id = env->indexStack[--(env->indexStackSize)];

	env->closureLinks[id].closure = env->stack[--(env->stackSize)];
	env->closureLinks[id].nextLinkId = env->currentId;
	env->currentId = id;
}



FLTerm * flEnvironmentJump(FLEnvironment * const env, const FLTermId nbJumps)
{
	FLEnvironmentId id = env->currentId;

	if (id == 0){
		fprintf(stderr, "UNKNOWN REFERENCE IN ENVIRONMENT\n");
		exit(EXIT_FAILURE);
	}

	for (FLTermId i = 0 ; i < nbJumps ; i++){

		id = env->closureLinks[id].nextLinkId;

		if (id == 0){
			fprintf(stderr, "UNKNOWN REFERENCE IN ENVIRONMENT\n");
			exit(EXIT_FAILURE);
		}
	}

	env->currentId = env->closureLinks[id].closure.envId;

	return env->closureLinks[id].closure.term;
}



void flEnvironmentPrint(const FLEnvironment * const env)
{
	printf("-----------------------------------\n");
	printf("\tSTACK\n");

	for (FLStackOffset i = 0 ; i  < env->stackSize; i++){
		printf("%u|\t", i);
		flTermPrint(env->stack[i].term);
		printf(" [%u]\n", env->stack[i].envId);
	}

	printf("\n\tENVIRONMENT\n");

	if (env->indexStackSize == 0){
		return;
	}

	for (FLStackOffset i = 1 ; i  < env->indexStack[env->indexStackSize - 1] ; i++){
		printf("%u|\t", i);
		flTermPrint(env->closureLinks[i].closure.term);
		printf(" [%u]\t ---> \t [%u]\n", env->closureLinks[i].closure.envId, env->closureLinks[i].nextLinkId);
	}

	printf("Current environment id : %u\n", env->currentId);
	printf("-----------------------------------\n");
}



