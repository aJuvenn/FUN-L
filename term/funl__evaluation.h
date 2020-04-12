/*
 * funl__evaluation.h
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#ifndef TERM_FUNL__EVALUATION_H_
#define TERM_FUNL__EVALUATION_H_

#include "../funl__include.h"


/*
int flSharedTermEvaluate(FLSharedTerm * term, FLEnvironment * env);
int flSharedTermEvaluateOneStep(FLSharedTerm * term, FLEnvironment * env);
*/

FLSharedTerm * flSharedTermFromParseTree(const FLParseTree * const tree, FLEnvironment * const env);


FLSharedTerm * flSharedTermEvaluate(FLSharedTerm * term, FLEnvironment * env);


#endif /* TERM_FUNL__EVALUATION_H_ */
