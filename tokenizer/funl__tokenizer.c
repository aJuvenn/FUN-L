/*
 * funl__tokenizer.c
 *
 *  Created on: 25 mars 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



/*
 * Returns 1 if the char can be used in a variable name
 * (which mean it is printable and not a semicolon or bracket)
 * Otherwise it returns 0.
 */
int flIsValidCharForVariableName(char c)
{
	return isgraph(c) && c != '(' && c != ')' && c != ';' && c != '[' && c != ']';
}


/*
 * List of langage key words. Their order must be the same as
 * in FlTokenKeyWordData corresponding enum.
 */
static const char * flKeyWords[FL_KEY_WORD_MAX_VALUE] = {
		"fun",
		"let",
		"in",
		"rec"
};


/*
 * If the cursor points to the beginning of a word, it is increased and read until
 * the end of the word. The corresponding token is written in out_token (it can be
 * either a key word or a variable name). In case of failure, out_token receives
 * FL_TOKEN_INVALID as type.
 */
void flTokenNextFromWord(FLStreamCursor * const cursor, FlToken * const out_token)
{
	unsigned wordSize = 0;

	while (flIsValidCharForVariableName((*cursor)[wordSize])){
		wordSize++;
	}

	if (wordSize == 0){
		out_token->type = FL_TOKEN_INVALID;
		return;
	}

	char * tokenName;
	FL_SIMPLE_ALLOC(tokenName, wordSize + 1);
	memcpy(tokenName, *cursor, wordSize);
	tokenName[wordSize] = 0;

	for (FlTokenKeyWordData kw = 0 ; kw < FL_KEY_WORD_MAX_VALUE ; ++kw){

		if (strcmp(tokenName, flKeyWords[kw]) != 0){
			continue;
		}

		free(tokenName);

		out_token->type = FL_TOKEN_KEYWORD;
		out_token->data.keyword = kw;

		*cursor += wordSize;
		return;
	}

	out_token->type = FL_TOKEN_VARIABLE;
	out_token->data.variableName = tokenName;
	*cursor += wordSize;
}


/*
 * Reads and increments the stream cursor to store into out_token the
 * next token in file. In case of failure, out_token receives
 * FL_TOKEN_INVALID as type.
 */
void flTokenNext(FLStreamCursor * const cursor, FlToken * const out_token)
{
	if (cursor == NULL || *cursor == NULL || out_token == NULL){
		return;
	}

	while (isspace(**cursor)){
		(*cursor)++;
	}

	switch (**cursor){

	case 0:
	case EOF:
		out_token->type = FL_TOKEN_END_OF_STREAM;
		return;

	case '(':
		out_token->type = FL_TOKEN_LEFT_BRACKET;
		(*cursor)++;
		return;

	case ')':
		out_token->type = FL_TOKEN_RIGHT_BRACKET;
		(*cursor)++;
		return;

	case '[':
		out_token->type = FL_TOKEN_LEFT_SQUARE_BRACKET;
		(*cursor)++;
		return;

	case ']':
		out_token->type = FL_TOKEN_RIGHT_SQUARE_BRACKET;
		(*cursor)++;
		return;

	case ';':
		out_token->type = FL_TOKEN_SEMICOLON;
		(*cursor)++;
		return;

	default:
		flTokenNextFromWord(cursor, out_token);
		return;
	}

}


/*
 * Prints on standard output the token
 */
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

	case FL_TOKEN_LEFT_SQUARE_BRACKET:
		printf("LEFT_SQUARE_BRACKET");
		return;

	case FL_TOKEN_RIGHT_SQUARE_BRACKET:
		printf("RIGHT_SQUARE_BRACKET");
		return;

	case FL_TOKEN_END_OF_STREAM:
		printf("END");
		return;

	case FL_TOKEN_INVALID:
		printf("INVALID");
		return;

	case FL_TOKEN_SEMICOLON:
		printf("SEMICOLON");
		return;

	default:
		printf("INVALID_TOKEN_TYPE");
		return;
	}
}


/*
 * Frees the string possibly contained into the token.
 * The pointer tk was not dynamically allocated.
 */
void flTokenFree(FlToken * tk)
{
	if (tk->type == FL_TOKEN_VARIABLE){
		free(tk->data.variableName);
	}
}

