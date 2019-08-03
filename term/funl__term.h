/*
 * funl__term.h
 *
 *  Created on: 3 avr. 2019
 *      Author: ajuvenn
 */

#ifndef VIRTUAL_MACHINE_FUNL__TERM_H_
#define VIRTUAL_MACHINE_FUNL__TERM_H_

#include "../funl__include.h"



struct FLTerm;
typedef struct FLTerm FLTerm;


typedef enum {

	FL_TERM_CALL,
	FL_TERM_FUN,
	FL_TERM_VAR_ID,
	FL_TERM_GLOBAL_VAR_ID,
	FL_TERM_LET

} FLTermType;


typedef struct {

	FLTerm * func;
	FLTerm * arg;

} FLTermCallData;


typedef struct {

	FLTerm * affect;
	FLTerm * following;

} FLTermLetData;



typedef uint32_t FLTermId;


struct FLTerm {

	FLTermType type;

	union {

		FLTermCallData call;
		FLTermLetData let;
		FLTerm * funBody;
		FLTermId varId;

	} data;
};





typedef FLTerm * (*FLTermManipulator)(const FLTerm * const);

FLTerm * flTermCopy(const FLTerm * const term);
FLTerm * flTermApply(FLTerm * term, FLTermManipulator f);


FLTerm * flTermNewVarId(FLTermId id);
FLTerm * flTermNewGlobalVarId(FLTermId id);
FLTerm * flTermNewFun(FLTerm * body);
FLTerm * flTermNewCall(FLTerm * fun, FLTerm * arg);
FLTerm * flTermNewLet(FLTerm * affect, FLTerm * following);


FLTerm * flTermParse(const char * const str);


void flTermPrint(const FLTerm * const term);
void flTermFree(FLTerm * term);


#define FL_TERM_DEBUG_PRINT(x)\
	do {\
		printf("%s %s : ", __FUNCTION__, #x);\
		flTermPrint(x);\
		printf("\n");\
	} while (0)

#endif /* VIRTUAL_MACHINE_FUNL__TERM_H_ */
