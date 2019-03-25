/*
 * funl__parse_tree.c
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */


#include "../funl__include.h"





FLParseTree * flParseTreeNewVar(const char * name);
FLParseTree * flParseTreeNewCall(FLParseTree * func, size_t nbArguments, FLParseTree ** args);
FLParseTree * flParseTreeNewFun(size_t nbParameters, char ** params, FLParseTree * body);
FLParseTree * flParseTreeNewDef(const char * name, FLParseTree * expr);


FLParseTree * flParseTreeRecursive(FLStreamCursor const cursor, FLStreamCursor * const out_nextCursor);



FLParseTree * flParseTreeLeftBracket(FLStreamCursor const cursor, FLStreamCursor * const out_nextCursor)
{
	FLParseTree * output = flParseTreeRecursive(cursor, out_nextCursor);

	if (output == NULL){
		return NULL;
	}

	FLStreamCursor nextCursor = *out_nextCursor;
	FLStreamCursor nextNextCursor;
	size_t nbArguments = 0;
	FLParseTree * arguments[FL_PARSER_MAXIMUM_ARITY];

	for (nextCursor = *out_nextCursor ; ; nextNextCursor = nextCursor){

		FLParseTree * nextTree = flParseTreeRecursive(nextCursor, &nextNextCursor);

		if (nextTree == NULL){
			break;
		}

		if (nbArguments == FL_PARSER_MAXIMUM_ARITY){
			goto INVALID;
		}

		arguments[nbArguments++] = nextTree;
	}

	FlToken token;
	nextNextCursor = flTokenNext(nextCursor, &token);

	if (token.type != FL_TOKEN_RIGHT_BRACKET){
		flTokenFree(&token);
		goto INVALID;
	}

	*out_nextCursor = nextNextCursor;

	if (nbArguments != 0){
		return flParseTreeNewCall(output, nbArguments, arguments);
	}


	return output;

	INVALID: {
		flParseTreeFree(output);

		for (size_t i = 0 ; i < nbArguments ; i++){
			flParseTreeFree(arguments[i]);
		}

		return NULL;
	}
}



FLParseTree * flParseDefTree(FLStreamCursor const cursor, FLStreamCursor * const out_nextCursor)
{
	FlToken token;
	FLStreamCursor nextCursor = flTokenNext(cursor, &token);

	if (token.type != FL_TOKEN_VARIABLE){
		goto INVALID;
	}

	FLParseTree * expr = flParseTreeRecursive(nextCursor, out_nextCursor);

	if (expr == NULL){
		goto INVALID;
	}

	return flParseTreeNewDef(token.data.variableName, expr);

	INVALID:{
		flTokenFree(&token);
		return NULL;
	}
}


FLParseTree * flParseFunTree(FLStreamCursor const cursor, FLStreamCursor * const out_nextCursor)
{
	FlToken token;
	FLStreamCursor nextCursor = flTokenNext(cursor, &token);
	size_t nbParameters = 0;
	char * parameters[FL_PARSER_MAXIMUM_ARITY];

	if (token.type != FL_TOKEN_LEFT_BRACKET){
		goto INVALID;
	}

	while (1){

		nextCursor = flTokenNext(nextCursor, &token);

		if (token.type != FL_TOKEN_VARIABLE){
			break;
		}

		if (nbParameters == FL_PARSER_MAXIMUM_ARITY){
			goto INVALID;
		}

		parameters[nbParameters++] = token.data.variableName;
	}

	if (token.type != FL_TOKEN_RIGHT_BRACKET || nbParameters == 0){
		goto INVALID;
	}

	FLParseTree * body = flParseTreeRecursive(nextCursor, out_nextCursor);

	if (body == NULL){
		goto INVALID;
	}

	return flParseTreeNewFun(nbParameters, parameters, body);

	INVALID:{
		flTokenFree(&token);

		for (size_t i = 0  ; i < nbParameters ; i++){
			free(parameters[i]);
		}

		return NULL;
	}
}


FLParseTree * flParseLetTree(FLStreamCursor const cursor, FLStreamCursor * const out_nextCursor)
{
	return NULL;
}




FLParseTree * flParseTreeRecursive(FLStreamCursor const cursor, FLStreamCursor * const out_nextCursor)
{
	FlToken token;
	FLStreamCursor nextCursor = flTokenNext(cursor, &token);

	switch (token.type){

	case FL_TOKEN_VARIABLE:
		*out_nextCursor = nextCursor;
		return flParseTreeNewVar(token.data.variableName);

	case FL_TOKEN_KEYWORD:

		switch (token.data.keyword){

		case FL_KEYWORD_DEF:
			return flParseDefTree(nextCursor, out_nextCursor);

		case FL_KEYWORD_FUN:
			return flParseFunTree(nextCursor, out_nextCursor);

		case FL_KEY_WORD_LET:
			return flParseLetTree(nextCursor, out_nextCursor);

		default:
			return NULL;
		}
		break;

		case FL_TOKEN_LEFT_BRACKET:
			return flParseTreeLeftBracket(nextCursor, out_nextCursor);

		case FL_TOKEN_INVALID:
		case FL_TOKEN_END_OF_STREAM:
		case FL_TOKEN_RIGHT_BRACKET:
		default:
			return NULL;

	}
}



FLParseTree * flParseTree(FLStreamCursor cursor)
{
	FLStreamCursor endCursor;
	return flParseTreeRecursive(cursor, &endCursor);
}



