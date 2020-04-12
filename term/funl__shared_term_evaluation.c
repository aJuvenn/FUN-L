/*
 * funl__shared_term_evaluation.c
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#include "../funl__include.h"


#define EXIT_NEED_RECURSIVE_CALL 42


FLSharedTerm * flSharedTermReplaceLocalVar(FLSharedTerm * term, const size_t idToReplace, FLSharedTerm * replacementTerm, FLEnvironment * env)
{
	if (term == NULL){
		return NULL;
	}

	switch (term->type){

	case FL_SHARED_TERM_VAR:

		if (term->var.id == idToReplace && !(term->var.isGlobal)){
			return replacementTerm;
		}

		return term;

	case FL_SHARED_TERM_INTEGER:
		return term;

	case FL_SHARED_TERM_FUN:
	{
		FLSharedTerm * body = flSharedTermReplaceLocalVar(term->fun.body, idToReplace + 1, replacementTerm, env);

		if (body == NULL){
			return NULL;
		}

		if (body == term->fun.body){
			return term;
		}

		return flSharedTermNewFun(body, env);
	} break;

	case FL_SHARED_TERM_REF:
		return flSharedTermReplaceLocalVar(term->ref, idToReplace, replacementTerm, env);

	case FL_SHARED_TERM_CALL:
	{
		FLSharedTerm * func = flSharedTermReplaceLocalVar(term->call.func, idToReplace, replacementTerm, env);

		if (func == NULL){
			return NULL;
		}

		FLSharedTerm * arg = flSharedTermReplaceLocalVar(term->call.arg, idToReplace, replacementTerm, env);

		if (arg == NULL){
			return NULL;
		}

		if (func == term->call.func && arg == term->call.arg){
			return term;
		}

		return flSharedTermNewCall(func, arg, term->call.isACallByName, env);
	} break;

	case FL_SHARED_TERM_IF_ELSE:
	{
		FLSharedTerm * condition = flSharedTermReplaceLocalVar(term->ifElse.condition, idToReplace, replacementTerm, env);

		if (condition == NULL){
			return NULL;
		}

		FLSharedTerm * thenValue = flSharedTermReplaceLocalVar(term->ifElse.thenValue, idToReplace, replacementTerm, env);

		if (thenValue == NULL){
			return NULL;
		}

		FLSharedTerm * elseValue = flSharedTermReplaceLocalVar(term->ifElse.elseValue, idToReplace, replacementTerm, env);

		if (elseValue == NULL){
			return NULL;
		}

		if (condition == term->ifElse.condition
				&& thenValue == term->ifElse.thenValue
				&& elseValue == term->ifElse.elseValue){
			return term;
		}

		return flSharedTermNewIfElse(condition, thenValue, elseValue, env);
	} break;


	case FL_SHARED_TERM_LET:
	{
		int isRecursive = (term->let.isRecursive);
		size_t idInAffect = isRecursive ? idToReplace+1 : idToReplace;
		FLSharedTerm * affect = flSharedTermReplaceLocalVar(term->let.affect, idInAffect, replacementTerm, env);

		if (affect == NULL){
			return NULL;
		}

		if (term->let.following == NULL){
			if (affect == term->let.affect){
				return term;
			}
			return flSharedTermNewLet(affect, NULL, isRecursive, env);
		}

		FLSharedTerm * following = flSharedTermReplaceLocalVar(term->let.following, idToReplace+1, replacementTerm, env);

		if (following == NULL){
			return NULL;
		}

		if (affect == term->let.affect && following == term->let.following){
			return term;
		}

		return flSharedTermNewLet(affect, following, isRecursive, env);
	} break;

	default:
		break;
	}

	return NULL;
}


#define FL_DUMMY_EVALUATION

FLSharedTerm * flSharedTermEvaluateOneStep(FLSharedTerm * term, FLEnvironment * env)
{
	int ret;

	switch(term->type){

	case FL_SHARED_TERM_VAR:
	{
		FL_DEBUG_PRINT("FL_SHARED_TERM_VAR: %s var %s of id %zu", term->var.isGlobal ? "global" : "local",
				term->var.isGlobal ? env->globalVarNameStack[term->var.id] : "", term->var.id);

		if (term->var.isGlobal){
			FL_DEBUG_PRINT("\tReturning %s", env->globalVarNameStack[term->var.id]);
			return term;
		}

		FLSharedTerm * correspoundingTerm = flEnvironmentExecutionTermFromVarId(env, term->var.id, term->var.isGlobal);

		if (correspoundingTerm == NULL){
			FL_DEBUG_PRINT("\tCorresponding term is NULL, returning %p", term);
			return term;
		}

		FL_DEBUG_PRINT("\tReturning %p", correspoundingTerm);
		return correspoundingTerm;
	} break;

	case FL_SHARED_TERM_INTEGER:
		FL_DEBUG_PRINT("FL_SHARED_TERM_INTEGER: %llu", term->integer);
		FL_DEBUG_PRINT("\tReturning %p", term);
		return term;

	case FL_SHARED_TERM_FUN:
	{
		FL_DEBUG_PRINT("FL_SHARED_TERM_FUN");

		ret = flEnvironmentPushExecutionTerm(env, NULL);

		if (ret != EXIT_SUCCESS){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		FLSharedTerm * evaluatedBody = flSharedTermEvaluate(term->fun.body, env);

		flEnvironmentPopExecutionTerm(env, 1);

		if (evaluatedBody == NULL){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		if (evaluatedBody == term->fun.body){
			FL_DEBUG_PRINT("\tReturning %p", term);
			return term;
		}

		return flSharedTermNewFun(evaluatedBody, env);
	} break;

	case FL_SHARED_TERM_CALL:
	{
		FL_DEBUG_PRINT("FL_SHARED_TERM_CALL");

		FLSharedTerm * evaluatedArg;

		if (term->call.isACallByName){
			evaluatedArg = term->call.arg;
		} else {
			evaluatedArg = flSharedTermEvaluate(term->call.arg, env);
			if (evaluatedArg == NULL){
				FL_DEBUG_PRINT("\tReturning NULL");
				return NULL;
			}
		}

		FLSharedTerm * evaluatedFunc = flSharedTermEvaluate(term->call.func, env);

		if (evaluatedFunc == NULL
				|| evaluatedFunc->type == FL_SHARED_TERM_INTEGER){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

#ifdef FL_DUMMY_EVALUATION
		if (evaluatedFunc->type == FL_SHARED_TERM_VAR && evaluatedFunc->var.isGlobal){

			FLSharedTerm * globTerm = flEnvironmentExecutionTermFromVarId(env, evaluatedFunc->var.id, 1);
			if (globTerm == NULL){
				FL_DEBUG_PRINT("\tReturning NULL");
				return NULL;
			}

			FLSharedTerm * newCall = flSharedTermNewCall(globTerm, evaluatedArg, term->call.isACallByName, env);
			if (newCall == NULL){
				FL_DEBUG_PRINT("\tReturning NULL");
				return NULL;
			}

			return flSharedTermEvaluate(newCall, env);
		}
#endif

		if (evaluatedFunc->type != FL_SHARED_TERM_FUN){
			if (evaluatedFunc == term->call.func && evaluatedArg == term->call.arg){
				return term;
			}
			return flSharedTermNewCall(evaluatedFunc, evaluatedArg, term->call.isACallByName, env);
		}

#ifndef FL_DUMMY_EVALUATION

		ret = flEnvironmentPushExecutionTerm(env, evaluatedArg);

		if (ret != EXIT_SUCCESS){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		FLSharedTerm * output = flSharedTermEvaluate(evaluatedFunc->fun.body, env);
		flEnvironmentPopExecutionTerm(env, 1);

		return output;
#else

		FLSharedTerm * replacedArgBody = flSharedTermReplaceLocalVar(evaluatedFunc->fun.body, 0, evaluatedArg, env);

		if (replacedArgBody == NULL){
			return NULL;
		}

		return flSharedTermEvaluate(replacedArgBody, env);
#endif
	} break;

	case FL_SHARED_TERM_IF_ELSE:
	{
		FL_DEBUG_PRINT("FL_SHARED_TERM_IF_ELSE");

		/* TODO : better strategy */
		FLSharedTerm * evaluatedCondition = flSharedTermEvaluate(term->ifElse.condition, env);

		if (evaluatedCondition == NULL){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		if (evaluatedCondition->type == FL_SHARED_TERM_INTEGER){
			if (evaluatedCondition->integer){
				return flSharedTermEvaluate(term->ifElse.thenValue, env);
			} else {
				return flSharedTermEvaluate(term->ifElse.elseValue, env);
			}
		}

#ifndef FL_DUMMY_EVALUATION
		FLSharedTerm * evaluatedThenValue = flSharedTermEvaluate(term->ifElse.thenValue, env);

		if (evaluatedThenValue == NULL){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		FLSharedTerm * evaluatedElseValue = flSharedTermEvaluate(term->ifElse.elseValue, env);

		if (evaluatedElseValue == NULL){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		if (evaluatedCondition == term->ifElse.condition
				&& evaluatedThenValue == term->ifElse.thenValue
				&&  evaluatedElseValue == term->ifElse.elseValue){
			return term;
		}
		 return flSharedTermNewIfElse(evaluatedCondition, evaluatedThenValue, evaluatedElseValue, env);
#else

		if (evaluatedCondition == term->ifElse.condition){
			return term;
		}

		return flSharedTermNewIfElse(evaluatedCondition, term->ifElse.thenValue, term->ifElse.elseValue, env);
#endif
	} break;

	case FL_SHARED_TERM_REF:
		FL_DEBUG_PRINT("FL_SHARED_TERM_REF");

		return flSharedTermEvaluate(term->ref, env);


	case FL_SHARED_TERM_LET:
	{
		FL_DEBUG_PRINT("FL_SHARED_TERM_LET");

		if (term->let.isRecursive){
			ret = flEnvironmentPushExecutionTerm(env, term->let.affect);
			if (ret != EXIT_SUCCESS){
				FL_DEBUG_PRINT("\tReturning NULL");
				return NULL;
			}
		}

		FLSharedTerm * evaluatedAffect = flSharedTermEvaluate(term->let.affect, env);

		if (term->let.isRecursive){
			flEnvironmentPopExecutionTerm(env, 1);
		}

		if (evaluatedAffect == NULL){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		if (term->let.following == NULL){
			return evaluatedAffect;
		}

		ret = flEnvironmentPushExecutionTerm(env, evaluatedAffect);

		if (ret != EXIT_SUCCESS){
			FL_DEBUG_PRINT("\tReturning NULL");
			return NULL;
		}

		FLSharedTerm * output = flSharedTermEvaluate(term->let.following, env);

		flEnvironmentPopExecutionTerm(env, 1);

		return output;
	} break;

	default:
		break;
	}

	FL_DEBUG_PRINT("\tDefault: returning NULL");
	return NULL;
}



FLSharedTerm * flSharedTermEvaluate(FLSharedTerm * term, FLEnvironment * env)
{
	if (term == NULL){
		return NULL;
	}

	FLSharedTerm * output = term;
	FLSharedTerm * newOutput;

	while (1){
		newOutput = flSharedTermEvaluateOneStep(output, env);

		return newOutput;

		if (newOutput == NULL){
			return NULL;
		}

		if (newOutput == output){
			return output;
		}

		output = newOutput;
	}
}


#if 0

int flSharedTermEvaluateCall(FLSharedTerm * callTerm, FLEnvironment * env)
{
	int needForRecursiveCall = 0;
	int ret;

	ret = flSharedTermEvaluateOneStep(callTerm->call.func, env);

	if (ret == EXIT_FAILURE){
		return ret;
	} else if (ret == EXIT_NEED_RECURSIVE_CALL){
		needForRecursiveCall = 1;
	}

	if (!callTerm->call.isACallByName){

		ret = flSharedTermEvaluateOneStep(callTerm->call.arg, env);

		if (ret == EXIT_FAILURE){
			return ret;
		} else if (ret == EXIT_NEED_RECURSIVE_CALL){
			needForRecursiveCall = 1;
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
		/* TODO */
	} break;

	case FL_SHARED_TERM_ARGREF:
	case FL_SHARED_TERM_CALL:
	case FL_SHARED_TERM_IF_ELSE:
		break;

	default:
		return EXIT_FAILURE;
	}

	if (needForRecursiveCall){
		return EXIT_NEED_RECURSIVE_CALL;
	}

	return EXIT_SUCCESS;
}


int flSharedTermEvaluateIfElse(FLSharedTerm * ifElseTerm, FLEnvironment * env)
{
	/*
	 * TODO : choose better if else evaluation strategy ?
	 */

	int ret;
	int needForRecursiveCall = 0;

	ret = flSharedTermEvaluateOneStep(ifElseTerm->ifElse.condition, env);

	if (ret == EXIT_FAILURE){
		return ret;
	} else if (ret == EXIT_NEED_RECURSIVE_CALL){
		needForRecursiveCall = 1;
	}

	/* Now the condition can't be a reference term */
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

		if (ret == EXIT_FAILURE){
			return ret;
		} else if (ret == EXIT_NEED_RECURSIVE_CALL){
			needForRecursiveCall = 1;
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

		if (ret == EXIT_FAILURE){
			return ret;
		} else if (ret == EXIT_NEED_RECURSIVE_CALL){
			needForRecursiveCall = 1;
		}

		ret = flSharedTermEvaluateOneStep(ifElseTerm->ifElse.elseValue, env);

		if (ret == EXIT_FAILURE){
			return ret;
		} else if (ret == EXIT_NEED_RECURSIVE_CALL){
			needForRecursiveCall = 1;
		}

		FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(ifElseTerm->ifElse.condition, env);
		FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(ifElseTerm->ifElse.thenValue, env);
		FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(ifElseTerm->ifElse.elseValue, env);

	} break;

	}

	if (needForRecursiveCall){
		return EXIT_NEED_RECURSIVE_CALL;
	}

	return EXIT_SUCCESS;
}



