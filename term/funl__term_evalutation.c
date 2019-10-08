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

	case FL_TERM_INTEGER:
		return flTermNewInteger(term->data.integer, env);

	case FL_TERM_IF_ELSE:
		return flTermNewIfElse(flTermIncrementOutsideVarIds(term->data.ifElse.condition, minToIncrement, incrementOf, env),
							   flTermIncrementOutsideVarIds(term->data.ifElse.thenValue, minToIncrement, incrementOf, env),
							   flTermIncrementOutsideVarIds(term->data.ifElse.elseValue, minToIncrement, incrementOf, env),
							   env);

	case FL_TERM_OPCALL:{

		size_t nbArgs = term->data.opCall.nbArguments;
		FLTerm ** argumentsCopy;
		FL_SIMPLE_ALLOC(argumentsCopy, nbArgs);

		for (size_t i = 0 ; i < nbArgs ; i++){
			argumentsCopy[i] = flTermIncrementOutsideVarIds(term->data.opCall.arguments[i], minToIncrement, incrementOf, env);
		}

		return flTermNewOpCall(term->data.opCall.op, nbArgs, argumentsCopy, env);
	}



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

	case FL_TERM_INTEGER:
		return flTermNewInteger(body->data.integer, env);

	case FL_TERM_IF_ELSE:
		return flTermNewIfElse(flTermReplaceIdByArg(body->data.ifElse.condition, idToReplace, arg, env),
							   flTermReplaceIdByArg(body->data.ifElse.thenValue, idToReplace, arg, env),
							   flTermReplaceIdByArg(body->data.ifElse.elseValue, idToReplace, arg, env),
							   env);

	case FL_TERM_OPCALL:{

		size_t nbArgs = body->data.opCall.nbArguments;
		FLTerm ** argumentsCopy;
		FL_SIMPLE_ALLOC(argumentsCopy, nbArgs);

		for (size_t i = 0 ; i < nbArgs ; i++){
			argumentsCopy[i] = flTermReplaceIdByArg(body->data.opCall.arguments[i], idToReplace, arg, env);
		}

		return flTermNewOpCall(body->data.opCall.op, nbArgs, argumentsCopy, env);
	}


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



FLTerm * flTermApplyOp(FlTokenOperatorData const op, size_t const nbArguments, const long long int * const arguments, FLEnvironment * const env){

	switch (op){

	case FL_OPERATOR_PLUS:{

		long long int output = 0;

		for (size_t i = 0 ; i < nbArguments ; i++){
			output += arguments[i];
		}

		return flTermNewInteger(output, env);
	}


	case FL_OPERATOR_MINUS:{

		long long int output = arguments[0];

		for (size_t i = 1 ; i < nbArguments ; i++){
			output -= arguments[i];
		}

		return flTermNewInteger(output, env);
	}

	case FL_OPERATOR_TIMES:{
		long long int output = 1;

		for (size_t i = 0 ; i < nbArguments ; i++){
			output *= arguments[i];
		}

		return flTermNewInteger(output, env);
	}

	case FL_OPERATOR_DIV:{
		long long int output = arguments[0];

		for (size_t i = 1 ; i < nbArguments ; i++){
			output /= arguments[i];
		}

		return flTermNewInteger(output, env);
	}

	case FL_OPERATOR_MOD:{
		long long int output = arguments[0];

		for (size_t i = 1 ; i < nbArguments ; i++){
			output = output % arguments[i];
		}

		return flTermNewInteger(output, env);
	}

	case FL_OPERATOR_EQ:{

		for (size_t i = 1 ; i < nbArguments ; i++){
			if (arguments[i] != arguments[0]){
				return flTermNewInteger(0, env);
			}
		}

		return flTermNewInteger(1, env);
	}

	case FL_OPERATOR_LT:{

		for (size_t i = 1 ; i < nbArguments ; i++){
			if (!(arguments[i-1] < arguments[i])){
				return flTermNewInteger(0, env);
			}
		}

		return flTermNewInteger(1, env);
	}

	case FL_OPERATOR_LEQ:{

		for (size_t i = 1 ; i < nbArguments ; i++){
			if (!(arguments[i-1] <= arguments[i])){
				return flTermNewInteger(0, env);
			}
		}

		return flTermNewInteger(1, env);
	}

	case FL_OPERATOR_GT:{

		for (size_t i = 1 ; i < nbArguments ; i++){
			if (!(arguments[i-1] > arguments[i])){
				return flTermNewInteger(0, env);
			}
		}

		return flTermNewInteger(1, env);
	}

	case FL_OPERATOR_GEQ:{

		for (size_t i = 1 ; i < nbArguments ; i++){
			if (!(arguments[i-1] >= arguments[i])){
				return flTermNewInteger(0, env);
			}
		}

		return flTermNewInteger(1, env);
	}

	default:
		return NULL;
	}
}


