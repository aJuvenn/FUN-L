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
	FL_PARSE_TREE_DEF,
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

	const char * functionName;

	size_t nbParameters;
	const char ** parameters;

	FLParseTree * body;

} FLParseTreeFunData;



typedef struct {

	const char * variableName;
	FLParseTree * expression;

} FLParseTreeDefData;


typedef struct {

	const char * variable;
	FLParseTree * affect;

	FLParseTree * following;

} FLParseTreeLetData;






struct FLParseTree {

	FLParseTreeType type;

	union {

		const char * var;
		FLParseTreeCallData call;
		FLParseTreeFunData fun;
		FLParseTreeDefData def;
		FLParseTreeLetData let;

	} data;

};





#define FL_PARSER_MAXIMUM_ARITY 64


FLParseTree * flParseTree(FLStreamCursor cursor);

void flParseTreePrint(const FLParseTree * const tree);

void flParseTreeFree(FLParseTree * tree);



















#endif /* PARSE_TREE_FUNL__PARSE_TREE_H_ */
