/*
 * funl__term.c
 *
 *  Created on: 1 avr. 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"



FLTerm * flTermNew()
{
	FLTerm * output;
	FL_SIMPLE_ALLOC(output, 1);
	return output;
}


FLTerm * flTermNewFun(FLTerm * funBody)
{
	FLTerm * output = flTermNew();

	output->type = FL_TERM_FUN;
	output->data.funBody = funBody;

	return output;
}


FLTerm * flTermNewVarId(FLTermId id)
{
	FLTerm * output = flTermNew();

	output->type = FL_TERM_VAR_ID;
	output->data.varId = id;

	return output;
}


FLTerm * flTermNewGlobalVarId(FLTermId id)
{
	FLTerm * output = flTermNew();

	output->type = FL_TERM_GLOBAL_VAR_ID;
	output->data.varId = id;

	return output;
}



FLTerm * flTermNewCall(FLTerm * fun, FLTerm * arg)
{
	FLTerm * output = flTermNew();

	output->type = FL_TERM_CALL;
	output->data.call.func = fun;
	output->data.call.arg = arg;

	return output;
}


FLTerm * flTermNewLet(FLTerm * affect, FLTerm * following)
{
	FLTerm * output = flTermNew();

	output->type = FL_TERM_LET;
	output->data.let.affect = affect;
	output->data.let.following = following;

	return output;
}



FLTerm * flTermParseRec(FLStreamCursor * const cursor)
{
	if (cursor == NULL || *cursor == NULL || **cursor == 0){
		return NULL;
	}

	char c = *((*cursor)++);

	switch (c){

	case 'L':

		if (*((*cursor)++) != '.'){
			return NULL;
		}

		return flTermNewFun(flTermParseRec(cursor));

	case '(':
	{

		FLTerm * app = flTermParseRec(cursor);
		char c1, c2, c3;

		if (app == NULL){
			return NULL;
		}

		c1 = *((*cursor)++);
		c2 = *((*cursor)++);
		c3 = *((*cursor)++);

		if (c1 != ')' || c2 != ' ' || c3 != '('){
			return NULL;
		}

		FLTerm * arg = flTermParseRec(cursor);

		if (arg == NULL){
			return NULL;
		}

		if (*((*cursor)++) != ')'){
			return NULL;
		}

		return flTermNewCall(app, arg);
	}

	default:
	{

		FLTermId s = 0;

		if (!isdigit(c)){
			return NULL;
		}

		do {

			s *= 10;
			s += (c - '0');
			c = *((*cursor)++);

		} while (isdigit(c));

		if (c != ')' && c != 0){
			return NULL;
		}

		(*cursor)--;

		return flTermNewVarId(s);
	}
	}


}



FLTerm * flTermParse(const char * const str)
{
	FLStreamCursor cursor = str;
	return flTermParseRec(&cursor);
}



void flTermPrint(const FLTerm * const term)
{
	if (term == NULL){
		printf("(NULL)");
		return;
	}

	switch (term->type){

	case FL_TERM_VAR_ID:
		printf("%u", term->data.varId);
		return;

	case FL_TERM_GLOBAL_VAR_ID:
		printf("<g>%u", term->data.varId);
		return;

	case FL_TERM_FUN:
		printf("L.");
		flTermPrint(term->data.funBody);
		return;

	case FL_TERM_CALL:
		printf("(");
		flTermPrint(term->data.call.func);
		printf(") (");
		flTermPrint(term->data.call.arg);
		printf(")");
		return;


	case FL_TERM_LET:
		printf("let ");
		flTermPrint(term->data.let.affect);
		printf(" in ");
		flTermPrint(term->data.let.following);
		return;

	default:
		printf("<INVALID TERM>");
		return;
	}
}






FLTerm * flTermCopy(const FLTerm * const term)
{
	if (term == NULL){
		return NULL;
	}

	switch (term->type){

	case FL_TERM_VAR_ID:
		return flTermNewVarId(term->data.varId);

	case FL_TERM_GLOBAL_VAR_ID:
		return flTermNewGlobalVarId(term->data.varId);

	case FL_TERM_FUN:
		return flTermNewFun(flTermCopy(term->data.funBody));

	case FL_TERM_CALL:
		return flTermNewCall(flTermCopy(term->data.call.func), flTermCopy(term->data.call.arg));

	case FL_TERM_LET:
		return flTermNewLet(flTermCopy(term->data.let.affect), flTermCopy(term->data.let.following));

	default:
		return NULL;
	}
}




void flTermFree(FLTerm * term)
{
	if (term == NULL){
		return;
	}

	switch (term->type){

	case FL_TERM_FUN:
		flTermFree(term->data.funBody);
		break;

	case FL_TERM_CALL:
		flTermFree(term->data.call.func);
		flTermFree(term->data.call.arg);
		break;

	case FL_TERM_LET:
		flTermFree(term->data.let.affect);
		flTermFree(term->data.let.following);
		break;

	default:
		break;

	}

	free(term);
}


