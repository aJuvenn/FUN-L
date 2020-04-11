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



#endif /* TERM_FUNL__SHARED_TERM_H_ */
