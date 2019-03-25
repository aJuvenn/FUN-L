/*
 * funl__tokenizer.c
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"




int flIsValidCharForVariableName(char c)
{
	return isgraph(c) && c != '(' && c != ')';
}


static const char * flKeyWords[FL_KEY_WORD_MAX_VALUE] = {
		"fun",
		"def",
		"let"
};


const char * flTokenNextFromWord(const char * const cursor, FlToken * const out_token)
{
	unsigned wordSize = 0;

	while (flIsValidCharForVariableName(cursor[wordSize])){
		wordSize++;
	}

	if (wordSize == 0){
		out_token->type = FL_TOKEN_INVALID;
		return NULL;
	}

	char * tokenName;
	FL_SIMPLE_ALLOC(tokenName, wordSize + 1);
	memcpy(tokenName, cursor, wordSize);
	tokenName[wordSize] = 0;

	for (FlTokenKeyWordData kw = 0 ; kw < FL_KEY_WORD_MAX_VALUE ; ++kw){

		if (strcmp(tokenName, flKeyWords[kw]) != 0){
			continue;
		}

		free(tokenName);

		out_token->type = FL_TOKEN_KEYWORD;
		out_token->data.keyword = kw;

		return cursor + wordSize;
	}

	out_token->type = FL_TOKEN_VARIABLE;
	out_token->data.variableName = tokenName;

	return cursor + wordSize;
}



FLStreamCursor flTokenNext(FLStreamCursor const cursor, FlToken * const out_token)
{
	if (cursor == NULL || out_token == NULL){
		return NULL;
	}

	const char * outCursor = cursor;

	while (isspace(*outCursor)){
		outCursor++;
	}

	switch (*outCursor){

	case 0:
	case EOF:
		out_token->type = FL_TOKEN_END_OF_STREAM;
		return outCursor;

	case '(':
		out_token->type = FL_TOKEN_LEFT_BRACKET;
		return outCursor + 1;

	case ')':
		out_token->type = FL_TOKEN_RIGHT_BRACKET;
		return outCursor + 1;

	default:
		break;
	}

	return flTokenNextFromWord(outCursor, out_token);
}



void flTokenPrint(const FlToken * const tk)
{
	switch (tk->type){

	case FL_TOKEN_VARIABLE:
		printf("VARIABLE(%s)", tk->data.variableName);
		return;

	case FL_TOKEN_KEYWORD:
		printf("KEYWORD(%s)", flKeyWords[tk->data.keyword]);
		return;

	case FL_TOKEN_LEFT_BRACKET:
		printf("LEFT_BRACKET");
		return;

	case FL_TOKEN_RIGHT_BRACKET:
		printf("RIGHT_BRACKET");
		return;

	case FL_TOKEN_END_OF_STREAM:
		printf("END");
		return;

	case FL_TOKEN_INVALID:
		printf("INVALID");
		return;

	default:
		printf("INVALID_TOKEN_TYPE");
		return;
	}
}


void flTokenFree(FlToken * tk)
{
	if (tk->type == FL_TOKEN_VARIABLE){
		free(tk->data.variableName);
	}
}