int flSharedTermEvaluateRef(FLSharedTerm * term, FLEnvironment * env)
{
	int ret;
	int needForRecursiveCall = 0;

	ret = flSharedTermEvaluateOneStep(term->ref, env);

	if (ret == EXIT_FAILURE){
		return ret;
	} else if (ret == EXIT_NEED_RECURSIVE_CALL){
		needForRecursiveCall = 1;
	}

	FLSharedTerm * referedTerm = flSharedTermReferedBy(term->ref);

	if (referedTerm == NULL){
		return EXIT_FAILURE;
	}

	FL_SHARED_TERM_MOVE_REFERENCE(term->ref, referedTerm, env);

	if (needForRecursiveCall){
		return EXIT_NEED_RECURSIVE_CALL;
	}

	return EXIT_SUCCESS;
}


int flSharedTermEvaluateArgRef(FLSharedTerm * term, FLEnvironment * env)
{
	FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(term->ref, env);

	return EXIT_SUCCESS;
}


int flSharedTermEvaluateOneStep(FLSharedTerm * term, FLEnvironment * env)
{
	if (term == NULL
			|| FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_EVALUATED | FL_SHARED_TERM_STATUS_BEING_EVALUATED)){
		return EXIT_SUCCESS;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_BEING_EVALUATED);

	int ret;

	switch(term->type){

	case FL_SHARED_TERM_FUN:
		ret = flSharedTermEvaluateOneStep(term->fun.body, env);

		if (ret != EXIT_FAILURE){
			FL_SHARED_TERM_REMOVE_REFERENCE_CHAIN(term->fun.body, env);
		}
		break;

	case FL_SHARED_TERM_REF:
		ret = flSharedTermEvaluateRef(term, env);
		break;

	case FL_SHARED_TERM_ARGREF:
		ret = flSharedTermEvaluateArgRef(term, env);
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
	for (size_t i = 0 ; ; i++){
		char fileName[256];
		sprintf(fileName, "in_evaluation_%zu.pdf", i);

		flSharedTermSaveToPDF(term, fileName);
		int ret = flSharedTermEvaluateOneStep(term, env);

		if (ret == EXIT_NEED_RECURSIVE_CALL){
			continue;
		}

		return ret;
	}

	return EXIT_SUCCESS;
}












