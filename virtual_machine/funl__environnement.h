/*
 * funl__environnement.h
 *
 *  Created on: 3 avr. 2019
 *      Author: ajuvenn
 */

#ifndef VIRTUAL_MACHINE_FUNL__ENVIRONNEMENT_H_
#define VIRTUAL_MACHINE_FUNL__ENVIRONNEMENT_H_

#include "../funl__include.h"


typedef uint32_t FLEnvironmentId;
typedef uint32_t FLStackOffset;



struct FLClosure {

	FLTerm * term;
	FLEnvironmentId envId;

};

typedef struct FLClosure FLClosure;



typedef struct {

	size_t indexStackSize;
	FLEnvironmentId * indexStack;
	FLEnvironmentId currentId;

	struct {

		FLClosure closure;
		FLEnvironmentId nextLinkId;

	} * closureLinks;


	FLStackOffset stackSize;
	FLStackOffset maxStackSize;
	FLClosure * stack;

} FLEnvironment;


void flEnvironmentInit(FLEnvironment * const env,  const FLStackOffset maxStackSize, const FLEnvironmentId envSize);

void flEnvironmentStackTerm(FLEnvironment * const env, FLTerm * const term);

void flEnvironmentStackClosure(FLEnvironment * const env);

FLTerm * flEnvironmentJump(FLEnvironment * const env, const FLTermId nbJumps);


void flEnvironmentPrint(const FLEnvironment * const env);















#endif /* VIRTUAL_MACHINE_FUNL__ENVIRONNEMENT_H_ */
