/*
 * funl__krivine_machine.h
 *
 *  Created on: 31 mars 2019
 *      Author: ajuvenn
 */

#ifndef FUNL__KRIVINE_MACHINE_H_
#define FUNL__KRIVINE_MACHINE_H_

#include "../funl__include.h"


FLTerm * flVMEvaluateTerm(FLTerm * term, FLEnvironment * const env);

void flVMEvaluateAndSaveTerm(FLTerm * term, FLEnvironment * const env);




#endif /* FUNL__KRIVINE_MACHINE_H_ */
