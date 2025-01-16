/*
 * cmp.c
 *
 *  Created on: 26 janv. 2024
 *      Author: apajadon
 */
#include <subModule/cmp.h>


char * cmpModeToString(cmp_mode_t mode)
{
	switch (mode) {
		case cmp_mode_notDefined:
			return "notDefined";
			break;
		case cmp_mode_nominal:
			return "nominal";
			break;
		case cmp_mode_degraded:
			return "degraded";
			break;
		case cmp_mode_failure:
			return "failure";
			break;
		default:
			return "NA";
			break;
	}
}
