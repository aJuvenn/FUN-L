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


FLTerm * flTermNewCall(FLTerm * fun, FLTerm * arg)
{
	FLTerm * output = flTermNew();

	output->type = FL_TERM_CALL;
	output->data.call.func = fun;
	output->data.call.arg = arg;

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
	switch (term->type){

	case FL_TERM_VAR_ID:
		printf("%u", term->data.varId);
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

	default:
		printf("<INVALID TERM>");
		return;
	}
}
