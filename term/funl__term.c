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


FLTerm * flTermNewInteger(long long int integer, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_INTEGER;
	output->data.integer = integer;

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

FLTerm * flTermNewOpCall(FlTokenOperatorData op, size_t nbArguments, FLTerm ** arguments, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_OPCALL;
	output->data.opCall.op = op;
	output->data.opCall.nbArguments = nbArguments;
	output->data.opCall.arguments = arguments;

	return output;
}


FLTerm * flTermNewIfElse(FLTerm * condition, FLTerm * thenValue, FLTerm * elseValue, FLEnvironment * const env)
{
	FLTerm * output = flTermNew(env);

	output->type = FL_TERM_IF_ELSE;
	output->data.ifElse.condition = condition;
	output->data.ifElse.thenValue = thenValue;
	output->data.ifElse.elseValue = elseValue;

	return output;
}






FLTerm * flTermCopy(const FLTerm * const term, FLEnvironment * const env)
{
	if (term == NULL){
		return NULL;
	}

	switch (term->type){

	case FL_TERM_VAR_ID:
		return flTermNewVarId(term->data.varId, env);

	case FL_TERM_INTEGER:
		return flTermNewInteger(term->data.integer, env);

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

	case FL_TERM_IF_ELSE:
		return flTermNewIfElse(flTermCopy(term->data.ifElse.condition, env),
				flTermCopy(term->data.ifElse.thenValue, env),
				flTermCopy(term->data.ifElse.elseValue, env),
				env);

	case FL_TERM_OPCALL:{

		size_t nbArguments = term->data.opCall.nbArguments;
		FLTerm ** argumentsCopy;
		FL_SIMPLE_ALLOC(argumentsCopy, nbArguments);

		for (size_t i = 0 ; i < nbArguments ; i++){
			argumentsCopy[i] = flTermCopy(term->data.opCall.arguments[i], env);
		}

		return flTermNewOpCall(term->data.opCall.op, nbArguments, argumentsCopy, env);
	}

	default:
		return NULL;
	}
}




void flTermFree(FLTerm * term, FLEnvironment * const env)
{
	if (term == NULL){
		return;
	}

	FLTerm ** availableTerms = env->availableTerms;
	const size_t remainigTermsStackOffset = env->maxNbAvailableTerms - 1;

	FLTerm * termToFree;

	size_t nbRemainingTermsToFree = 1;
	availableTerms[remainigTermsStackOffset] = term;

	while (nbRemainingTermsToFree != 0){

		termToFree = availableTerms[remainigTermsStackOffset - (--nbRemainingTermsToFree)];

		switch (termToFree->type){

		case FL_TERM_FUN:
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = termToFree->data.funBody;
			break;

		case FL_TERM_CALL:{
			FLTermCallData data = termToFree->data.call;
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = data.func;
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = data.arg;
			break;
		}

		case FL_TERM_LET:{
			FLTermLetData data = termToFree->data.let;
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = data.affect;
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = data.following;
			break;
		}

		case FL_TERM_IF_ELSE:{
			FLTermIfElseData data = termToFree->data.ifElse;
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = data.condition;
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = data.thenValue;
			availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = data.elseValue;
			break;
		}

		case FL_TERM_OPCALL:{

			FLTermOpCallData data = termToFree->data.opCall;
			FLTerm ** arguments = data.arguments;
			const size_t nbArguments = data.nbArguments;

			for (size_t i = 0 ; i < nbArguments ; i++){
				availableTerms[remainigTermsStackOffset - (nbRemainingTermsToFree++)] = arguments[i];
			}

			free(arguments);

			break;
		}

		default:
			break;
		}

		availableTerms[env->nbAvailableTerms++] = termToFree;
	}
}


