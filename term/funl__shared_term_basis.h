/*
 * funl__shared_term_basis.h
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#ifndef TERM_FUNL__BASIS_H_
#define TERM_FUNL__BASIS_H_

#include "../funl__include.h"

FLSharedTerm * flSharedTermNewFun(FLSharedTerm * funBody, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewRef(FLSharedTerm * pointedTerm, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewArgRef(FLSharedTerm * pointedFun, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewInteger(long long int integer, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewCall(FLSharedTerm * fun, FLSharedTerm * arg, int isACallByName, FLEnvironment * const env);
FLSharedTerm * flSharedTermNewIfElse(FLSharedTerm * condition, FLSharedTerm * thenValue, FLSharedTerm * elseValue, FLEnvironment * const env);
void flSharedTermFree(FLSharedTerm * term, FLEnvironment * const env);

int flSharedTermSaveToPDF(const FLSharedTerm * term, const char * const outputPath);


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
				flSharedTermFree(_FL_TMP_REF_TO_REMOVE, (env));\
			}\
		} while(0)


#define FL_SHARED_TERM_MOVE_REFERENCE(place, newPointedTerm, env)\
		do {\
			FLSharedTerm * _FL_TMP = (place);\
			(place) = (newPointedTerm);\
			(place)->nbReferences++;\
			if ((_FL_TMP->nbReferences--) == 1){\
				flSharedTermFree(_FL_TMP, (env));\
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
				flSharedTermFree(_FL_TMP_ORIGINAL_REF, (env));\
			}\
		} while (0)



#endif /* TERM_FUNL__BASIS_H_ */
