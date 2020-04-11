/*
 * funl__shared_term_evaluation.c
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#include "../funl__include.h"



void flSharedTermCopyIfHasPathToArgRef(FLSharedTerm * term, FLSharedTerm * targetedFunc, FLSharedTerm * funcArg, FLEnvironment * env)
{
	if (term->copy != NULL
			|| FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED)){
		return;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED);

	switch (term->type){

	case FL_SHARED_TERM_ARGREF:
		if (term->ref == targetedFunc){
			term->copy = flSharedTermNewRef(funcArg, env);
		}
		break;

	case FL_SHARED_TERM_FUN:
		flSharedTermCopyIfHasPathToArgRef(term->fun.body, targetedFunc, funcArg, env);
		if (term->fun.body->copy != NULL){
			term->copy = flSharedTermNewFun(term->fun.body->copy, env);
		}
		break;

	case FL_SHARED_TERM_REF:
		flSharedTermCopyIfHasPathToArgRef(term->ref, targetedFunc, funcArg, env);
		if (term->ref->copy != NULL){
			term->copy = flSharedTermNewRef(term->ref->copy, env);
		}
		break;

	case FL_SHARED_TERM_CALL:
		flSharedTermCopyIfHasPathToArgRef(term->call.func, targetedFunc, funcArg, env);
		flSharedTermCopyIfHasPathToArgRef(term->call.arg, targetedFunc, funcArg, env);
		if (term->call.func->copy != NULL || term->call.arg->copy != NULL){
			FLSharedTerm * func = (term->call.func->copy) ? (term->call.func->copy) : (term->call.func);
			FLSharedTerm * arg = (term->call.arg->copy) ? (term->call.arg->copy) : (term->call.arg);
			term->copy = flSharedTermNewCall(func, arg, term->call.isACallByName, env);
		}
		break;

	case FL_SHARED_TERM_IF_ELSE:
		flSharedTermCopyIfHasPathToArgRef(term->ifElse.condition, targetedFunc, funcArg, env);
		flSharedTermCopyIfHasPathToArgRef(term->ifElse.thenValue, targetedFunc, funcArg, env);
		flSharedTermCopyIfHasPathToArgRef(term->ifElse.elseValue, targetedFunc, funcArg, env);
		if (term->ifElse.condition->copy || term->ifElse.thenValue->copy || term->ifElse.elseValue->copy){
			FLSharedTerm * condition = (term->ifElse.condition->copy) ? (term->ifElse.condition->copy) : (term->ifElse.condition);
			FLSharedTerm * thenValue = (term->ifElse.thenValue->copy) ? (term->ifElse.thenValue->copy) : (term->ifElse.thenValue);
			FLSharedTerm * elseValue = (term->ifElse.elseValue->copy) ? (term->ifElse.elseValue->copy) : (term->ifElse.elseValue);
			term->copy = flSharedTermNewIfElse(condition, thenValue, elseValue, env);
		}
		break;


	default:
		break;
	}

	FL_FLAG_UNSET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED);
}


void flSharedTermUntieCopy(FLSharedTerm * const term)
{
	if (term->copy == NULL
			|| FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED)){
		return;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED);

	term->copy = NULL;

	switch (term->type){

	case FL_SHARED_TERM_FUN:
		flSharedTermUntieCopy(term->fun.body);
		break;

	case FL_SHARED_TERM_REF:
		flSharedTermUntieCopy(term->ref);
		break;

	case FL_SHARED_TERM_CALL:
		flSharedTermUntieCopy(term->call.func);
		flSharedTermUntieCopy(term->call.arg);
		break;

	case FL_SHARED_TERM_IF_ELSE:
		flSharedTermUntieCopy(term->ifElse.condition);
		flSharedTermUntieCopy(term->ifElse.thenValue);
		flSharedTermUntieCopy(term->ifElse.elseValue);
		break;

	default:
		break;
	}

	FL_FLAG_UNSET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED);
}

#define EXIT_NEED_RECURSIVE_CALL 42

int flSharedTermEvaluateCall(FLSharedTerm * callTerm, FLEnvironment * env)
{
	int ret;

	ret = flSharedTermEvaluateOneStep(callTerm->call.func, env);

	if (ret != EXIT_SUCCESS){
		return ret;
	}

	if (!callTerm->call.isACallByName){

		ret = flSharedTermEvaluateOneStep(callTerm->call.arg, env);

		if (ret != EXIT_SUCCESS){
			return ret;
		}
	}

	FLSharedTerm * func = flSharedTermReferedBy(callTerm->call.func);

	if (func == NULL){
		return EXIT_FAILURE;
	}

	FLSharedTerm * arg = flSharedTermReferedBy(callTerm->call.arg);

	if (arg == NULL){
		return EXIT_FAILURE;
	}

	FL_SHARED_TERM_MOVE_REFERENCE(callTerm->call.func, func, env);
	FL_SHARED_TERM_MOVE_REFERENCE(callTerm->call.arg, arg, env);

	switch (func->type){

	case FL_SHARED_TERM_FUN:
	{
		FLSharedTerm * body = func->fun.body;

		/* Replacing arg reference by real arg in a copy, and updates dependencies */
		flSharedTermCopyIfHasPathToArgRef(body, func, arg, env);
		FLSharedTerm * copiedBody = (body->copy) ? (body->copy) : body;
		flSharedTermUntieCopy(body);

		/* Transforming the call into a reference to the copied body */

		FLSharedTerm * ref;
		FL_SHARED_TERM_SET_REFERENCE(ref, flSharedTermReferedBy(copiedBody));
		FL_SHARED_TERM_REMOVE_REFERENCE(callTerm->call.func, env);
		FL_SHARED_TERM_REMOVE_REFERENCE(callTerm->call.arg, env);

		callTerm->type = FL_SHARED_TERM_REF;
		callTerm->ref = ref;

		/* Returns a special code to force a recursive call */
		return EXIT_NEED_RECURSIVE_CALL;

	} break;

	case FL_SHARED_TERM_ARGREF:
	case FL_SHARED_TERM_CALL:
	case FL_SHARED_TERM_IF_ELSE:
		break;

	default:
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


