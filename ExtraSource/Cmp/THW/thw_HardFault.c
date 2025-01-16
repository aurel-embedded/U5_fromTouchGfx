/*
 * thw_HardFault.c
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
#include "APP.h"

const char thw_HardFault_menuName[] = "HardFault";


static void thw_HardFault_invalidMemoryAccess(void);
static void thw_HardFault_stackOverflow(void);
static void thw_HardFault_illegalInstruction(void);

//****************************************
//			*** Main Menu ***
//****************************************
st_thw_menuItem thw_HardFault_menuTab[] = {
		{.name = "Invalid Memory Access", 	.pActionFn = thw_HardFault_invalidMemoryAccess, 	.info = 0},
		{.name = "Stack Overflow", 			.pActionFn = thw_HardFault_stackOverflow, 			.info = 0},
		{.name = "Illegal Instruction",		.pActionFn = thw_HardFault_illegalInstruction, 		.info = 0},
};


uint16_t thw_HardFault_menuTabSize = sizeof(thw_HardFault_menuTab) / sizeof(st_thw_menuItem);


void thw_HardFault_DisplayMenu(void);
void thw_HardFault_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_HardFault_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_HardFault_setActive(void)
{
	// Menu affiché
	thw_actualMenu.refreshFn = 		NULL;
	thw_actualMenu.displayMenu = 	thw_HardFault_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_HardFault_ManageChoice;
	thw_actualMenu.clearScreen = 	true;
}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_HardFault_DisplayMenu(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_HardFault_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_HardFault_menuName);
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_HardFault_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_HardFault_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");

}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_HardFault_ManageChoice(char CodeToManage)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_HardFault_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_HardFault_menuTabSize)){
		if(thw_HardFault_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_HardFault_menuTab[CodeToManage - 1].pActionFn();
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
/// \fn 		void thw_HardFault_invalidMemoryAccess(void)
/// \brief		Accéder à une adresse mémoire qui n'est pas mappée sur le STM32
///				provoque généralement un HardFault.
///				Par exemple, essayer d'accéder à une adresse en dehors des
///				plages de la RAM ou de la mémoire externe.
///
///				- Effet attendu : Un HardFault sera déclenché parce que l'adresse
///									0xFFFFFFFF n'est pas mappée sur le STM32.
///				- Registres à analyser :
///						- Le PC indiquera l'adresse où le programme a tenté de
///							lire cette adresse.
///						- Le LR contiendra l'adresse de retour avant l'erreur.
///						- Les registres généraux contiendront des informations
///							sur les données traitées avant l'erreur.
//------------------------------------------------------------------------------
static void thw_HardFault_invalidMemoryAccess(void)
{
    // Accès à une adresse mémoire invalide
    volatile uint32_t *invalid_address = (uint32_t*)0xFFFFFFFF;

    // Lecture de l'adresse invalide
    uint32_t value = *invalid_address;
    UNUSED(value);

    // Boucle infinie pour attendre le HardFault
    while (1);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_HardFault_stackOverflow(void)
/// \brief		Dépasser les limites de la pile peut également provoquer un
///				HardFault, en particulier si le programme corrompt le pointeur
///				de pile ou écrase d'autres sections de mémoire.
///
///				- Effet attendu : Si la pile est trop petite pour stocker le
///									tableau, un HardFault sera déclenché.
///				- Registres à analyser :
///						- Le SP (Stack Pointer) vous montrera l'adresse où la pile a dépassé sa limite.
///						- Le PC pointera vers l'instruction qui a causé l'écrasement.
///						- Le LR pourrait montrer des signes de corruption si le
///							débordement a écrasé la zone de la pile où il est stocké.
//------------------------------------------------------------------------------
static void thw_HardFault_stackOverflow(void)
{
	// Déclare un grand tableau local qui dépasse la taille de la pile
	uint32_t large_array[1000000];
	UNUSED(large_array);

	// Remplissage du tableau pour simuler un débordement
	for (int i = 0; i < 1000000; i++) {
		large_array[i] = i;
	}

	// Boucle infinie pour attendre le HardFault
	while (1);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_HardFault_illegalInstruction(void)
/// \brief		Exécuter une instruction illégale ou non alignée (par exemple,
///					un saut vers une adresse qui ne contient pas de code valide)
///					provoquera un HardFault.
///
///				- Effet attendu : Le microcontrôleur va essayer d'exécuter du
///									code à partir de 0x20000000 (RAM), qui n'est
///									pas une zone où le code est stocké, ce qui
///									déclenchera un HardFault.
///				- Registres à analyser :
///						- Le PC pointera vers l'adresse de la RAM où l'instruction illégale a été tentée.
///						- Le LR indiquera l'adresse de retour avant l'erreur.
///						- Les registres contiendront des informations sur les données préparées pour l'instruction.
//------------------------------------------------------------------------------
static void thw_HardFault_illegalInstruction(void)
{
	// Déclare un pointeur de fonction
	void (*invalid_function)(void);

	// Pointeur vers une adresse invalide qui ne contient pas de code valide
	invalid_function = (void (*)(void))0x20000000;  // Adresse alignée sur la RAM

	// Appel de la fonction à partir de cette adresse
	invalid_function();

	// Boucle infinie pour attendre le HardFault
	while (1);
}


#endif	// MODE_THW
