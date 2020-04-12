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
	FL_SHARED_TERM_VAR,
	FL_SHARED_TERM_REF,
	FL_SHARED_TERM_INTEGER,
	FL_SHARED_TERM_IF_ELSE,
	FL_SHARED_TERM_LET

} FLSharedTermType;



typedef enum {

	FL_SHARED_TERM_STATUS_NONE = 0,
	FL_SHARED_TERM_STATUS_EVALUATED = (1 << 0),
	FL_SHARED_TERM_STATUS_BEING_EVALUATED = (1 << 1),
	FL_SHARED_TERM_STATUS_BEING_DESTROYED = (1 << 2),
	FL_SHARED_TERM_STATUS_BEING_COPIED = (1 << 3),

	FL_SHARED_TERM_STATUS_SEEKING_PATH = (1 << 4),
	FL_SHARED_TERM_STATUS_ADDING_PATH = (1 << 5),
	FL_SHARED_TERM_STATUS_HAS_PATH = (1 << 6)

} FLSharedTermStatus;


struct FLSharedTerm {

	FLSharedTermType type;
	size_t nbReferences;
	FLSharedTermStatus status;

	FLSharedTerm * copy;

	union {

		FLSharedTerm * ref;
		long long int integer;
		size_t varId;

		struct {
			FLSharedTerm * func;
			FLSharedTerm * arg;
			int isACallByName;
		} call;

		struct {
			FLSharedTerm * body;
		} fun;

		struct {
			FLSharedTerm * condition;
			FLSharedTerm * thenValue;
			FLSharedTerm * elseValue;
		} ifElse;

		struct {
			int isRecursive;
			FLSharedTerm * affect;
			FLSharedTerm * following;
		} let;
	};
};



#endif /* TERM_FUNL__SHARED_TERM_H_ */
