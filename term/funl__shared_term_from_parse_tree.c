/*
 * funl__term_build_from_parse_tree.c
 *
 *  Created on: 31 juil. 2019
 *      Author: ajuvenn
 */



#include "../funl__include.h"


#if 0
FLSharedTerm * flSharedTermFromParseTreeOpCall(const FLParseTree * const call, FLEnvironment * const env)
{
	size_t nbArguments = call->data.call.nbArguments;
	FlTokenOperatorData op = call->data.call.function->data.op;

	if (nbArguments == 0){
		/*
		 * TODO : number of arguments can be checked depending on the operator
		 */
		return NULL;
	}

	FLSharedTerm ** arguments;
	FL_SIMPLE_ALLOC(arguments, nbArguments);

	/*
	 * TODO : add in freeterm arguments free
	 */

	size_t i = 0;

	for (i = 0 ; i < nbArguments ; i++){

		arguments[i] = flSharedTermFromParseTree(call->data.call.arguments[i], env);

		if (arguments[i] == NULL){
			goto INVALID;
		}
	}

	FLSharedTerm * output =  flSharedTermNewOpCall(op, nbArguments, arguments, env);

	if (output == NULL){
		goto INVALID;
	}

	return output;


	INVALID:{
		for (size_t j = 0 ; j < i ; j++){
			flSharedTermFree(arguments[j], env);
		}

		free(arguments);

		return NULL;
	}
}
#endif



FLSharedTerm * flSharedTermFromParseTreeCall(const FLParseTree * const call, FLEnvironment * const env)
{

	if (call->data.call.function->type == FL_PARSE_TREE_OP){
		/*
		 * TODO return flSharedTermFromParseTreeOpCall(call, env);
		 */
		return NULL;
	}

	FLSharedTerm * output = flSharedTermFromParseTree(call->data.call.function, env);

	if (output == NULL){
		return NULL;
	}

	for (size_t i = 0 ; i < call->data.call.nbArguments ; i++){

		FLSharedTerm * arg = flSharedTermFromParseTree(call->data.call.arguments[i], env);

		if (arg == NULL){
			flSharedTermFree(output, env);
			return NULL;
		}

		output = flSharedTermNewCall(output, arg, call->data.call.isACallByName, env);
	}

	return output;
}



FLSharedTerm * flSharedTermFromParseTreeFun(const FLParseTree * const fun, FLEnvironment * const env)
{
	size_t nbParameters = fun->data.fun.nbParameters;

	FLSharedTerm * output = NULL;

	for (size_t i = 0 ; i < nbParameters ; i++){

		output = flSharedTermNewFun(output, env);

		if (output == NULL){
			/* TODO */
			return NULL;
		}
	}

	FLSharedTerm * funChainCursor = output;

	for (size_t i = 0 ; i < nbParameters ; i++){

		FLSharedTerm * argRef = flSharedTermNewArgRef(funChainCursor, env);

		if (argRef == NULL){
			/* TODO */
			return NULL;
		}

		int ret = flEnvironmentPushLocalVar(env, fun->data.fun.parameters[i], argRef);

		if (ret != EXIT_SUCCESS){
			/* TODO */
			return NULL;
		}

		if (funChainCursor->fun.body != NULL){
			/* This way the cursor is the last of the chain after the loop */
			funChainCursor = funChainCursor->fun.body;
		}
	}

	FLSharedTerm * funBody = flSharedTermFromParseTree(fun->data.fun.body, env);

	if (funBody == NULL){
		/* TODO */
		return NULL;
	}

	flEnvironmentPopLocalVar(env, nbParameters);

	FL_SHARED_TERM_SET_REFERENCE(funChainCursor->fun.body, funBody);

	return output;
}


FLSharedTerm * flSharedTermFromParseTreeLet(const FLParseTree * const let, FLEnvironment * const env)
{
	/*
	 * TODO : if let has not 'in'
	 * TODO : errors
	 */
	int ret;

	FLSharedTerm * affect;
	{
		FLSharedTerm * affectRef;

		if (let->data.let.recursive){

			affectRef = flSharedTermNewRef(NULL, env);

			if (affectRef == NULL){
				return NULL;
			}

			ret = flEnvironmentPushLocalVar(env, let->data.let.variable, affectRef);
			if (ret != EXIT_SUCCESS){
				return NULL;
			}
		}

		affect = flSharedTermFromParseTree(let->data.let.affect, env);

		if (affect == NULL){
			return NULL;
		}

		if (let->data.let.recursive){
			FL_SHARED_TERM_SET_REFERENCE(affectRef->ref, affect);
			affect = affectRef;
		} else {
			ret = flEnvironmentPushLocalVar(env, let->data.let.variable, affect);
			if (ret != EXIT_SUCCESS){
				return NULL;
			}
		}
	}

	if (let->data.let.following == NULL){
		return NULL;
	}

	FLSharedTerm * output = flSharedTermFromParseTree(let->data.let.following, env);

	if (output == NULL){
		return NULL;
	}

	flEnvironmentPopLocalVar(env, 1);

	return output;
}



FLSharedTerm * flSharedTermFromParseTreeIfElse(const FLParseTree * const tree, FLEnvironment * const env)
{
	FLSharedTerm * condition = flSharedTermFromParseTree(tree->data.ifElse.condition, env);

	if (condition == NULL){
		return NULL;
	}

	FLSharedTerm * thenValue = flSharedTermFromParseTree(tree->data.ifElse.thenValue, env);

	if (thenValue == NULL){
		flSharedTermFree(condition, env);
		return NULL;
	}

	FLSharedTerm * elseValue = flSharedTermFromParseTree(tree->data.ifElse.elseValue, env);

	if (elseValue == NULL){
		flSharedTermFree(condition, env);
		flSharedTermFree(thenValue, env);
		return NULL;
	}

	return flSharedTermNewIfElse(condition, thenValue, elseValue, env);
}



FLSharedTerm * flSharedTermFromParseTree(const FLParseTree * const tree, FLEnvironment * const env)
{
	if (tree == NULL){
		return NULL;
	}

	switch (tree->type){

	case FL_PARSE_TREE_INTEGER:
		return flSharedTermNewInteger(tree->data.integer, env);

	case FL_PARSE_TREE_CALL:
		return flSharedTermFromParseTreeCall(tree, env);

	case FL_PARSE_TREE_FUN:
		return flSharedTermFromParseTreeFun(tree, env);

	case FL_PARSE_TREE_VAR:
		return flEnvironmentSharedTermFromVarName(env, tree->data.var);

	case FL_PARSE_TREE_LET:
		return flSharedTermFromParseTreeLet(tree, env);

	case FL_PARSE_TREE_IF_ELSE:
		return flSharedTermFromParseTreeIfElse(tree, env);

	default:
		break;
	}

	return NULL;
}
