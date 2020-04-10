/*
 * funl__shared_term.h
 *
 *  Created on: 9 avr. 2020
 *      Author: ajuvenn
 */

#ifndef TERM_FUNL__SHARED_TERM_H_
#define TERM_FUNL__SHARED_TERM_H_

#include "../funl__include.h"


struct FLSharedTerm;
typedef struct FLSharedTerm FLSharedTerm;


typedef enum {

	FL_SHARED_TERM_CALL,
	FL_SHARED_TERM_FUN,
	FL_SHARED_TERM_REF,
	FL_SHARED_TERM_ARGREF,
	FL_SHARED_TERM_INTEGER,
	FL_SHARED_TERM_IF_ELSE

} FLSharedTermType;



typedef enum {

	FL_SHARED_TERM_STATUS_NONE = 0,
	FL_SHARED_TERM_STATUS_EVALUATED = (1 << 0),
	FL_SHARED_TERM_STATUS_BEING_EVALUATED = (1 << 1),
	FL_SHARED_TERM_STATUS_BEING_DESTROYED = (1 << 2),
	FL_SHARED_TERM_STATUS_BEING_COPIED = (1 << 3)


} FLSharedTermStatus;


struct FLSharedTerm {

	FLSharedTermType type;
	size_t nbReferences;
	FLSharedTermStatus status;

	FLSharedTerm * copy;

	union {

		struct {
			FLSharedTerm * func;
			FLSharedTerm * arg;
			int isACallByName;
		} call;

		struct {
			FLSharedTerm * body;
		} fun;

		FLSharedTerm * ref;
		long long int integer;

		struct {
			FLSharedTerm * condition;
			FLSharedTerm * thenValue;
			FLSharedTerm * elseValue;
		} ifElse;
	};
};


FLSharedTerm * flSharedTermNewFun(FLSharedTerm * funBody, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewRef(FLSharedTerm * pointedTerm, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewArgRef(FLSharedTerm * pointedFun, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewInteger(long long int integer, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewCall(FLSharedTerm * fun, FLSharedTerm * arg, int isACallByName, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewIfElse(FLSharedTerm * condition, FLSharedTerm * thenValue, FLSharedTerm * elseValue, FLEnvironment * const env);

int flSharedTermSaveToPDF(const FLSharedTerm * term, const char * const outputPath);

int flSharedTermEvaluateOneStep(FLSharedTerm * term, FLEnvironment * env);

void flSharedTermDestroy(FLSharedTerm * term, FLEnvironment * const env);

#define FL_SHARED_TERM_SET_REFERENCE(place, pointedTerm)\
		do {\
			FLSharedTerm * _FL_TMP_POINTED_TERM = (pointedTerm);\
			if ((_FL_TMP_POINTED_TERM) == NULL){\
				break;\
			}\
			(place) = _FL_TMP_POINTED_TERM;\
			_FL_TMP_POINTED_TERM->nbReferences++;\
		} while(0)


#define FL_SHARED_TERM_REMOVE_REFERENCE(place, env)\
		do {\
			FLSharedTerm * const _FL_TMP_REF_TO_REMOVE = (place);\
			(place) = NULL;\
			if ((_FL_TMP_REF_TO_REMOVE->nbReferences--) == 1){\
				flSharedTermDestroy(_FL_TMP_REF_TO_REMOVE, (env));\
			}\
		} while(0)


#define FL_SHARED_TERM_MOVE_REFERENCE(place, newPointedTerm, env)\
		do {\
			FLSharedTerm * _FL_TMP = (place);\
			(place) = (newPointedTerm);\
			(place)->nbReferences++;\
			if ((_FL_TMP->nbReferences--) == 1){\
				flSharedTermDestroy(_FL_TMP, (env));\
			}\
		} while (0)


FLSharedTerm * flSharedTermReferedBy(FLSharedTerm * refTerm);

#define FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(place, env)\
		do {\
			FLSharedTerm * const _FL_TMP_ORIGINAL_REF = (place);\
			FLSharedTerm * const _FL_TMP_POINTED_TERM = flSharedTermReferedBy(_FL_TMP_ORIGINAL_REF);\
			(place) = _FL_TMP_POINTED_TERM;\
			_FL_TMP_POINTED_TERM->nbReferences++;\
			if ((_FL_TMP_ORIGINAL_REF->nbReferences--) == 1){\
				flSharedTermDestroy(_FL_TMP_ORIGINAL_REF, (env));\
			}\
		} while (0)


int flSharedTermEvaluate(FLSharedTerm * term, FLEnvironment * env);

#endif /* TERM_FUNL__SHARED_TERM_H_ */
