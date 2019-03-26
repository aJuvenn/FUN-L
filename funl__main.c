/*
 * funl__main.c
 *
 *  Created on: 9 janv. 2019
 *      Author: ajuvenn
 */
#include "funl__include.h"


int main(int argc, char **argv) {

	const char * s = "let true fun (x y) x let false fun (x y) y let if fun (bool then else) (bool then else) true";

	FLStreamCursor cursor = s;

	FlToken tk;

	do {
		cursor = flTokenNext(cursor, &tk);
		flTokenPrint(&tk);
		printf("\n");

		flTokenFree(&tk);

	} while (tk.type != FL_TOKEN_END_OF_STREAM
			&& tk.type != FL_TOKEN_INVALID);

	cursor = s;
	FLParseTree * tree = flParseTree(cursor);

	flParseTreePrint(tree);
	flParseTreeFree(tree);

	return 0;

}

