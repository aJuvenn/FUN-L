/*
 * funl__parse_tree.h
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */

#ifndef PARSE_TREE_FUNL__PARSE_TREE_H_
#define PARSE_TREE_FUNL__PARSE_TREE_H_

#include "../funl__include.h"


typedef enum {

	FL_PARSE_TREE_END_OF_FILE = 0,
	FL_PARSE_TREE_VAR,
	FL_PARSE_TREE_INTEGER,
	FL_PARSE_TREE_CALL,
	FL_PARSE_TREE_FUN,
	FL_PARSE_TREE_LET,
	FL_PARSE_TREE_OP,
	FL_PARSE_TREE_IF_ELSE

} FLParseTreeType;

struct FLParseTree;
typedef struct FLParseTree FLParseTree;


typedef struct {

	FLParseTree * function;

	size_t nbArguments;
	FLParseTree ** arguments;

	int isACallByName;

} FLParseTreeCallData;


typedef struct {

	size_t nbParameters;
	char ** parameters;

	FLParseTree * body;

} FLParseTreeFunData;



typedef struct {

	char * variable;
	FLParseTree * affect;

	FLParseTree * following;

	int recursive;

} FLParseTreeLetData;



typedef struct {

	FLParseTree * condition;
	FLParseTree * thenValue;
	FLParseTree * elseValue;

} FLParseTreeIfElseData;






struct FLParseTree {

	FLParseTreeType type;

	union {

		char * var;
		long long int integer;
		FLParseTreeCallData call;
		FLParseTreeFunData fun;
		FLParseTreeLetData let;
		FlTokenOperatorData op;
		FLParseTreeIfElseData ifElse;

	} data;

};





#define FL_PARSER_MAXIMUM_ARITY 64


FLParseTree * flParseTree(FLStreamCursor cursor);


FLParseTree * flParseTreeRecursive(FLStreamCursor * const cursor);



void flParseTreePrint(const FLParseTree * const tree);

void flParseTreeFree(FLParseTree * tree);







FLParseTree * flParseTreeNewVar(char * name);
FLParseTree * flParseTreeNewOp(FlTokenOperatorData const op);
FLParseTree * flParseTreeNewInteger(long long int integer);
FLParseTree * flParseTreeNewCall(FLParseTree * func, size_t nbArguments, FLParseTree ** args, int isACallByName);
FLParseTree * flParseTreeNewFun(size_t nbParameters, char ** params, FLParseTree * body);
FLParseTree * flParseTreeNewLet(char * variable, FLParseTree * affectExpr, FLParseTree * followingExpr, int recursive);

FLParseTree * flParseTreeNewIfElse(FLParseTree * condition, FLParseTree * thenValue, FLParseTree * elseValue);
FLParseTree * flParseTreeNewEndOfFile();


int flParseTreeIsAGlobalDefinition(const FLParseTree * const tree);


#endif /* PARSE_TREE_FUNL__PARSE_TREE_H_ */
