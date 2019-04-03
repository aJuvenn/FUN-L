/*
 * funl__main.c
 *
 *  Created on: 9 janv. 2019
 *      Author: ajuvenn
 */
#include "funl__include.h"


int main(int argc, char **argv) {

	const char * s = "let rec true fun (x y) let z (+ x y) in z;";

	FLStreamCursor cursor = s;

	FlToken tk;

	do {
		flTokenNext(&cursor, &tk);
		flTokenPrint(&tk);
		printf("\n");

		flTokenFree(&tk);

	} while (tk.type != FL_TOKEN_END_OF_STREAM
			&& tk.type != FL_TOKEN_INVALID);

	cursor = s;

	printf("\nParsing Tree...\n");
	FLParseTree * tree = flParseTree(cursor);
	printf("Done. Printing tree...\n\n");
	flParseTreePrint(tree);
	printf("\n\nDone.\n");
	flParseTreeFree(tree);
	printf("\n");


	FLTerm * trueTerm =  flTermParse("(L.0) (L.L.1)");
	FLTerm * falseTerm = flTermParse("(L.0) (L.L.0)");
	FLTerm * ifTerm = flTermParse("(L.0) (L.L.L.((2) (1)) (0))");
	FLTerm * andTerm = flTermParse("L.L.(((2) (1)) (0)) (3)");
	FLTerm * orTerm = flTermParse("L.L.(((3) (1)) (5)) (0)");

	FLTerm * ifTrueAndFalseThenTrueElseFalseOrTrue = flTermParse(
			"(((2) (((1) (4)) (3))) (4)) (((0) (3)) (4))"
	);

	FLEnvironment env;
	flEnvironmentInit(&env, 25, 100);


	flVMEvaluateAndSaveTerm(trueTerm, &env);
	flVMEvaluateAndSaveTerm(falseTerm, &env);
	flVMEvaluateAndSaveTerm(ifTerm, &env);
	flVMEvaluateAndSaveTerm(andTerm, &env);
	flVMEvaluateAndSaveTerm(orTerm, &env);


	flVMEvaluateTerm(ifTrueAndFalseThenTrueElseFalseOrTrue, &env);

	return 0;

}

