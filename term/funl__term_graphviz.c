/*
 * funl__term_graphviz.c
 *
 *  Created on: 8 avr. 2020
 *      Author: ajuvenn
 */


#include "../funl__include.h"

void flTermSaveToDotFileRec(const FLTerm * term, FILE * const f);


int flTermSaveToDotFile(const FLTerm * term, const char * const outputPath)
{
	FILE * f = fopen(outputPath, "w");

	if (f == NULL){
		return EXIT_FAILURE;
	}

	fprintf(f, "digraph Term {\n");

	flTermSaveToDotFileRec(term, f);

	fprintf(f, "}\n");
	fclose(f);

	return EXIT_FAILURE;
}


void flTermSaveToDotFileRec(const FLTerm * term, FILE * const f)
{
	switch (term->type){

	case FL_TERM_CALL:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=%s];\n", term, "red", "call");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.call.func, "red", "func");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.call.arg, "red", "arg");

		flTermSaveToDotFileRec(term->data.call.func, f);
		flTermSaveToDotFileRec(term->data.call.arg, f);
		return;

	case FL_TERM_FUN:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=%s];\n", term, "blue", "fun");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.funBody, "blue", "body");

		flTermSaveToDotFileRec(term->data.funBody, f);
		return;

	case FL_TERM_GLOBAL_VAR_ID:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"global $%u\"];\n", term, "green", term->data.varId);
		return;

	case FL_TERM_IF_ELSE:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s\"];\n", term, "black", "if else");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.ifElse.condition, "black", "condition");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.ifElse.thenValue, "black", "then");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.ifElse.elseValue, "black", "else");

		flTermSaveToDotFileRec(term->data.ifElse.condition, f);
		flTermSaveToDotFileRec(term->data.ifElse.thenValue, f);
		flTermSaveToDotFileRec(term->data.ifElse.elseValue, f);
		return;

	case FL_TERM_INTEGER:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%lld\"];\n", term, "green", term->data.integer);
		return;

	case FL_TERM_LET:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=%s];\n", term, "black", "let");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.let.affect, "black", "affect");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->data.let.following, "black", "following");

		flTermSaveToDotFileRec(term->data.let.affect, f);
		flTermSaveToDotFileRec(term->data.let.following, f);
		return;

	case FL_TERM_OPCALL:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s %d\"];\n", term, "black", "opcall", term->data.opCall.op);

		for (size_t i = 0 ; i < term->data.opCall.nbArguments ; i++){
			fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s %zu]\"];\n", term, term->data.opCall.arguments[i], "black", "op arg", i);
		}

		for (size_t i = 0 ; i < term->data.opCall.nbArguments ; i++){
			flTermSaveToDotFileRec(term->data.opCall.arguments[i], f);
		}

		return;

	case FL_TERM_VAR_ID:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"local $%u\"];\n", term, "green", term->data.varId);
		return;

	default:
		return;
	}
}
