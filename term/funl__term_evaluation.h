/*
 * funl__term_evaluation.h
 *
 *  Created on: 1 août 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"




struct FLTerm;
typedef struct FLTerm FLTerm;


typedef enum {

	FL_TERM_CALL,
	FL_TERM_FUN,
	FL_TERM_VAR_ID,
	FL_TERM_GLOBAL_VAR_ID,
	FL_TERM_LET,
	FL_TERM_INTEGER,
	FL_TERM_OPCALL,
	FL_TERM_IF_ELSE

} FLTermType;


typedef struct {

	FLTerm * func;
	FLTerm * arg;
	int isACallByName;

} FLTermCallData;



typedef struct {

	FlTokenOperatorData op;
	size_t nbArguments;
	FLTerm ** arguments;

} FLTermOpCallData;


typedef struct {

	FLTerm * affect;
	FLTerm * following;

} FLTermLetData;


typedef struct {

	FLTerm * condition;
	FLTerm * thenValue;
	FLTerm * elseValue;

} FLTermIfElseData;


typedef uint32_t FLTermId;


struct FLTerm {

	FLTermType type;

	union {

		FLTermCallData call;
		FLTermLetData let;
		FLTerm * funBody;
		FLTermId varId;
		long long int integer;
		FLTermOpCallData opCall;
		FLTermIfElseData ifElse;

	} data;
};




typedef struct {

	const char ** varNameStack;
	size_t varNameStackSize;


	char ** globalVarNames;
	FLTerm ** globalTerms;
	size_t nbGlobalVar;


	FLTerm * allocatedTermPool;
	FLTerm ** availableTerms;
	size_t nbAvailableTerms;
	size_t maxNbAvailableTerms;

} FLEnvironment;




FLTerm * flTermNewVarId(FLTermId id, FLEnvironment * const env);
FLTerm * flTermNewInteger(long long int integer, FLEnvironment * const env);
FLTerm * flTermNewGlobalVarId(FLTermId id, FLEnvironment * const env);
FLTerm * flTermNewFun(FLTerm * body, FLEnvironment * const env);
FLTerm * flTermNewCall(FLTerm * fun, FLTerm * arg, int isACallByName, FLEnvironment * const env);
FLTerm * flTermNewLet(FLTerm * affect, FLTerm * following, FLEnvironment * const env);
FLTerm * flTermNewOpCall(FlTokenOperatorData op, size_t nbArguments, FLTerm ** arguments, FLEnvironment * const env);
FLTerm * flTermNewIfElse(FLTerm * condition, FLTerm * thenValue, FLTerm * elseValue, FLEnvironment * const env);


FLTerm * flTermCopy(const FLTerm * const term, FLEnvironment * const env);



void flTermFree(FLTerm * term, FLEnvironment * const env);


#define FL_TERM_DEBUG_PRINT(x)\
	do {\
		printf("%s %s : ", __FUNCTION__, #x);\
		flTermPrint(x);\
		printf("\n");\
	} while (0)




FLEnvironment * flEnvironmentNew(const size_t maximumNbGlobalVar,
								 const size_t maximumVarNameStackSize,
								 const size_t allocatedTermPoolSize);


void flEnvironmentFree(FLEnvironment * env);


FLTerm * flGetTermFromGlobalId(const FLTermId id, FLEnvironment * const env);


FLTermId flTermIdFromVarName(const char * const varName, const FLEnvironment * const env);
FLTermId flGlobalIdFromName(const char * const globalVarName, const FLEnvironment * const env);



void flTermPrint(const FLTerm * const term, const FLEnvironment * const env);



FLTerm * flTermFromParseTree(const FLParseTree * const tree, FLEnvironment * const env);


FLTerm * flTermEvaluation(const FLTerm * const term, FLEnvironment * const env);


FLTerm * flTermEvaluationFromParseTree(const FLParseTree * const tree, FLEnvironment * const env);

