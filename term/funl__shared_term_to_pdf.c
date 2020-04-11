/*
 * funl__shared_term_to_pdf.c
 *
 *  Created on: 9 avr. 2020
 *      Author: ajuvenn
 */

#include "../funl__include.h"



#define FL_SHARED_TERM_GRAPHVIZ_MAX_NB_NODES 1024
static unsigned nbSeenTerms = 0;
static FLSharedTerm const ** seenTerms = NULL;
void flSharedTermSaveToDotFileRec(const FLSharedTerm * term, FILE * const f);



int flSharedTermSaveToPDF(const FLSharedTerm * term, const char * const outputPath)
{
	size_t outputPathSize = strlen(outputPath);
	char * dotFilePath = malloc(outputPathSize + 5);

	if (dotFilePath == NULL){
		return EXIT_FAILURE;
	}

	sprintf(dotFilePath, "%s.dot", outputPath);

	FILE * f = fopen(dotFilePath, "w");

	if (f == NULL){
		return EXIT_FAILURE;
	}

	nbSeenTerms = 0;
	seenTerms = malloc(FL_SHARED_TERM_GRAPHVIZ_MAX_NB_NODES * sizeof(FLSharedTerm *));

	if (seenTerms == NULL){
		free(dotFilePath);
		fclose(f);
		return EXIT_FAILURE;
	}

	fprintf(f, "digraph Term {\n");

	flSharedTermSaveToDotFileRec(term, f);

	fprintf(f, "}\n");
	fclose(f);
	nbSeenTerms = 0;
	free(seenTerms);

	char * cmd = malloc(2 * outputPathSize + 100);

	if (cmd == NULL){
		return EXIT_FAILURE;
	}

	sprintf(cmd, "dot -Tpdf %s -o %s", dotFilePath, outputPath);
	free(dotFilePath);

	int ret = system(cmd);

	free(cmd);

	return ret;
}


void flSharedTermSaveToDotFileRec(const FLSharedTerm * term, FILE * const f)
{
	for (unsigned i = 0 ; i < nbSeenTerms ; i++){
		if (seenTerms[i] == term){
			return;
		}
	}

	seenTerms[nbSeenTerms++] = term;

	switch (term->type){

	case FL_SHARED_TERM_CALL:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=%s];\n", term, "red", "call");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->call.func, "red", "func");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->call.arg, "red", "arg");

		flSharedTermSaveToDotFileRec(term->call.func, f);
		flSharedTermSaveToDotFileRec(term->call.arg, f);
		return;

	case FL_SHARED_TERM_FUN:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=%s];\n", term, "blue", "fun");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->fun.body, "blue", "body");

		flSharedTermSaveToDotFileRec(term->fun.body, f);
		return;

	case FL_SHARED_TERM_IF_ELSE:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s\"];\n", term, "black", "if else");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->ifElse.condition, "black", "condition");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->ifElse.thenValue, "black", "then");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->ifElse.elseValue, "black", "else");

		flSharedTermSaveToDotFileRec(term->ifElse.condition, f);
		flSharedTermSaveToDotFileRec(term->ifElse.thenValue, f);
		flSharedTermSaveToDotFileRec(term->ifElse.elseValue, f);
		return;

	case FL_SHARED_TERM_INTEGER:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%lld\"];\n", term, "green", term->integer);
		return;

	case FL_SHARED_TERM_REF:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s\"];\n", term, "orange", "ref");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\", style=dashed];\n", term, term->ref, "orange", "*");
		flSharedTermSaveToDotFileRec(term->ref, f);
		return;

	case FL_SHARED_TERM_ARGREF:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s\"];\n", term, "purple", "arg ref");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\", style=dashed];\n", term, term->ref, "purple", "*");
		return;

	default:
		return;
	}
}









