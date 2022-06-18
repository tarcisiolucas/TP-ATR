#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include "../Principal/CheckForError.h"

#define _CHECKERROR	1	//Ativa fun??o CheckForError

HANDLE hEscEvent;
HANDLE hKeyTEvent;


//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY

HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {
	hEscEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EscEvent");
	hKeyTEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"TEvent");


	HANDLE Events[2] = { hEscEvent, hKeyTEvent};
	DWORD ret;
	int nTipoEvento;
	int aux = 0;

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 1) {
			if (aux == 0) {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_GREEN);
				std::cout << "Exibicao de Dados Otimização!" << std::endl;
				aux = 1;
			}
			else {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
				std::cout << "Exibicao de Dados Bloqueada!" << std::endl;
				aux = 0;
			}
		}

	} while (nTipoEvento != 0); //Loop ocorre enquanto Esc n�o for selecionado
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Otimização terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyTEvent);

	return EXIT_SUCCESS;
}
