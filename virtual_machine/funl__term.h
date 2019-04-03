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
	FL_TERM_VAR_ID

} FLTermType;


typedef struct {

	FLTerm * func;
	FLTerm * arg;

} FLTermCallData;


typedef uint32_t FLTermId;


struct FLTerm {

	FLTermType type;

	union {

		FLTermCallData call;
		FLTerm * funBody;
		FLTermId varId;

	} data;
};



FLTerm * flTermNewVarId(FLTermId id);
FLTerm * flTermNewFun(FLTerm * body);
FLTerm * flTermNewCall(FLTerm * fun, FLTerm * arg);
void flTermPrint(const FLTerm * const term);
FLTerm * flTermParse(const char * const str);


#endif /* VIRTUAL_MACHINE_FUNL__TERM_H_ */
