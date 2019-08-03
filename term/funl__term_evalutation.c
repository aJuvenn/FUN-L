/*
 * funl__term_evalutation.c
 *
 *  Created on: 31 juil. 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLTerm * flTermIncrementOutsideVarIds(const FLTerm * const term, const FLTermId minToIncrement, const FLTermId incrementOf)
{

	switch (term->type){


	case FL_TERM_GLOBAL_VAR_ID:
		return flTermNewGlobalVarId(term->data.varId);


	case FL_TERM_VAR_ID:

		if (term->data.varId >= minToIncrement){
			return flTermNewVarId(term->data.varId + incrementOf);
		}

		return flTermNewVarId(term->data.varId);


	case FL_TERM_CALL:
		return flTermNewCall(flTermIncrementOutsideVarIds(term->data.call.func, minToIncrement, incrementOf),
							 flTermIncrementOutsideVarIds(term->data.call.arg, minToIncrement, incrementOf));


	case FL_TERM_FUN:
		return flTermNewFun(flTermIncrementOutsideVarIds(term->data.funBody, minToIncrement + 1, incrementOf));


	case FL_TERM_LET:
		return flTermNewLet(flTermIncrementOutsideVarIds(term->data.let.affect, minToIncrement, incrementOf),
							flTermIncrementOutsideVarIds(term->data.let.following, minToIncrement + 1, incrementOf));

	default:
		break;

	}

	return NULL;
}



FLTerm * flTermReplaceIdByArg(const FLTerm * const body, const FLTermId idToReplace, const FLTerm * const arg)
{
	if (body == NULL){
		return NULL;
	}

	switch(body->type){

	case FL_TERM_VAR_ID:

		if (body->data.varId == idToReplace){
			return flTermIncrementOutsideVarIds(arg, 0, idToReplace);
		}

		if (body->data.varId > idToReplace){
			return flTermNewVarId(body->data.varId - 1);
		}

		return flTermNewVarId(body->data.varId);


	case FL_TERM_GLOBAL_VAR_ID:
		return flTermNewGlobalVarId(body->data.varId);


	case FL_TERM_CALL:
		return flTermNewCall(flTermReplaceIdByArg(body->data.call.func, idToReplace, arg),
				flTermReplaceIdByArg(body->data.call.arg, idToReplace, arg));


	case FL_TERM_FUN:
		return flTermNewFun(flTermReplaceIdByArg(body->data.funBody, idToReplace + 1, arg));


	case FL_TERM_LET:
		return flTermNewLet(flTermReplaceIdByArg(body->data.let.affect, idToReplace, arg),
							flTermReplaceIdByArg(body->data.let.following, idToReplace + 1, arg));


	default:
		break;
	}

	return NULL;
}




FLTerm * flTermEvaluationCall(const FLTerm * const call, FLEnvironment * const env)
{
	FLTerm * output;
	FLTerm * evaluatedFunc = flTermEvaluation(call->data.call.func, env);


	if (evaluatedFunc == NULL){
		return NULL;
	}

	switch (evaluatedFunc->type){

	case FL_TERM_CALL:
	case FL_TERM_VAR_ID:
		return flTermNewCall(evaluatedFunc, flTermCopy(call->data.call.arg));

	case FL_TERM_FUN:
	{
		FLTerm * replaceArgs = flTermReplaceIdByArg(evaluatedFunc->data.funBody, 0, call->data.call.arg);

		output = flTermEvaluation(replaceArgs, env);

		/*
		 * TODO : free
		 */
		//flTermFree(evaluatedFunc);
		//flTermFree(replaceArgs);
		return output;
	}


	case FL_TERM_GLOBAL_VAR_ID:
	{
		FLTerm * globalTerm = flGetTermFromGlobalId(evaluatedFunc->data.varId, env);

		// TODO free
		//flTermFree(evaluatedFunc);

		if (globalTerm == NULL){
			return NULL;
		}

		FLTerm * globalTermInCall = flTermNewCall(globalTerm, flTermCopy(call->data.call.arg));

		output = flTermEvaluation(globalTermInCall, env);

		// TODO free
		//flTermFree(globalTermInCall);

		return output;
	}

	default:
		break;
	}

	return NULL;
}



FLTerm * flTermEvaluationLet(const FLTerm * const let, FLEnvironment * const env)
{
	FLTerm * evaluatedAffect = flTermEvaluation(let->data.let.affect, env);

	if (evaluatedAffect == NULL){
		return NULL;
	}

	FLTerm * replacedAffect = flTermReplaceIdByArg(let->data.let.following, 0, evaluatedAffect);
	FLTerm * output = flTermEvaluation(replacedAffect, env);
	/*
	 * TODO : free
	 */

	//flTermFree(evaluatedAffect);
	// flTermFree(replacedAffect);

	return output;
}



FLTerm * flTermEvaluation(const FLTerm * const term, FLEnvironment * const env)
{
	if (term == NULL){
		return NULL;
	}

	switch (term->type){

	case FL_TERM_FUN:{

		FLTerm * recCall = flTermEvaluation(term->data.funBody, env);

		if (recCall == NULL){
			return NULL;
		}

		return flTermNewFun(recCall);
	}

	case FL_TERM_VAR_ID:
		return flTermNewVarId(term->data.varId);

	case FL_TERM_GLOBAL_VAR_ID:
		return flTermNewGlobalVarId(term->data.varId);

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
	flTermFree(term);

	if (evaluatedTerm == NULL){
		fprintf(stderr, "flTermEvaluation failed\n");
		return NULL;
	}

	return evaluatedTerm;
}