#if 0

int flSharedTermHasPathToArgRef(FLSharedTerm * term, FLSharedTerm * targetedFunc, FLEnvironment * env)
{
	if (FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_SEEKING_PATH)){
		return 0;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_SEEKING_PATH);

	int output;

	switch (term->type){

	case FL_SHARED_TERM_ARGREF:
		if (flSharedTermReferedBy(term->ref) == targetedFunc){
			output = 1;
		} else {
			output = flSharedTermHasPathToArgRef(term->ref, targetedFunc, env);
		} break;

	case FL_SHARED_TERM_FUN:

		if (term == targetedFunc){
			output = 0;
		} else {
			output = flSharedTermHasPathToArgRef(term->fun.body, targetedFunc, env);
		}
		break;

	case FL_SHARED_TERM_REF:
		output = flSharedTermHasPathToArgRef(term->ref, targetedFunc, env);
		break;

	case FL_SHARED_TERM_CALL:
		output = flSharedTermHasPathToArgRef(term->call.func, targetedFunc, env);

		if (output){
			break;
		}

		output = flSharedTermHasPathToArgRef(term->call.arg, targetedFunc, env);

		break;

	case FL_SHARED_TERM_IF_ELSE:
		output = flSharedTermHasPathToArgRef(term->ifElse.condition, targetedFunc, env);

		if (output){
			break;
		}

		output = flSharedTermHasPathToArgRef(term->ifElse.thenValue, targetedFunc, env);

		if (output){
			break;
		}

		output = flSharedTermHasPathToArgRef(term->ifElse.elseValue, targetedFunc, env);

		break;

	default:
		output = 0;
		break;
	}

	FL_FLAG_UNSET(term->status, FL_SHARED_TERM_STATUS_SEEKING_PATH);

	return output;
}



