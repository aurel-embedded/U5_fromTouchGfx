/*
 * error.h
 *
 *  Created on: Oct 17, 2024
 *      Author: apajadon
 */

#ifndef ASSERTERROR_H_
#define ASSERTERROR_H_

#define ASSERT_ERROR(INFO) {																				\
	printf("*** Critical error in file: %s, on line: %d (Info: %s) \r\n", __FILE__, __LINE__, INFO);		\
	while(1){}																								\
}																											\




#endif /* ASSERTERROR_H_ */
