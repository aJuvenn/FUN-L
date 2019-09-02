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


void flParseTreePrintInteger(const FLParseTree * const tree)
{
	printf("%lld", tree->data.integer);
}


void flParseTreePrintCall(const FLParseTree * const tree)
{
	printf(tree->data.call.isACallByName ? "[" : "(");
	flParseTreePrint(tree->data.call.function);

	for (size_t i = 0 ; i < tree->data.call.nbArguments ; i++){
		printf(" ");
		flParseTreePrint(tree->data.call.arguments[i]);
	}

	printf(tree->data.call.isACallByName ? "]" : ")");
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




void flParseTreePrintLet(const FLParseTree * const tree)
{
	printf("(let %s %s ", tree->data.let.recursive ? "rec" : "", tree->data.let.variable);
	flParseTreePrint(tree->data.let.affect);


	if (tree->data.let.following != NULL){
		printf(" in ");
		flParseTreePrint(tree->data.let.following);
	} else {
		printf(";");
	}

	printf(")");
}



void flParseTreePrintEndOfFile(const FLParseTree * const tree)
{
	printf("<END OF FILE>");
}



void flParseTreePrint(const FLParseTree * const tree)
{
	if (tree == NULL){
		printf("<NULL>");
		return;
	}

	switch (tree->type){

	case FL_PARSE_TREE_VAR:
		flParseTreePrintVar(tree);
		return;

	case FL_PARSE_TREE_INTEGER:
		flParseTreePrintInteger(tree);
		return;

	case FL_PARSE_TREE_CALL:
		flParseTreePrintCall(tree);
		return;

	case FL_PARSE_TREE_FUN:
		flParseTreePrintFun(tree);
		return;

	case FL_PARSE_TREE_LET:
		flParseTreePrintLet(tree);
		return;

	case FL_PARSE_TREE_END_OF_FILE:
		flParseTreePrintEndOfFile(tree);
		return;

	}
}