void flTermAddEmptyCopyIfHasPath(FLSharedTerm * const term, FLSharedTerm * targetedFunc, FLEnvironment * env)
{
	if (FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_ADDING_PATH)){
		return;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_ADDING_PATH);

	if (flSharedTermHasPathToArgRef(term, targetedFunc, env)){
		//FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_HAS_PATH);
		term->copy = flSharedTermNewRef(NULL, env);
		term->copy->nbReferences++;
	}

	switch (term->type){

	case FL_SHARED_TERM_FUN:
		flTermAddEmptyCopyIfHasPath(term->fun.body, targetedFunc, env);
		break;

	case FL_SHARED_TERM_REF:
	case FL_SHARED_TERM_ARGREF:
		flTermAddEmptyCopyIfHasPath(term->ref, targetedFunc, env);
		break;

	case FL_SHARED_TERM_CALL:
		flTermAddEmptyCopyIfHasPath(term->call.func, targetedFunc, env);
		flTermAddEmptyCopyIfHasPath(term->call.arg, targetedFunc, env);
		break;

	case FL_SHARED_TERM_IF_ELSE:
		flTermAddEmptyCopyIfHasPath(term->ifElse.condition, targetedFunc, env);
		flTermAddEmptyCopyIfHasPath(term->ifElse.thenValue, targetedFunc, env);
		flTermAddEmptyCopyIfHasPath(term->ifElse.elseValue, targetedFunc, env);
		break;

	default:
		break;
	}

	FL_FLAG_UNSET(term->status, FL_SHARED_TERM_STATUS_ADDING_PATH);
}




