/*
 * funl__parse_tree_print.c
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */


#include "../funl__include.h"



void flParseTreePrint(const FLParseTree * const tree);


void flParseTreePrintVar(const FLParseTree * const tree)
{
	printf("%s", tree->data.var);
}


void flParseTreePrintCall(const FLParseTree * const tree)
{
	printf("(");
	flParseTreePrint(tree->data.call.function);

	for (size_t i = 0 ; i < tree->data.call.nbArguments ; i++){
		printf(" ");
		flParseTreePrint(tree->data.call.arguments[i]);
	}

	printf(")");
}



void flParseTreePrintFun(const FLParseTree * const tree)
{
	printf("(fun (");

	for (size_t i = 0 ; i < tree->data.fun.nbParameters ; i++){

		if (i != 0){
			printf(" ");
		}

		printf("%s", tree->data.fun.parameters[i]);
	}

	printf(") ");
	flParseTreePrint(tree->data.fun.body);
	printf(")");
}



void flParseTreePrintDef(const FLParseTree * const tree)
{
	printf("(def ");
	printf("%s ", tree->data.def.variableName);
	flParseTreePrint(tree->data.def.expression);
	printf(")");
}

void flParseTreePrintLet(const FLParseTree * const tree)
{
	printf("(let %s ", tree->data.let.variable);
	flParseTreePrint(tree->data.let.affect);
	printf(" ");
	flParseTreePrint(tree->data.let.following);
	printf(")");
}


void flParseTreePrintInvalid(const FLParseTree * const tree)
{
	printf("<INVALID>");
}



void flParseTreePrint(const FLParseTree * const tree)
{
	if (tree == NULL){
		flParseTreePrintInvalid(tree);
		return;
	}

	switch (tree->type){

	case FL_PARSE_TREE_VAR:
		flParseTreePrintVar(tree);
		return;

	case FL_PARSE_TREE_CALL:
		flParseTreePrintCall(tree);
		return;

	case FL_PARSE_TREE_DEF:
		flParseTreePrintDef(tree);
		return;

	case FL_PARSE_TREE_FUN:
		flParseTreePrintFun(tree);
		return;

	case FL_PARSE_TREE_LET:
		flParseTreePrintLet(tree);
		return;

	case FL_PARSE_TREE_INVALID:
		flParseTreePrintInvalid(tree);
		return;

	default:
		return;
	}
}

