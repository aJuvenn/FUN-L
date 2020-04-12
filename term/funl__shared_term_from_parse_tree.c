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

	for (size_t i = 0 ; i < nbParameters ; i++){

		int ret = flEnvironmentPushVar(env, fun->data.fun.parameters[i], NULL);

		if (ret != EXIT_SUCCESS){
			return NULL;
		}
	}

	FLSharedTerm * body = flSharedTermFromParseTree(fun->data.fun.body, env);

	if (body == NULL){
		return NULL;
	}

	flEnvironmentPopVar(env, nbParameters);

	FLSharedTerm * output = body;

	for (size_t i = 0 ; i < nbParameters ; i++){

		output = flSharedTermNewFun(output, env);

		if (output == NULL){
			return NULL;
		}
	}

	return output;
}


FLSharedTerm * flSharedTermFromParseTreeLet(const FLParseTree * const let, FLEnvironment * const env)
{
	int isRecursive = let->data.let.recursive;
	int ret;

	if (isRecursive){
		ret = flEnvironmentPushVar(env, let->data.let.variable, NULL);
		if (ret != EXIT_SUCCESS){
			return NULL;
		}
	}

	FLSharedTerm * affect = flSharedTermFromParseTree(let->data.let.affect, env);

	if (affect == NULL){
		return NULL;
	}

	if (let->data.let.following == NULL){
		if (isRecursive){
			flEnvironmentPopVar(env, 1);
		}
		return flSharedTermNewLet(affect, NULL, isRecursive, env);
	}

	if (!isRecursive){
		ret = flEnvironmentPushVar(env, let->data.let.variable, NULL);
		if (ret != EXIT_SUCCESS){
			return NULL;
		}
	}

	FLSharedTerm * following = flSharedTermFromParseTree(let->data.let.following, env);

	flEnvironmentPopVar(env, 1);

	if (following == NULL){
		return NULL;
	}

	return flSharedTermNewLet(affect, following, isRecursive, env);
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


FLSharedTerm * flSharedTermFromParseTreeVar(const FLParseTree * const tree, FLEnvironment * const env)
{
	size_t id;
	int isAGlobalVar;
	flEnvironmentVarId(env, tree->data.var, &id, &isAGlobalVar);

	if (id == (size_t) -1){
		return NULL;
	}

	return flSharedTermNewVar(id, isAGlobalVar, env);
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
		return flSharedTermFromParseTreeVar(tree, env);

	case FL_PARSE_TREE_LET:
		return flSharedTermFromParseTreeLet(tree, env);

	case FL_PARSE_TREE_IF_ELSE:
		return flSharedTermFromParseTreeIfElse(tree, env);

	default:
		break;
	}

	return NULL;
}
