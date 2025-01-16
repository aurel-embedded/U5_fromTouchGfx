/*
 * THW_hardwareTest_common.h
 *
 *  Created on: 07 jan. 2023
 *      Author: apajadon
 */

#ifndef USER_THW_THW_HARDWARETEST_COMMON_H_
#define USER_THW_THW_HARDWARETEST_COMMON_H_
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <main.h>
#include <Tools/serial_VT100.h>
#include "Config/thw_config.h"

//----------------------------------------------------------------------
// MENU
#define THW_MENU_ITEM_NAME_SIZE 50
typedef struct _st_thw_menuItem{
	 char 		name[THW_MENU_ITEM_NAME_SIZE];
	 void		(*pActionFn)(void);
	 uint32_t	info;
}st_thw_menuItem;

// Gestion des menus
typedef struct _st_thw_actualMenu{
	void (*displayMenu)(void);					// Pointeur sur Fonction Display menu
	void (*manageChoiceFn)(char CodeToManage);	// Pointeur sur Fonction ManageChoice
	void (*refreshFn)(void);					// Pointeur sur Fonction Refresh
	uint16_t	refreshPeriodInMs;				// Période de rafraichissement de la fonction refresh en ms
	bool		clearScreen;					// Flag pour effacer l'écran
}st_thw_actualMenu;

// Commande commune à tous les menus
typedef enum{
	thw_cmdRetour = 0,
	thw_menuNumber_InvalidChoice = 0xFFFF
}eHwt_commonCmd;
//----------------------------------------------------------------------

typedef enum{
	thw_testResult_testNA = 0,
	thw_testResult_testRunning,
	thw_testResult_testERROR,
	thw_testResult_testOK,
}thw_testResult_t;


//----------------------------------------------------------------------
// THW COM
#define RX_BUF_SIZE	10
extern volatile uint8_t thw_line[RX_BUF_SIZE+1];
extern HAL_StatusTypeDef	thw_com_init(void);
extern HAL_StatusTypeDef	thw_com_exit(void);
extern uint16_t 			thw_com_transmit(uint8_t *pData, uint16_t Size);
extern bool 				thw_com_manageRx(void);
extern HAL_StatusTypeDef 	THW_COM_printf(const char *fmt, ...);

//----------------------------------------------------------------------
extern void THW_avoidClearScreen(void);
extern void THW_displayActionMenu(st_thw_menuItem *pMenuItems, uint8_t menuItemsQty);

// Definition de macros de gestion de l'ecran
#define THW_printf(...)			THW_COM_printf( __VA_ARGS__)
#define THW_printfCL(...)		THW_COM_printf(__VA_ARGS__ VT100_COLOR_RESET"\r\n")
#define THW_clearScreen()		THW_printf(VT100_CLEARSCR)
#define THW_clearEndOfScreen()	THW_printf(VT100_CLEAREOS)
#define THW_textColor(Color) 	THW_printf(Color)
#define THW_goto(x,y) 			THW_printf(VT100_GOTOYX, x,y)
#define THW_saveCurPos() 		THW_printf(VT100_SAVEPOS)
#define THW_restoreCurPos() 	THW_printf(VT100_RESTOREPOS)
#define THW_xtermTitle(Name) 	THW_printf("\033]0;%s\007", Name)
#define THW_saveCurPos() 		THW_printf(VT100_SAVEPOS)
#define THW_restoreCurPos() 	THW_printf(VT100_RESTOREPOS)

#define THW_Banner(Menu)	THW_printf(" %s (v%s) | THW | %s\r\n", THW_SoftwareReference, THW_SoftwareVersion, Menu)

extern st_thw_actualMenu thw_actualMenu;	// Structure sur le menu actuellement utilisé

#endif /* USER_THW_THW_HARDWARETEST_COMMON_H_ */
