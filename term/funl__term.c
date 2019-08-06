/*
 * funl__term.c
 *
 *  Created on: 1 avr. 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLTerm * flTermNew(FLEnvironment * const env)
{
	if (env->nbAvailableTerms == 0){
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	return env->availableTerms[--(env->nbAvailableTerms)];
}


FLTerm * flTermNewFun(FLTerm * funBody, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_FUN;
	output->data.funBody = funBody;

	return output;
}


FLTerm * flTermNewVarId(FLTermId id, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_VAR_ID;
	output->data.varId = id;

	return output;
}


FLTerm * flTermNewGlobalVarId(FLTermId id, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_GLOBAL_VAR_ID;
	output->data.varId = id;

	return output;
}



FLTerm * flTermNewCall(FLTerm * fun, FLTerm * arg, int isACallByName, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_CALL;
	output->data.call.func = fun;
	output->data.call.arg = arg;
	output->data.call.isACallByName = isACallByName;

	return output;
}


FLTerm * flTermNewLet(FLTerm * affect, FLTerm * following, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_LET;
	output->data.let.affect = affect;
	output->data.let.following = following;

	return output;
}





void flTermPrint(const FLTerm * const term)
{
	if (term == NULL){
		printf("(NULL)");
		return;
	}

	switch (term->type){

	case FL_TERM_VAR_ID:
		printf("%u", term->data.varId);
		return;

	case FL_TERM_GLOBAL_VAR_ID:
		printf("<g>%u", term->data.varId);
		return;

	case FL_TERM_FUN:
		printf("L.");
		flTermPrint(term->data.funBody);
		return;

	case FL_TERM_CALL:
		printf(term->data.call.isACallByName ? "[" : "(");
		flTermPrint(term->data.call.func);
		printf(" ");
		flTermPrint(term->data.call.arg);
		printf(term->data.call.isACallByName ? "]" : ")");
		return;


	case FL_TERM_LET:
		printf("let ");
		flTermPrint(term->data.let.affect);
		printf(" in ");
		flTermPrint(term->data.let.following);
		return;

	default:
		printf("<INVALID TERM>");
		return;
	}
}






FLTerm * flTermCopy(const FLTerm * const term, FLEnvironment * const env)
{
	if (term == NULL){
		return NULL;
	}

	switch (term->type){

	case FL_TERM_VAR_ID:
		return flTermNewVarId(term->data.varId, env);

	case FL_TERM_GLOBAL_VAR_ID:
		return flTermNewGlobalVarId(term->data.varId, env);

	case FL_TERM_FUN:
		return flTermNewFun(flTermCopy(term->data.funBody, env), env);

	case FL_TERM_CALL:
		return flTermNewCall(flTermCopy(term->data.call.func, env),
							 flTermCopy(term->data.call.arg, env),
							 term->data.call.isACallByName,
							 env);

	case FL_TERM_LET:
		return flTermNewLet(flTermCopy(term->data.let.affect, env),
							flTermCopy(term->data.let.following, env),
							env);

	default:
		return NULL;
	}
}




void flTermFree(FLTerm * term, FLEnvironment * const env)
{
	if (term == NULL){
		return;
	}

	switch (term->type){

	case FL_TERM_FUN:
		flTermFree(term->data.funBody, env);
		break;

	case FL_TERM_CALL:
		flTermFree(term->data.call.func, env);
		flTermFree(term->data.call.arg, env);
		break;

	case FL_TERM_LET:
		flTermFree(term->data.let.affect, env);
		flTermFree(term->data.let.following, env);
		break;

	default:
		break;

	}

	env->availableTerms[env->nbAvailableTerms++] = term;
}


