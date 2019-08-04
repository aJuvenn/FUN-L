/*
 * funl__utils.h
 *
 *  Created on: 9 janv. 2019
 *      Author: ajuvenn
 */

#ifndef FUNL__UTILS_H_
#define FUNL__UTILS_H_

#include "../funl__include.h"


#define FL_SIMPLE_ALLOC(ptr, nb_elements)\
	if ((nb_elements) == 0){\
		fprintf(stderr, "FL_SIMPLE_ALLOC (in %s) : received 0 as number of elements\n", __FUNCTION__);\
		exit(EXIT_FAILURE);\
	}\
	(ptr) = malloc((nb_elements) * sizeof(*(ptr)));\
	if ((ptr) == NULL){\
		fprintf(stderr, "FL_SIMPLE_ALLOC (in %s) : malloc returned NULL\n", __FUNCTION__);\
		exit(EXIT_FAILURE);\
	}\



#endif /* FUNL__UTILS_H_ */
