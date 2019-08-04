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
	FL_TOKEN_SEMICOLON,
	FL_TOKEN_KEYWORD,

	FL_TOKEN_END_OF_STREAM

} FLTokenType;



typedef enum {

	FL_KEYWORD_FUN = 0,
	FL_KEY_WORD_LET,
	FL_KEY_WORD_IN,
	FL_KEY_WORD_REC,

	FL_KEY_WORD_MAX_VALUE

} FlTokenKeyWordData;


/*
 * FlToken type represents a token extracted from a text
 * file reading.
 */
typedef struct {

	/* Type field specifying which kind of token it is */
	FLTokenType type;

	/* Data contained in the token, depending on its type */
	union {
		char * variableName;
		FlTokenKeyWordData keyword;
	} data;

} FlToken;



/*
 * A FLStreamCursor is a char pointer used to parse a
 * text file into tokens.
 */
typedef const char * FLStreamCursor;


/*
 * Reads and increments the stream cursor to store into out_token the
 * next token in file. In case of failure, out_token receives
 * FL_TOKEN_INVALID as type.
 */
void flTokenNext(FLStreamCursor * const cursor, FlToken * const out_token);

/*
 * Prints on standard output the token
 */
void flTokenPrint(const FlToken * const tk);


/*
 * Frees the string possibly contained into the token.
 * The pointer tk was not dynamically allocated.
 */
void flTokenFree(FlToken * tk);


#endif /* FUNL__TOKENIZER_H_ */
