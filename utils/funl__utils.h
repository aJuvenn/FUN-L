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


#define FL_FLAG_SET(value, flags)\
	do  {\
		(value) |= (flags);\
	} while (0)

#define FL_FLAG_UNSET(value, flags)\
	do  {\
		(value) &= ~(flags);\
	} while (0)

#define FL_FLAG_IS_SET(value, flags) ((value) & (flags))


#ifdef FL_DEBUG
#define FL_DEBUG_PRINT(...)\
	do {\
		fprintf(stdout, "[FUN-L][%s][line %u] ", __FUNCTION__, __LINE__);\
		fprintf(stdout, __VA_ARGS__);\
		fprintf(stdout, "\n");\
	} while (0)
#else
#define FL_DEBUG_PRINT(...)
#endif

#endif /* FUNL__UTILS_H_ */
