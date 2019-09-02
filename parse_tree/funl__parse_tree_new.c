/*
 * funl__parse_tree_new.c
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"


/*
 * Allocates and returns a new empty tree.
 */
FLParseTree * flParseTreeNew()
{
	FLParseTree * output;
	FL_SIMPLE_ALLOC(output, 1);

	return output;
}


FLParseTree * flParseTreeNewVar(char * name)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_VAR;
	output->data.var = name;

	return output;
}


FLParseTree * flParseTreeNewEndOfFile()
{
	FLParseTree * output = flParseTreeNew();
	output->type = FL_PARSE_TREE_END_OF_FILE;
	return output;
}




FLParseTree * flParseTreeNewInteger(long long int integer)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_INTEGER;
	output->data.integer = integer;

	return output;
}


FLParseTree * flParseTreeNewCall(FLParseTree * func, size_t nbArguments, FLParseTree * args[], int isACallByName)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_CALL;
	output->data.call.function = func;
	output->data.call.nbArguments = nbArguments;
	output->data.call.isACallByName = isACallByName;

	FL_SIMPLE_ALLOC(output->data.call.arguments, nbArguments);
	memcpy(output->data.call.arguments, args, nbArguments * sizeof(FLParseTree *));

	return output;
}


FLParseTree * flParseTreeNewFun(size_t nbParameters, char ** params, FLParseTree * body)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_FUN;
	output->data.fun.nbParameters = nbParameters;

	FL_SIMPLE_ALLOC(output->data.fun.parameters, nbParameters);
	memcpy(output->data.fun.parameters, params, nbParameters * sizeof(char *));

	output->data.fun.body = body;

	return output;
}



FLParseTree * flParseTreeNewLet(char * variable, FLParseTree * affectExpr, FLParseTree * followingExpr, int recursive)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_LET;
	output->data.let.variable = variable;
	output->data.let.affect = affectExpr;
	output->data.let.following = followingExpr;
	output->data.let.recursive = recursive;

	return output;
}


void flParseTreeFree(FLParseTree * tree)
{

	switch (tree->type){


	case FL_PARSE_TREE_VAR:
		free(tree->data.var);
		break;


	case FL_PARSE_TREE_CALL:

		flParseTreeFree(tree->data.call.function);

		for (size_t i = 0 ; i < tree->data.call.nbArguments ; i++){
			flParseTreeFree(tree->data.call.arguments[i]);
		}

		free(tree->data.call.arguments);

		break;


	case FL_PARSE_TREE_FUN:

		flParseTreeFree(tree->data.fun.body);

		for (size_t i = 0 ; i < tree->data.fun.nbParameters ; i++){
			free(tree->data.fun.parameters[i]);
		}

		free(tree->data.fun.parameters);

		break;


	case FL_PARSE_TREE_LET:

		flParseTreeFree(tree->data.let.affect);

		if (tree->data.let.following != NULL){
			flParseTreeFree(tree->data.let.following);
		}

		free(tree->data.let.variable);

		break;


	default:
		break;
	}

	free(tree);
}


