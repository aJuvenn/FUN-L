/*
 * funl__term_build_from_parse_tree.c
 *
 *  Created on: 31 juil. 2019
 *      Author: ajuvenn
 */



#include "../funl__include.h"






FLTerm * flTermFromParseTreeVar(const FLParseTree * const var, FLEnvironment * const env)
{
	FLTermId varId = flTermIdFromVarName(var->data.var, env);

	if (varId != (FLTermId) -1){
		return flTermNewVarId(varId, env);
	}

	varId = flGlobalIdFromName(var->data.var, env);

	if (varId == (FLTermId) -1){
		fprintf(stderr, "Unknown variable %s\n", var->data.var);
		return NULL;
	}

	return flTermNewGlobalVarId(varId, env);
}




FLTerm * flTermFromParseTreeCall(const FLParseTree * const call, FLEnvironment * const env)
{
	FLTerm * output = flTermFromParseTree(call->data.call.function, env);

	if (output == NULL){
		return NULL;
	}

	for (size_t i = 0 ; i < call->data.call.nbArguments ; i++){

		FLTerm * arg = flTermFromParseTree(call->data.call.arguments[i], env);

		if (arg == NULL){
			flTermFree(output, env);
			return NULL;
		}

		output = flTermNewCall(output, arg, call->data.call.isACallByName, env);
	}

	return output;
}



FLTerm * flTermFromParseTreeFun(const FLParseTree * const fun, FLEnvironment * const env)
{
	size_t nbParameters = fun->data.fun.nbParameters;

	for (size_t i = 0 ; i < nbParameters ; i++){
		env->varNameStack[env->varNameStackSize++] = fun->data.fun.parameters[i];
	}

	FLTerm * funBody = flTermFromParseTree(fun->data.fun.body, env);

	env->varNameStackSize -= nbParameters;

	if (funBody == NULL){
		return NULL;
	}

	FLTerm * output = funBody;

	for (size_t i = 0 ; i < nbParameters ; i++){
		output = flTermNewFun(output, env);
	}

	return output;
}


FLTerm * flTermFromParseTreeLet(const FLParseTree * const let, FLEnvironment * const env)
{
	/*
	 * TODO : if let->data.let.recursive
	 * TODO : if let has not 'in'
	*/

	FLTerm * affect = flTermFromParseTree(let->data.let.affect, env);

	if (affect == NULL){
		fprintf(stderr, "flTermFromParseTreeLet : affect flTermFromParseTree failed\n");
		return NULL;
	}

	env->varNameStack[env->varNameStackSize++] = let->data.let.variable;

	FLTerm * following = flTermFromParseTree(let->data.let.following, env);

	env->varNameStackSize--;

	if (following == NULL){
		flTermFree(affect, env);
		fprintf(stderr, "flTermFromParseTreeLet : following flTermFromParseTree failed\n");
		return NULL;
	}

	return flTermNewLet(affect, following, env);
}



FLTerm * flTermFromParseTree(const FLParseTree * const tree, FLEnvironment * const env)
{
	if (tree == NULL){
		return NULL;
	}

	switch (tree->type){

	case FL_PARSE_TREE_INTEGER:
		return flTermNewInteger(tree->data.integer, env);

	case FL_PARSE_TREE_CALL:
		return flTermFromParseTreeCall(tree, env);

	case FL_PARSE_TREE_FUN:
		return flTermFromParseTreeFun(tree, env);

	case FL_PARSE_TREE_VAR:
		return flTermFromParseTreeVar(tree, env);

	case FL_PARSE_TREE_LET:
		return flTermFromParseTreeLet(tree, env);

	default:
		break;
	}

	return NULL;
}
