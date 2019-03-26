/*
 * funl__parse_tree_new.c
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLParseTree * flParseTreeNew()
{
	FLParseTree * output;
	FL_SIMPLE_ALLOC(output, 1);

	return output;
}


FLParseTree * flParseTreeNewVar(const char * name)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_VAR;
	output->data.var = name;

	return output;
}


FLParseTree * flParseTreeNewCall(FLParseTree * func, size_t nbArguments, FLParseTree ** args)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_CALL;
	output->data.call.function = func;
	output->data.call.nbArguments = nbArguments;

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


FLParseTree * flParseTreeNewDef(const char * name, FLParseTree * expr)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_DEF;
	output->data.def.variableName = name;
	output->data.def.expression = expr;

	return output;
}

FLParseTree * flParseTreeNewLet(const char * variable, FLParseTree * affectExpr, FLParseTree * followingExpr)
{
	FLParseTree * output = flParseTreeNew();

	output->type = FL_PARSE_TREE_LET;
	output->data.let.variable = variable;
	output->data.let.affect = affectExpr;
	output->data.let.following = followingExpr;

	return output;
}


void flParseTreeFree(FLParseTree * tree)
{
	/*
	 * TODO
	 */
	return;
}


