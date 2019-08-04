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

	FL_PARSE_TREE_INVALID = 0,
	FL_PARSE_TREE_VAR,
	FL_PARSE_TREE_CALL,
	FL_PARSE_TREE_FUN,
	FL_PARSE_TREE_LET

} FLParseTreeType;

struct FLParseTree;
typedef struct FLParseTree FLParseTree;


typedef struct {

	FLParseTree * function;

	size_t nbArguments;
	FLParseTree ** arguments;

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






struct FLParseTree {

	FLParseTreeType type;

	union {

		char * var;
		FLParseTreeCallData call;
		FLParseTreeFunData fun;
		FLParseTreeLetData let;

	} data;

};





#define FL_PARSER_MAXIMUM_ARITY 64


FLParseTree * flParseTree(FLStreamCursor cursor);


FLParseTree * flParseTreeRecursive(FLStreamCursor * const cursor);



void flParseTreePrint(const FLParseTree * const tree);

void flParseTreeFree(FLParseTree * tree);







FLParseTree * flParseTreeNewVar(char * name);
FLParseTree * flParseTreeNewCall(FLParseTree * func, size_t nbArguments, FLParseTree ** args);
FLParseTree * flParseTreeNewFun(size_t nbParameters, char ** params, FLParseTree * body);
FLParseTree * flParseTreeNewLet(char * variable, FLParseTree * affectExpr, FLParseTree * followingExpr, int recursive);






#endif /* PARSE_TREE_FUNL__PARSE_TREE_H_ */