void flTermLinkCopies(FLSharedTerm * const term, FLSharedTerm * targetedFunc, FLSharedTerm * funcArg, FLEnvironment * env)
{
	if (FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED)){
		return;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED);

	switch (term->type){

	case FL_SHARED_TERM_FUN:
	{
		flTermLinkCopies(term->fun.body, targetedFunc, funcArg, env);

		if (term->copy == NULL){
			break;
		}

		FLSharedTerm * body = (term->fun.body->copy) ? (term->fun.body->copy) : (term->fun.body);

		FL_SHARED_TERM_SET_REFERENCE(term->copy->ref, flSharedTermNewFun(body, env));
	} break;

	case FL_SHARED_TERM_REF:
	{
		flTermLinkCopies(term->ref, targetedFunc, funcArg, env);

		if (term->copy == NULL){
			break;
		}

		FLSharedTerm * ref = (term->ref->copy) ? (term->ref->copy)  : (term->ref);
		FL_SHARED_TERM_SET_REFERENCE(term->copy->ref, ref);

	} break;

	case FL_SHARED_TERM_ARGREF:
	{
		flTermLinkCopies(term->ref, targetedFunc, funcArg, env);

		if (term->copy == NULL){
			break;
		}

		if (term->ref == targetedFunc){
			FL_SHARED_TERM_SET_REFERENCE(term->copy->ref, funcArg);
		} else {
			FLSharedTerm * ref = (term->ref->copy) ? (term->ref->copy)  : (term->ref);
			FL_SHARED_TERM_SET_REFERENCE(term->copy->ref, flSharedTermNewArgRef(ref, env));
		}
	} break;

	case FL_SHARED_TERM_CALL:
	{
		flTermLinkCopies(term->call.func, targetedFunc, funcArg, env);
		flTermLinkCopies(term->call.arg, targetedFunc, funcArg, env);

		if (term->copy == NULL){
			break;
		}

		FLSharedTerm * func = (term->call.func->copy) ? (term->call.func->copy)  : (term->call.func);
		FLSharedTerm * arg = (term->call.arg->copy) ? (term->call.arg->copy)  : (term->call.arg);

		FL_SHARED_TERM_SET_REFERENCE(term->copy->ref, flSharedTermNewCall(func, arg, term->call.isACallByName, env));
	} break;

	case FL_SHARED_TERM_IF_ELSE:
	{
		flTermLinkCopies(term->ifElse.condition, targetedFunc, funcArg, env);
		flTermLinkCopies(term->ifElse.thenValue, targetedFunc, funcArg, env);
		flTermLinkCopies(term->ifElse.elseValue, targetedFunc, funcArg, env);

		if (term->copy == NULL){
			break;
		}

		FLSharedTerm * condition = (term->ifElse.condition->copy) ? (term->ifElse.condition->copy)  : (term->ifElse.condition);
		FLSharedTerm * thenValue = (term->ifElse.thenValue->copy) ? (term->ifElse.thenValue->copy)  : (term->ifElse.thenValue);
		FLSharedTerm * elseValue = (term->ifElse.elseValue->copy) ? (term->ifElse.elseValue->copy)  : (term->ifElse.elseValue);

		FL_SHARED_TERM_SET_REFERENCE(term->copy->ref, flSharedTermNewIfElse(condition, thenValue, elseValue, env));
	} break;

	default:
		break;
	}

	FL_FLAG_UNSET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED);
}