int flSharedTermEvaluateIfElse(FLSharedTerm * ifElseTerm, FLEnvironment * env)
{
	/*
	 * TODO : choose better if else evaluation strategy ?
	 */

	int ret;

	ret = flSharedTermEvaluateOneStep(ifElseTerm->ifElse.condition, env);

	if (ret != EXIT_SUCCESS){
		return ret;
	}

	/*
	 * Now the condition can't be a reference term
	 *  TODO : generalize this operation to every ref sub members
	 */
	FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(ifElseTerm->ifElse.condition, env);

	switch (ifElseTerm->ifElse.condition->type){

	case FL_SHARED_TERM_INTEGER:
	{
		FLSharedTerm * termToEvaluate;

		if (ifElseTerm->ifElse.condition->integer){
			termToEvaluate = ifElseTerm->ifElse.thenValue;
		} else {
			termToEvaluate = ifElseTerm->ifElse.elseValue;
		}

		ret = flSharedTermEvaluateOneStep(termToEvaluate, env);

		if (ret != EXIT_SUCCESS){
			return ret;
		}

		/* Transforming term into reference to the member */

		FLSharedTerm * ref;
		FL_SHARED_TERM_SET_REFERENCE(ref, flSharedTermReferedBy(termToEvaluate));

		FL_SHARED_TERM_REMOVE_REFERENCE(ifElseTerm->ifElse.condition, env);
		FL_SHARED_TERM_REMOVE_REFERENCE(ifElseTerm->ifElse.thenValue, env);
		FL_SHARED_TERM_REMOVE_REFERENCE(ifElseTerm->ifElse.elseValue, env);

		ifElseTerm->type = FL_SHARED_TERM_REF;
		ifElseTerm->ref = ref;

	} break;

	case FL_SHARED_TERM_FUN:
		return EXIT_FAILURE;

	default:
	{
		/* Evaluate every member */

		ret = flSharedTermEvaluateOneStep(ifElseTerm->ifElse.thenValue, env);

		if (ret != EXIT_SUCCESS){
			return ret;
		}

		ret = flSharedTermEvaluateOneStep(ifElseTerm->ifElse.elseValue, env);

		if (ret != EXIT_SUCCESS){
			return ret;
		}

		FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(ifElseTerm->ifElse.condition, env);
		FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(ifElseTerm->ifElse.thenValue, env);
		FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(ifElseTerm->ifElse.elseValue, env);

	} break;

	}

	return EXIT_SUCCESS;
}



int flSharedTermEvaluateRef(FLSharedTerm * term, FLEnvironment * env)
{
	int ret;

	ret = flSharedTermEvaluateOneStep(term->ref, env);

	if (ret != EXIT_SUCCESS){
		return ret;
	}

	FLSharedTerm * referedTerm = flSharedTermReferedBy(term->ref);

	if (referedTerm == NULL){
		return EXIT_FAILURE;
	}

	FL_SHARED_TERM_MOVE_REFERENCE(term->ref, referedTerm, env);

	return EXIT_SUCCESS;
}


int flSharedTermEvaluateOneStep(FLSharedTerm * term, FLEnvironment * env)
{
	if (FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_EVALUATED | FL_SHARED_TERM_STATUS_BEING_EVALUATED)){
		return EXIT_SUCCESS;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_BEING_EVALUATED);

	int ret;

	switch(term->type){

	case FL_SHARED_TERM_FUN:
		ret = flSharedTermEvaluateOneStep(term->fun.body, env);

		if (ret == EXIT_SUCCESS){
			FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(term->fun.body, env);
		}
		break;

	case FL_SHARED_TERM_REF:
		ret = flSharedTermEvaluateRef(term, env);
		break;

	case FL_SHARED_TERM_CALL:
		ret = flSharedTermEvaluateCall(term, env);
		break;

	case FL_SHARED_TERM_IF_ELSE:
		ret = flSharedTermEvaluateIfElse(term, env);
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

	FL_FLAG_UNSET(term->status, FL_SHARED_TERM_STATUS_BEING_EVALUATED);

	if (ret != EXIT_SUCCESS){
		return ret;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_EVALUATED);

	return EXIT_SUCCESS;
}



int flSharedTermEvaluate(FLSharedTerm * term, FLEnvironment * env)
{
	 while (1){

		int ret = flSharedTermEvaluateOneStep(term, env);

		if (ret == EXIT_NEED_RECURSIVE_CALL){
			continue;
		}

		return ret;
	}
}
