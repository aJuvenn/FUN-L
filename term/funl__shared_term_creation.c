/*
 * funl__shared_term_creation.c
 *
 *  Created on: 11 avr. 2020
 *      Author: ajuvenn
 */

#include "../funl__include.h"

/*
 * TODO : include env
 */
static inline FLSharedTerm * flSharedTermNew(FLEnvironment * const env)
{
	FLSharedTerm * output = malloc(sizeof(FLSharedTerm));

	if (output == NULL){
		return NULL;
	}

	output->nbReferences = 0;
	output->status = FL_SHARED_TERM_STATUS_NONE;
	output->copy = NULL;

	return output;
}


FLSharedTerm * flSharedTermNewFun(FLSharedTerm * funBody, FLEnvironment * const env)
{
	FLSharedTerm * output = flSharedTermNew(env);
	output->type = FL_SHARED_TERM_FUN;

	FL_SHARED_TERM_SET_REFERENCE(output->fun.body, funBody);

	return output;
}


FLSharedTerm * flSharedTermNewRef(FLSharedTerm * pointedTerm, FLEnvironment * const env)
{
	FLSharedTerm * output = flSharedTermNew(env);
	output->type = FL_SHARED_TERM_REF;

	FL_SHARED_TERM_SET_REFERENCE(output->ref, pointedTerm);

	return output;
}


FLSharedTerm * flSharedTermNewArgRef(FLSharedTerm * pointedFun, FLEnvironment * const env)
{
	FLSharedTerm * output = flSharedTermNew(env);
	output->type = FL_SHARED_TERM_ARGREF;
	output->status = FL_SHARED_TERM_STATUS_EVALUATED;

	/* Arg ref is not counted as a reference for garbage collector */
	output->ref = pointedFun;

	return output;
}


FLSharedTerm * flSharedTermNewInteger(long long int integer, FLEnvironment * const env)
{
	FLSharedTerm * output = flSharedTermNew(env);

	output->type = FL_SHARED_TERM_INTEGER;
	output->status = FL_SHARED_TERM_STATUS_EVALUATED;
	output->integer = integer;

	return output;
}


FLSharedTerm * flSharedTermNewCall(FLSharedTerm * func, FLSharedTerm * arg, int isACallByName, FLEnvironment * const env)
{
	FLSharedTerm * output = flSharedTermNew(env);
	output->type = FL_SHARED_TERM_CALL;

	output->call.isACallByName = isACallByName;

	FL_SHARED_TERM_SET_REFERENCE(output->call.func, func);
	FL_SHARED_TERM_SET_REFERENCE(output->call.arg, arg);

	return output;
}



FLSharedTerm * flSharedTermNewIfElse(FLSharedTerm * condition, FLSharedTerm * thenValue, FLSharedTerm * elseValue, FLEnvironment * const env)
{
	FLSharedTerm * output = flSharedTermNew(env);
	output->type = FL_SHARED_TERM_IF_ELSE;

	FL_SHARED_TERM_SET_REFERENCE(output->ifElse.condition, condition);
	FL_SHARED_TERM_SET_REFERENCE(output->ifElse.thenValue, thenValue);
	FL_SHARED_TERM_SET_REFERENCE(output->ifElse.elseValue, elseValue);

	return output;
}



FLSharedTerm * flSharedTermReferedBy(FLSharedTerm * refTerm)
{
	/*
	 * TODO : avoid infinite loop
	 */
	FLSharedTerm * output = refTerm;

	while (output->type == FL_SHARED_TERM_REF){
		output = output->ref;
	}

	return output;
}


void flSharedTermFree(FLSharedTerm * term, FLEnvironment * const env)
{
	if (FL_FLAG_IS_SET(term->status, FL_SHARED_TERM_STATUS_BEING_DESTROYED)){
		return;
	}

	FL_FLAG_SET(term->status, FL_SHARED_TERM_STATUS_BEING_DESTROYED);

	switch (term->type)
	{
	case FL_SHARED_TERM_FUN:
		FL_SHARED_TERM_REMOVE_REFERENCE(term->fun.body, env);
		break;

	case FL_SHARED_TERM_REF:
		FL_SHARED_TERM_REMOVE_REFERENCE(term->ref, env);
		break;

	case FL_SHARED_TERM_CALL:
		FL_SHARED_TERM_REMOVE_REFERENCE(term->call.func, env);
		FL_SHARED_TERM_REMOVE_REFERENCE(term->call.arg, env);
		break;

	case FL_SHARED_TERM_IF_ELSE:
		FL_SHARED_TERM_REMOVE_REFERENCE(term->ifElse.condition, env);
		FL_SHARED_TERM_REMOVE_REFERENCE(term->ifElse.thenValue, env);
		FL_SHARED_TERM_REMOVE_REFERENCE(term->ifElse.elseValue, env);
		break;

	default:
		break;
	}

	FL_FLAG_UNSET(term->status, FL_SHARED_TERM_STATUS_BEING_DESTROYED);

	/*
	 * TODO : env
	 */
	(void) env;
	free(term);
}