void flSharedTermCopyIfHasPathToArgRef(FLSharedTerm * term, FLSharedTerm * targetedFunc, FLSharedTerm * funcArg, FLEnvironment * env)
{
	if (term->copy != NULL
			|| FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED)){
		return;
	}

	int hasAPath = flSharedTermHasPathToArgRef(term, targetedFunc, env);

	if (!hasAPath){
		return;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_BEING_COPIED);

	switch (term->type){

	case FL_SHARED_TERM_FUN:
	{
		flSharedTermCopyIfHasPathToArgRef(term->fun.body, targetedFunc, funcArg, env);
		FLSharedTerm * body = (term->fun.body->copy) ? (term->fun.body->copy) : (term->fun.body);
		term->copy = flSharedTermNewFun(body, env);
	} break;

	case FL_SHARED_TERM_REF:
	{
		flSharedTermCopyIfHasPathToArgRef(term->ref, targetedFunc, funcArg, env);
		FLSharedTerm * ref = (term->ref->copy) ? (term->ref->copy)  : (term->ref);
		term->copy = flSharedTermNewRef(ref, env);
	} break;

	case FL_SHARED_TERM_ARGREF:
	{
		if (term->ref == targetedFunc){
			term->copy = flSharedTermNewRef(funcArg, env);
		} else {
			flSharedTermCopyIfHasPathToArgRef(term->ref, targetedFunc, funcArg, env);
			FLSharedTerm * ref = (term->ref->copy) ? (term->ref->copy)  : (term->ref);
			term->copy = flSharedTermNewArgRef(ref, env);
		}
	} break;

	case FL_SHARED_TERM_CALL:
	{
		flSharedTermCopyIfHasPathToArgRef(term->call.func, targetedFunc, funcArg, env);
		flSharedTermCopyIfHasPathToArgRef(term->call.arg, targetedFunc, funcArg, env);
		FLSharedTerm * func = (term->call.func->copy) ? (term->call.func->copy)  : (term->call.func);
		FLSharedTerm * arg = (term->call.arg->copy) ? (term->call.arg->copy)  : (term->call.arg);
		term->copy = flSharedTermNewCall(func, arg, term->call.isACallByName, env);
	} break;

	case FL_SHARED_TERM_IF_ELSE:
	{
		flSharedTermCopyIfHasPathToArgRef(term->ifElse.condition, targetedFunc, funcArg, env);
		flSharedTermCopyIfHasPathToArgRef(term->ifElse.thenValue, targetedFunc, funcArg, env);
		flSharedTermCopyIfHasPathToArgRef(term->ifElse.elseValue, targetedFunc, funcArg, env);
		FLSharedTerm * condition = (term->ifElse.condition->copy) ? (term->ifElse.condition->copy)  : (term->ifElse.condition);
		FLSharedTerm * thenValue = (term->ifElse.thenValue->copy) ? (term->ifElse.thenValue->copy)  : (term->ifElse.thenValue);
		FLSharedTerm * elseValue = (term->ifElse.elseValue->copy) ? (term->ifElse.elseValue->copy)  : (term->ifElse.elseValue);
		term->copy = flSharedTermNewIfElse(condition, thenValue, elseValue, env);
	} break;

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

#endif



#endif








