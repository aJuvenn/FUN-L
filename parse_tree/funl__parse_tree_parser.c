/*
 * funl__parse_tree.c
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */


#include "../funl__include.h"




FLParseTree * flParseTreeLeftBracket(FLStreamCursor * const cursor, const FLTokenType expectedRightBracket)
{
	FLParseTree * output = flParseTreeRecursive(cursor);

	if (output == NULL){
		return NULL;
	}

	size_t nbArguments = 0;
	FLParseTree * arguments[FL_PARSER_MAXIMUM_ARITY];

	while (1){

		FLStreamCursor lastCursorValue = *cursor;
		FLParseTree * nextTree = flParseTreeRecursive(cursor);

		if (nextTree == NULL){
			*cursor = lastCursorValue;
			break;
		}

		if (nbArguments == FL_PARSER_MAXIMUM_ARITY){
			goto INVALID;
		}

		arguments[nbArguments++] = nextTree;
	}

	FlToken token;
	flTokenNext(cursor, &token);

	if (token.type != expectedRightBracket){
		flTokenFree(&token);
		goto INVALID;
	}

	if (nbArguments != 0){
		int isACallByName = (expectedRightBracket == FL_TOKEN_RIGHT_SQUARE_BRACKET);
		return flParseTreeNewCall(output, nbArguments, arguments, isACallByName);
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




FLParseTree * flParseFunTree(FLStreamCursor * const cursor)
{
	FlToken token;
	size_t nbParameters = 0;
	char * parameters[FL_PARSER_MAXIMUM_ARITY];

	flTokenNext(cursor, &token);

	FLTokenType leftBracketType = token.type;
	FLTokenType expectedRightBracketType;

	if (leftBracketType == FL_TOKEN_LEFT_BRACKET){
		expectedRightBracketType = FL_TOKEN_RIGHT_BRACKET;
	} else if (leftBracketType == FL_TOKEN_LEFT_SQUARE_BRACKET){
		expectedRightBracketType = FL_TOKEN_RIGHT_SQUARE_BRACKET;
	} else {
		goto INVALID;
	}

	while (1){

		flTokenNext(cursor, &token);

		if (token.type != FL_TOKEN_VARIABLE){
			break;
		}

		if (nbParameters == FL_PARSER_MAXIMUM_ARITY){
			goto INVALID;
		}

		parameters[nbParameters++] = token.data.variableName;
	}


	if (token.type != expectedRightBracketType || nbParameters == 0){
		goto INVALID;
	}

	FLParseTree * body = flParseTreeRecursive(cursor);

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



FLParseTree * flParseLetTree(FLStreamCursor * const cursor)
{
	FlToken token;
	FLParseTree * affect = NULL;
	char * variable = NULL;
	int recursive = 0;

	flTokenNext(cursor, &token);

	if (token.type == FL_TOKEN_KEYWORD && token.data.keyword == FL_KEY_WORD_REC){
		recursive = 1;
		flTokenNext(cursor, &token);
	}

	if (token.type != FL_TOKEN_VARIABLE){
		goto INVALID;
	}

	affect = flParseTreeRecursive(cursor);

	if (affect == NULL){
		goto INVALID;
	}

	variable = token.data.variableName;
	flTokenNext(cursor, &token);

	if (token.type == FL_TOKEN_SEMICOLON){
		return flParseTreeNewLet(variable, affect, NULL, recursive);
	}

	if (token.type != FL_TOKEN_KEYWORD || token.data.keyword != FL_KEY_WORD_IN){
		goto INVALID;
	}

	FLParseTree * following = flParseTreeRecursive(cursor);

	if (following == NULL){
		goto INVALID;
	}

	return flParseTreeNewLet(variable, affect, following, recursive);


	INVALID:{

		if (affect != NULL){
			flParseTreeFree(affect);
		}

		if (variable != NULL){
			free(variable);
		}

		flTokenFree(&token);

		return NULL;
	}
}



FLParseTree * flParseIfElseTree(FLStreamCursor * const cursor)
{
	FlToken token;

	FLParseTree * condition = flParseTreeRecursive(cursor);

	if (condition == NULL){
		return NULL;
	}

	FLParseTree * thenValue = flParseTreeRecursive(cursor);

	if (thenValue == NULL){
		flParseTreeFree(condition);
		return NULL;
	}

	flTokenNext(cursor, &token);

	if (token.type != FL_TOKEN_KEYWORD || token.data.keyword != FL_KEY_WORD_ELSE){
		flParseTreeFree(condition);
		flParseTreeFree(thenValue);
		flTokenFree(&token);
		return NULL;
	}

	FLParseTree * elseValue = flParseTreeRecursive(cursor);

	if (elseValue == NULL){
		flParseTreeFree(condition);
		flParseTreeFree(thenValue);
		return NULL;
	}

	flTokenNext(cursor, &token);

	if (token.type != FL_TOKEN_KEYWORD || token.data.keyword != FL_KEY_WORD_END){
		flParseTreeFree(condition);
		flParseTreeFree(thenValue);
		flParseTreeFree(elseValue);
		flTokenFree(&token);
		return NULL;
	}

	return flParseTreeNewIfElse(condition, thenValue, elseValue);
}



FLParseTree * flParseTreeRecursive(FLStreamCursor * const cursor)
{
	FlToken token;
	flTokenNext(cursor, &token);

	switch (token.type){

	case FL_TOKEN_VARIABLE:
		return flParseTreeNewVar(token.data.variableName);

	case FL_TOKEN_INTEGER:
		return flParseTreeNewInteger(token.data.integer);

	case FL_TOKEN_OPERATOR:
		return flParseTreeNewOp(token.data.operator);

	case FL_TOKEN_KEYWORD:

		switch (token.data.keyword){

		case FL_KEY_WORD_FUN:
			return flParseFunTree(cursor);

		case FL_KEY_WORD_LET:
			return flParseLetTree(cursor);

		case FL_KEY_WORD_IF:
			return flParseIfElseTree(cursor);

		default:
			return NULL;
		}
		break;

	case FL_TOKEN_LEFT_BRACKET:
		return flParseTreeLeftBracket(cursor, FL_TOKEN_RIGHT_BRACKET);

	case FL_TOKEN_LEFT_SQUARE_BRACKET:
		return flParseTreeLeftBracket(cursor, FL_TOKEN_RIGHT_SQUARE_BRACKET);

	case FL_TOKEN_END_OF_STREAM:
		return flParseTreeNewEndOfFile();

	default:
		return NULL;

	}
}



FLParseTree * flParseTree(const FLStreamCursor cursor)
{
	FLStreamCursor endCursor = cursor;
	FLParseTree * output = flParseTreeRecursive(&endCursor);

	if (output == NULL){
		printf("Cursor stopped at %p (%lu character)\n", endCursor, endCursor - cursor);
	}

	return output;
}






