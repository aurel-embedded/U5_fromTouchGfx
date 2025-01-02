/*
 * Tools.c
 *
 *  Created on: Nov 22, 2024
 *      Author: apajadon
 */





long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x-in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

