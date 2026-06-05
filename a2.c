/*
 * tresenratlla.c
 * 
 * Copyright 2026 jocsmeus <jocsmeus420@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

inline int max(int a, int b)
{
	return a > b ? a : b;
}

void imprimir_ajuda (char *nom_programa)
{
	printf("Forma d'ús:\n"
	       "  %s [OPCIÓ...]\n"
	       "Un programa del tres en ratlla\n\n"
	       "Opcions de l'aplicació:\n"
	       "  O       El jugador té la peça O i comença el programa\n"
	       "  X       El jugador té la peça X i comença el jugador\n",
	       nom_programa);
}

typedef struct
{
    unsigned Ocupades;
    unsigned Jugador;
    unsigned Torn;
} Posicio;

Posicio Tauler;


void moure(unsigned pos)
{
	Tauler.Ocupades |= pos;
	Tauler.Jugador  |= pos & -Tauler.Torn;
	Tauler.Torn      = !Tauler.Torn;
}

void treure(unsigned pos)
{
	Tauler.Torn      = !Tauler.Torn;
	Tauler.Ocupades &= ~pos;
	Tauler.Jugador  &= ~pos;
}

_Bool alineades()
{
	unsigned
	Peces  = Tauler.Jugador;
	Peces ^= -Tauler.Torn;
	Peces &= Tauler.Ocupades;
	
	return (   ((Peces >> 1) & (Peces >> 2))
	         | ((Peces >> 4) & (Peces >> 8))
	         | ((Peces >> 5) & (Peces >> 10))
	         | ((Peces >> 3) & (Peces >> 6))) & Peces;
}

int negamax(int jugada)
{
	if (alineades())
		return -1;
	
	if (jugada >= 8)
		return 0;
	
	int eval = -1;
	unsigned movs = ~Tauler.Ocupades & 0x777;
	
	while (movs)
	{
		unsigned
		copia  = movs;
		movs  &= movs - 1; // treure el bit
		copia ^= movs;     // l'últim bit

		moure (copia);
		eval   = max(eval, -negamax (jugada+1));
		treure (copia);
	}
	
	return eval;
}

unsigned jugades_fetes = 0;

void jugar(void)
{
	unsigned movs = ~Tauler.Ocupades & 0x777;
	unsigned opcions[10];
	unsigned idx = 0;
	int millor = -1;
	
	while (movs)
	{
		unsigned
		copia  = movs;
		movs  &= movs - 1; // treure el bit
		copia ^= movs;     // l'últim bit

		moure (copia);
		int eval = -negamax (jugades_fetes+1);
		treure (copia);
		
		if (eval == millor)
			opcions[idx++] = copia;
		
		if (eval > millor)
			millor = eval,
			opcions[idx = 0] = copia,
			idx++;
	}
	
	if (!idx)
		return;
	
	moure (opcions[rand() % idx]);
	++jugades_fetes;
}

unsigned de_posicio_a_indx(int a)
{
	const unsigned transformar[9] = {2,6,10,1,5,9,0,4,8};
	return transformar[a];
}

_Bool color(unsigned pos)
{
	return Tauler.Jugador & (1 << pos);
}

_Bool ocupada(unsigned pos)
{
	return Tauler.Ocupades & (1 << de_posicio_a_indx(pos));
}

void imprimir()
{	
	static const char taula_caractrers[2][2] = {{'.', '.'},{'O', 'X'}};
	printf(" +---+---+---+\n");
	for (unsigned i = 0; i < 9; i++)
	{
		int j = de_posicio_a_indx(i);
		printf (" | %c", taula_caractrers[ocupada(i)][color(j)]);
		
		if (i == 2 || i == 5 || i == 8)
			printf(" |\n +---+---+---+\n");
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	if (argc <= 1 || (argv[1][0] != 'O' && argv[1][0] != 'X'))
	{
		if (argc == 1)
			imprimir_ajuda(argv[0]);
		goto acabar;
	}
	
	srand(time(NULL));
	
	printf("Les caselles:\n"
	       " +---+---+---+\n"
	       " | 0 | 1 | 2 |\n"
	       " +---+---+---+\n"
	       " | 3 | 4 | 5 |\n"
	       " +---+---+---+\n"
	       " | 6 | 7 | 8 |\n"
	       " +---+---+---+\n\n");
	
	if (argv[1][0] == 'O')
		jugar();
	
	imprimir();
	
	while (!alineades())
	{
		if (jugades_fetes >= 8)
			goto taules;
		
		int moviment;
		_Bool llegit_correctament = 1;
		do
		{
			llegit_correctament = scanf("%d", &moviment) == 1;
		}while (llegit_correctament
		        && (moviment < 0 || moviment > 8 || ocupada(moviment)));

		if (!llegit_correctament)
			goto acabar;
		
		moure (1 << de_posicio_a_indx(moviment));
		imprimir();
		
		if (++jugades_fetes > 8)
			goto taules;
		
		jugar();
		imprimir();
	}
	
	printf("Has perdut\n");
	return 0;
	
taules:
	printf("Empat\n");
	return 0;
	
acabar:
	return 1;
}
