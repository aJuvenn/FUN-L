/*
 * funl__term_evaluation.h
 *
 *  Created on: 1 août 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



typedef struct {

	const char ** varNameStack;
	size_t varNameStackSize;


	const char ** globalVarNames;
	FLTerm ** globalTerms;
	size_t nbGlobalVar;


} FLEnvironment;


FLEnvironment * flEnvironmentNew(const size_t maximumSize);
void flEnvironmentFree(FLEnvironment * env);


FLTerm * flGetTermFromGlobalId(const FLTermId id, const FLEnvironment * const env);


FLTermId flTermIdFromVarName(const char * const varName, const FLEnvironment * const env);
FLTermId flGlobalIdFromName(const char * const globalVarName, const FLEnvironment * const env);



FLTerm * flTermFromParseTree(const FLParseTree * const tree, FLEnvironment * const env);


FLTerm * flTermEvaluation(const FLTerm * const term, FLEnvironment * const env);


FLTerm * flTermEvaluationFromParseTree(const FLParseTree * const tree, FLEnvironment * const env);

