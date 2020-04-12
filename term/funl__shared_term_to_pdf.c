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


void flSharedTermSaveToDotFileRec(const FLSharedTerm * term, FILE * const f, const FLEnvironment * const env)
{
	for (unsigned i = 0 ; i < nbSeenTerms ; i++){
		if (seenTerms[i] == term){
			return;
		}
	}

	seenTerms[nbSeenTerms++] = term;

	if (term == NULL){
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s\"];\n", NULL, "red", "NULL");
		return;
	}

	switch (term->type){

	case FL_SHARED_TERM_CALL:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s [%zu]\"];\n", term, "red", "call", term->nbReferences);
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->call.func, "red", "func");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->call.arg, "red", "arg");

		flSharedTermSaveToDotFileRec(term->call.func, f, env);
		flSharedTermSaveToDotFileRec(term->call.arg, f, env);
		return;

	case FL_SHARED_TERM_FUN:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s [%zu]\"];\n", term, "blue", "fun", term->nbReferences);
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->fun.body, "blue", "body");

		flSharedTermSaveToDotFileRec(term->fun.body, f, env);
		return;

	case FL_SHARED_TERM_IF_ELSE:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s [%zu]\"];\n", term, "black", "if else", term->nbReferences);
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->ifElse.condition, "black", "condition");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->ifElse.thenValue, "black", "then");
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->ifElse.elseValue, "black", "else");

		flSharedTermSaveToDotFileRec(term->ifElse.condition, f, env);
		flSharedTermSaveToDotFileRec(term->ifElse.thenValue, f, env);
		flSharedTermSaveToDotFileRec(term->ifElse.elseValue, f, env);
		return;

	case FL_SHARED_TERM_LET:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s [%zu]\"];\n", term, "yellow", term->let.isRecursive ? "let rec" : "let", term->nbReferences);
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->let.affect, "yellow", "affect");
		if (term->let.following){
			fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\"];\n", term, term->let.following, "yellow", "following");
		}
		flSharedTermSaveToDotFileRec(term->let.affect, f, env);
		if (term->let.following){
			flSharedTermSaveToDotFileRec(term->let.following, f, env);
		}
		return;

	case FL_SHARED_TERM_INTEGER:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%lld [%zu]\"];\n", term, "green", term->integer, term->nbReferences);
		return;

	case FL_SHARED_TERM_REF:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s [%zu]\"];\n", term, "orange", "ref", term->nbReferences);
		fprintf(f, "\"%p\" -> \"%p\" [color=%s, label=\"[%s]\", style=dashed];\n", term, term->ref, "orange", "*");
		flSharedTermSaveToDotFileRec(term->ref, f, env);
		return;

	case FL_SHARED_TERM_VAR:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s var %s $%zu [%zu]\"];\n", term, "green",
				term->var.isGlobal ? "global" : "local", term->var.isGlobal ? env->globalVarNameStack[term->var.id] : "",
						term->var.id, term->nbReferences);
		return;

	default:
		fprintf(f, "\"%p\" [color=%s, shape=box, label=\"%s [%zu]\"];\n", term, "black", "UNKNOWN", term->nbReferences);
		return;
	}
}


int flSharedTermSaveToPDF(const FLSharedTerm * term, const char * const outputPath, const FLEnvironment * const env)
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

	flSharedTermSaveToDotFileRec(term, f, env);

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









