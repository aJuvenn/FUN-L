/*
 * funl__term_evalutation.c
 *
 *  Created on: 31 juil. 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLTerm * flTermIncrementOutsideVarIds(const FLTerm * const term, const FLTermId minToIncrement, const FLTermId incrementOf, FLEnvironment * const env)
{

	switch (term->type){


	case FL_TERM_GLOBAL_VAR_ID:
		return flTermNewGlobalVarId(term->data.varId, env);


	case FL_TERM_VAR_ID:

		if (term->data.varId >= minToIncrement){
			return flTermNewVarId(term->data.varId + incrementOf, env);
		}

		return flTermNewVarId(term->data.varId, env);


	case FL_TERM_CALL:
		return flTermNewCall(flTermIncrementOutsideVarIds(term->data.call.func, minToIncrement, incrementOf, env),
				flTermIncrementOutsideVarIds(term->data.call.arg, minToIncrement, incrementOf, env),
				term->data.call.isACallByName,
				env);


	case FL_TERM_FUN:
		return flTermNewFun(flTermIncrementOutsideVarIds(term->data.funBody, minToIncrement + 1, incrementOf, env), env);


	case FL_TERM_LET:
		return flTermNewLet(flTermIncrementOutsideVarIds(term->data.let.affect, minToIncrement, incrementOf, env),
				flTermIncrementOutsideVarIds(term->data.let.following, minToIncrement + 1, incrementOf, env), env);

	default:
		break;

	}

	return NULL;
}



FLTerm * flTermReplaceIdByArg(const FLTerm * const body, const FLTermId idToReplace, const FLTerm * const arg, FLEnvironment * const env)
{
	if (body == NULL){
		return NULL;
	}

	switch(body->type){

	case FL_TERM_VAR_ID:

		if (body->data.varId == idToReplace){
			return flTermIncrementOutsideVarIds(arg, 0, idToReplace, env);
		}

		if (body->data.varId > idToReplace){
			return flTermNewVarId(body->data.varId - 1, env);
		}

		return flTermNewVarId(body->data.varId, env);


	case FL_TERM_GLOBAL_VAR_ID:
		return flTermNewGlobalVarId(body->data.varId, env);


	case FL_TERM_CALL:
		return flTermNewCall(flTermReplaceIdByArg(body->data.call.func, idToReplace, arg, env),
							 flTermReplaceIdByArg(body->data.call.arg, idToReplace, arg, env),
							 body->data.call.isACallByName,
							 env);

	case FL_TERM_FUN:
		return flTermNewFun(flTermReplaceIdByArg(body->data.funBody, idToReplace + 1, arg, env), env);


	case FL_TERM_LET:
		return flTermNewLet(flTermReplaceIdByArg(body->data.let.affect, idToReplace, arg, env),
							flTermReplaceIdByArg(body->data.let.following, idToReplace + 1, arg, env),
							env);


	default:
		break;
	}

	return NULL;
}




FLTerm * flTermEvaluationCall(const FLTerm * const call, FLEnvironment * const env)
{
	FLTerm * output;
	FLTerm * argToUse;
	const int isACallByName = call->data.call.isACallByName;
	FLTerm * evaluatedFunc = flTermEvaluation(call->data.call.func, env);


	if (evaluatedFunc == NULL){
		return NULL;
	}

	switch (evaluatedFunc->type){

	case FL_TERM_CALL:
	case FL_TERM_VAR_ID:

		if (isACallByName){
			argToUse = flTermCopy(call->data.call.arg, env);
		} else {
			argToUse = flTermEvaluation(call->data.call.arg, env);
		}

		return flTermNewCall(evaluatedFunc, argToUse, call->data.call.isACallByName, env);


	case FL_TERM_FUN:
	{
		if (isACallByName){
			argToUse = call->data.call.arg;
		} else {
			argToUse = flTermEvaluation(call->data.call.arg, env);
		}

		FLTerm * replaceArgs = flTermReplaceIdByArg(evaluatedFunc->data.funBody, 0, argToUse, env);
		flTermFree(evaluatedFunc, env);

		if (!isACallByName){
			flTermFree(argToUse, env);
		}

		if (replaceArgs == NULL){
			return NULL;
		}

		output = flTermEvaluation(replaceArgs, env);
		flTermFree(replaceArgs, env);

		if (output == NULL){
			return NULL;
		}

		return output;
	}


	case FL_TERM_GLOBAL_VAR_ID:
	{
		FLTerm * globalTerm = flGetTermFromGlobalId(evaluatedFunc->data.varId, env);
		flTermFree(evaluatedFunc, env);

		if (globalTerm == NULL){
			return NULL;
		}

		if (isACallByName){
			argToUse = flTermCopy(call->data.call.arg, env);
		} else {
			argToUse = flTermEvaluation(call->data.call.arg, env);
		}

		FLTerm * globalTermInCall = flTermNewCall(globalTerm, argToUse, call->data.call.isACallByName, env);
		output = flTermEvaluation(globalTermInCall, env);
		flTermFree(globalTermInCall, env);

		if (output == NULL){
			return NULL;
		}

		return output;
	}

	default:
		flTermFree(evaluatedFunc, env);
		return NULL;
	}
}






FLTerm * flTermEvaluationLet(const FLTerm * const let, FLEnvironment * const env)
{
	FLTerm * evaluatedAffect = flTermEvaluation(let->data.let.affect, env);

	if (evaluatedAffect == NULL){
		return NULL;
	}

	FLTerm * replacedAffect = flTermReplaceIdByArg(let->data.let.following, 0, evaluatedAffect, env);
	flTermFree(evaluatedAffect, env);

	if (replacedAffect == NULL){
		return NULL;
	}

	FLTerm * output = flTermEvaluation(replacedAffect, env);
	flTermFree(replacedAffect, env);

	if (output == NULL){
		return NULL;
	}

	return output;
}



FLTerm * flTermEvaluationFun(const FLTerm * const fun, FLEnvironment * const env)
{
	FLTerm * recCall = flTermEvaluation(fun->data.funBody, env);

	if (recCall == NULL){
		return NULL;
	}

	return flTermNewFun(recCall, env);
}


FLTerm * flTermEvaluation(const FLTerm * const term, FLEnvironment * const env)
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
		return flTermEvaluationFun(term, env);

	case FL_TERM_CALL:
		return flTermEvaluationCall(term, env);

	case FL_TERM_LET:
		return flTermEvaluationLet(term, env);

	default:
		break;
	}

	return NULL;
}




FLTerm * flTermEvaluationFromParseTree(const FLParseTree * const tree, FLEnvironment * const env)
{
	FLTerm * term = flTermFromParseTree(tree, env);

	if (term == NULL){
		fprintf(stderr, "flTermFromParseTree failed\n");
		return NULL;
	}

	FLTerm * evaluatedTerm = flTermEvaluation(term, env);
	flTermFree(term, env);

	if (evaluatedTerm == NULL){
		fprintf(stderr, "flTermEvaluation failed\n");
		return NULL;
	}

	return evaluatedTerm;
}