FLTerm * flTermEvaluationOpCall(const FLTerm * const call, FLEnvironment * const env)
{
	size_t nbArguments = call->data.opCall.nbArguments;
	FLTerm ** evaluatedArguments;
	FL_SIMPLE_ALLOC(evaluatedArguments, nbArguments);
	int allIsInteger = 1;

	size_t i;

	for (i = 0 ; i < nbArguments ; i++){

		evaluatedArguments[i] = flTermEvaluation(call->data.opCall.arguments[i], env);

		if (evaluatedArguments[i] == NULL){
			goto INVALID;
		}

		if (evaluatedArguments[i]->type != FL_TERM_INTEGER){
			allIsInteger = 0;
		}
	}

	FlTokenOperatorData op = call->data.opCall.op;
	FLTerm * output;

	if (allIsInteger){

		long long int * integerList;
		FL_SIMPLE_ALLOC(integerList, nbArguments);

		for (size_t i = 0 ; i < nbArguments ; i++){
			integerList[i] = evaluatedArguments[i]->data.integer;
			flTermFree(evaluatedArguments[i], env);
		}

		free(evaluatedArguments);
		output = flTermApplyOp(op, nbArguments, integerList, env);
		free(integerList);

	} else {

		output = flTermNewOpCall(op, nbArguments, evaluatedArguments, env);

		if (output == NULL){
			goto INVALID;
		}
	}

	return output;

	INVALID:{

		for (size_t j = 0 ; j < i ; j++){
			flTermFree(evaluatedArguments[j], env);
		}

		free(evaluatedArguments);

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



FLTerm * flTermEvaluationIfElse(const FLTerm * const ifElse, FLEnvironment * const env)
{
	FLTerm * evaluatedCondition = flTermEvaluation(ifElse->data.ifElse.condition, env);

	if (evaluatedCondition == NULL){
		return NULL;
	}

	if (evaluatedCondition->type != FL_TERM_INTEGER){
		return flTermNewIfElse(evaluatedCondition,
				flTermCopy(ifElse->data.ifElse.thenValue, env),
				flTermCopy(ifElse->data.ifElse.elseValue, env),
				env);
	}

	FLTerm * termToEvaluate;

	if (evaluatedCondition->data.integer){
		termToEvaluate = ifElse->data.ifElse.thenValue;
	} else {
		termToEvaluate = ifElse->data.ifElse.elseValue;
	}

	flTermFree(evaluatedCondition, env);

	return flTermEvaluation(termToEvaluate, env);
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
		// return flTermNewGlobalVarId(term->data.varId, env);
		return flTermEvaluation(env->globalTerms[term->data.varId], env);

	case FL_TERM_FUN:
		return flTermEvaluationFun(term, env);

	case FL_TERM_CALL:
		return flTermEvaluationCall(term, env);

	case FL_TERM_LET:
		return flTermEvaluationLet(term, env);

	case FL_TERM_INTEGER:
		return flTermNewInteger(term->data.integer, env);

	case FL_TERM_OPCALL:
		return flTermEvaluationOpCall(term, env);

	case FL_TERM_IF_ELSE:
		return flTermEvaluationIfElse(term, env);

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

