/*
 * funl__parse_tree_utils.c
 *
 *  Created on: 2 sept. 2019
 *      Author: ajuvenn
 */

#include "../funl__include.h"


int flParseTreeIsAGlobalDefinition(const FLParseTree * const tree)
{
	return tree->type == FL_PARSE_TREE_LET && tree->data.let.following == NULL;
}
