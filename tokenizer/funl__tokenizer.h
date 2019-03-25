/*
 * funl__tokenizer.h
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */

#ifndef FUNL__TOKENIZER_H_
#define FUNL__TOKENIZER_H_


#include "../funl__include.h"

typedef enum {

	FL_TOKEN_INVALID = 0,

	FL_TOKEN_VARIABLE,
	FL_TOKEN_LEFT_BRACKET,
	FL_TOKEN_RIGHT_BRACKET,
	FL_TOKEN_KEYWORD,

	FL_TOKEN_END_OF_STREAM

} FlTokenType;



typedef enum {

	FL_KEYWORD_FUN = 0,
	FL_KEYWORD_DEF = 1,
	FL_KEY_WORD_LET = 2,

	FL_KEY_WORD_MAX_VALUE

} FlTokenKeyWordData;



typedef struct {

	FlTokenType type;

	union {
		char * variableName;
		FlTokenKeyWordData keyword;
	} data;

} FlToken;


typedef const char * FLStreamCursor;

FLStreamCursor flTokenNext(FLStreamCursor const cursor, FlToken * const out_token);

void flTokenPrint(const FlToken * const tk);

void flTokenFree(FlToken * tk);


#endif /* FUNL__TOKENIZER_H_ */
