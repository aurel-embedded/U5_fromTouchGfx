/*
 * dbg_debugPrintf.h
 *
 *  Created on: 8 mars 2018
 *      Author: apajadon
 */

#ifndef DBG_DBG_DEBUGPRINTF_H_
#define DBG_DBG_DEBUGPRINTF_H_
#include <main.h>
#include <Tools/serial_VT100.h>


#define DBG_ERROR(...) {DBG_printf(VT100_FG_RED);\
						DBG_printf("[ERROR]:");\
						DBG_printf(__VA_ARGS__);\
						DBG_printf(VT100_COLOR_RESET);}
#define DBG_ERRORX(...) {DBG_printf(VT100_FG_LIGHTRED);\
						DBG_printf("[ERROR]:%s l.%d - ", __func__, __LINE__);\
						DBG_printf(__VA_ARGS__);\
						DBG_printf(VT100_COLOR_RESET);}
#define DBG_WARNING(...) {DBG_printf(VT100_FG_LIGHTRED);\
						DBG_printf("[WARNING]:");\
						DBG_printf(__VA_ARGS__);\
						DBG_printf(VT100_COLOR_RESET);}
#define DBG_WARNINGX(...) {DBG_printf(VT100_FG_RED);\
						DBG_printf("[WARNING]:%s l.%d - ", __func__, __LINE__);\
						DBG_printf(__VA_ARGS__);\
						DBG_printf(VT100_COLOR_RESET);}
#define DBG_INFO(...) 	{DBG_printf(VT100_FG_YELLOW);\
						DBG_printf("[INFO]:");\
						DBG_printf(__VA_ARGS__);\
						DBG_printf(VT100_COLOR_RESET);}
#define DBG_INFOX(...) 	{DBG_printf(VT100_FG_YELLOW);\
						DBG_printf("[INFO]:%s l.%d - ", __func__, __LINE__);\
						DBG_printf(__VA_ARGS__);\
						DBG_printf(VT100_COLOR_RESET);}
#define DBG_clearScreen()		DBG_printf(VT100_CLEARSCR)
#define DBG_goto(x,y) 			DBG_printf(VT100_GOTOYX, x,y)


extern HAL_StatusTypeDef 	DBG_init(void);
extern HAL_StatusTypeDef 	DBG_exit(void);
extern HAL_StatusTypeDef 	DBG_printf(const char *fmt, ...);
extern HAL_StatusTypeDef 	DBG_printfLine(uint16_t X, uint16_t Y,const char *fmt, ...);
extern void 				DBG_lock(void);
extern void 				DBG_unlock(void);
extern HAL_StatusTypeDef 	DBG_printfNoLock(const char *fmt, ...);
extern void 				DBG_displayBanner(void);

#endif /* DBG_DBG_DEBUGPRINTF_H_ */
