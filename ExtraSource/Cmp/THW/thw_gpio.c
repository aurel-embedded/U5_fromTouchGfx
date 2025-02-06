
/*
 * thw_gpio.c
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * les hardfault afin d pouvoir s'entrainer à les identifier\n
 * Ces fonctions sont appelées par le composant principal Hardware Test.\n\n
 *
 *  Created on: 26 sept. 2024
 *      Author: apajadon
 */
#ifdef MODE_THW

#include <stddef.h>
#include <stdint.h>
#include <THW/thw.h>
#include <THW_core/THW_testHardware_common.h>
#include <main.h>

const char thw_Gpio_menuName[] = "Gpio";


static void thw_Gpio_SpareGpio_PG15_toggle(void);
static void thw_Gpio_SpareGpio_PG15_set(void);
static void thw_Gpio_SpareGpio_PG15_reset(void);

//****************************************
//			*** Main Menu ***
//****************************************
st_thw_menuItem thw_Gpio_menuTab[] = {
		{.name = "SpareGpio_PG15 - Toggle", 	.pActionFn = thw_Gpio_SpareGpio_PG15_toggle, 	.info = 0},
		{.name = "SpareGpio_PG15 - Set", 		.pActionFn = thw_Gpio_SpareGpio_PG15_set, 		.info = 0},
		{.name = "SpareGpio_PG15 - Reset", 	.pActionFn = thw_Gpio_SpareGpio_PG15_reset, 	.info = 0},
};


uint16_t thw_Gpio_menuTabSize = sizeof(thw_Gpio_menuTab) / sizeof(st_thw_menuItem);


void thw_Gpio_DisplayMenu(void);
void thw_Gpio_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_Gpio_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_Gpio_setActive(void)
{
	// Menu affiché
	thw_actualMenu.refreshFn = 		NULL;
	thw_actualMenu.displayMenu = 	thw_Gpio_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_Gpio_ManageChoice;
	thw_actualMenu.clearScreen = 	true;
}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_Gpio_DisplayMenu(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_Gpio_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_Gpio_menuName);
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_Gpio_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_Gpio_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");

}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_Gpio_ManageChoice(char CodeToManage)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_Gpio_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_Gpio_menuTabSize)){
		if(thw_Gpio_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_Gpio_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		// Return to the previous menu
		thw_main_setActive();
	}
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************




//------------------------------------------------------------------------------
/// \fn 		static void thw_Gpio_SpareGpio_PG15_toggle(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_Gpio_SpareGpio_PG15_toggle(void)
{
	HAL_GPIO_TogglePin(SpareGpio_PG15_GPIO_Port, SpareGpio_PG15_Pin);
}

//------------------------------------------------------------------------------
/// \fn 		static void thw_Gpio_SpareGpio_PG15_set(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_Gpio_SpareGpio_PG15_set(void)
{
	HAL_GPIO_WritePin(SpareGpio_PG15_GPIO_Port, SpareGpio_PG15_Pin, GPIO_PIN_SET);
}

//------------------------------------------------------------------------------
/// \fn 		static void thw_Gpio_SpareGpio_PG15_reset(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_Gpio_SpareGpio_PG15_reset(void)
{
	HAL_GPIO_WritePin(SpareGpio_PG15_GPIO_Port, SpareGpio_PG15_Pin, GPIO_PIN_RESET);
}



#endif	// MODE_THW
