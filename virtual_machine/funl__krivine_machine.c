/*
 * funl__krivine_machine.c
 *
 *  Created on: 31 mars 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"




FLTerm * flVMEvaluateTerm(FLTerm * term, FLEnvironment * const env)
{
	while (1){

		flTermPrint(term);
		printf("\n");
		flEnvironmentPrint(env);

		switch (term->type){

		case FL_TERM_CALL:
			flEnvironmentStackTerm(env, term->data.call.arg);
			term = term->data.call.func;
			break;


		case FL_TERM_FUN:

			if (env->stackSize == 0){
				printf("ENV AT THE END\nEvaluated Term: ");
				flTermPrint(term);
				printf("\n");
				flEnvironmentPrint(env);
				return term;
			}

			flEnvironmentStackClosure(env);
			term = term->data.funBody;
			break;


		case FL_TERM_VAR_ID:{
			term = flEnvironmentJump(env, term->data.varId);
			break;
		}


		default:
			fprintf(stderr, "FUN-L FATAL ERROR : INVALID INSTRUCTION\n");
			exit(EXIT_FAILURE);
		}
	}
}



void flVMEvaluateAndSaveTerm(FLTerm * term, FLEnvironment * const env)
{
	/*
	 * TODO : dirty, do better
	 */
	FLTerm * evaluated_term = flVMEvaluateTerm(term, env);
	flEnvironmentStackTerm(env, evaluated_term);
	flEnvironmentStackClosure(env);
}

