/*
 * serial_VT100.h
 *
 *  Created on: 8 févr. 2018
 *      Author: apajadon
 */

#ifndef UTILS_UTILS_VT100_H_
#define UTILS_UTILS_VT100_H_

// VT100 : Actions sur l'ecran d'un terminal
#define VT100_CLEARSCR        "\x1B[2J\x1B[;H"        // Clear SCReen
#define VT100_CLEAREOL        "\x1B[K"                // Clear End Of Line
#define VT100_CLEAREOS        "\x1B[J"                // Clear End Of Screen
#define VT100_CLEARLCR        "\x1B[0K"               // Clear Line Cursor Right
#define VT100_CLEARLCL        "\x1B[1K"               // Clear Line Cursor Left
#define VT100_CLEARELN        "\x1B[2K"               // Clear Entire LiNe
#define VT100_CLEARCDW        "\x1B[0J"               // Clear Curseur DoWn
#define VT100_CLEARCUP        "\x1B[1J"               // Clear Curseur UP
#define VT100_GOTOYX          "\x1B[%.2d;%.2dH"       // Goto at (y,x)
#define VT100_SAVEPOS         "\x1B[s"       		  // Save Cursor Position
#define VT100_RESTOREPOS      "\x1B[u"       		  // Restore Cursor Position

#define VT100_INSERTMOD       "\x1B[4h"               // Mode insertion
#define VT100_OVERWRITEMOD    "\x1B[4l"               // Mode de non insertion
#define VT100_DELAFCURSOR     "\x1B[K"

// VT100 : Actions sur le curseur
#define VT100_CURSON          "\x1B[?25h"             // Curseur visible
#define VT100_CURSOFF         "\x1B[?25l"             // Curseur invisible

// VT100 : Actions sur les caracteres affichables
#define VT100_NORMAL          "\x1B[0m\0"             // Normal/Reset
#define VT100_BOLD            "\x1B[1m"               // Gras
#define VT100_UNDERLINE       "\x1B[4m"               // Souligne
#define VT100_BLINKING        "\x1B[5m"               // Clignotant
#define VT100_INVVIDEO        "\x1B[7m"               // Inverse video

// VT100 : Color
#define VT100_COLOR_RESET	"\x1B[0m"

// Couleurs caracteres
#define VT100_FG_LIGHTRED	"\x1B[1;31m"	// Caractere Rouge vif
#define VT100_FG_LIGHTGREEN	"\x1B[1;32m"	// Caractere Vert vif
#define VT100_FG_YELLOW		"\x1B[1;33m"	// Caractere Jaune vif
#define VT100_FG_LIGHTBLUE	"\x1B[1;34m"	// Caractere Bleu vif
#define VT100_FG_MAGENTA	"\x1B[1;35m"	// Caractere Magenta
#define VT100_FG_LIGHTCYAN	"\x1B[1;36m"	// Caractere Cyan vif
#define VT100_FG_WHITE		"\x1B[1;37m"	// Caractere Blanc

#define VT100_FG_RED		"\x1B[0;31m"	// Caractere Rouge
#define VT100_FG_GREEN		"\x1B[0;32m"	// Caractere Vert
#define VT100_FG_BROWN		"\x1B[0;33m"	// Caractere Marron
#define VT100_FG_BLUE		"\x1B[0;34m"	// Caractere Bleu
#define VT100_FG_PINK		"\x1B[0;35m"	// Caractere Rose
#define VT100_FG_CYAN		"\x1B[0;36m"	// Caractere Cyan
#define VT100_FG_GREY		"\x1B[0;37m"	// Caractere Gris

// Couleurs fond
#define VT100_BG_RED	"\x1B[41m"				// Fond Rouge
#define VT100_BG_GREEN	"\x1B[42m"				// Fond Vert
#define VT100_BG_YELLOW	"\x1B[43m"				// Fond Jaune
#define VT100_BG_WHITE	"\x1B[47m"				// Fond Blanc



#endif /* UTILS_UTILS_VT100_H_ */
