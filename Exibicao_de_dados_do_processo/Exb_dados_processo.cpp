#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include "../Principal/CheckForError.h"

#define _CHECKERROR	1	//Ativa fun??o CheckForError

HANDLE hEscEvent;
HANDLE hKeyREvent;


//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY

HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {
	hEscEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EscEvent");
	hKeyREvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"REvent");


	HANDLE Events[2] = { hEscEvent, hKeyREvent };
	DWORD ret;
	int nTipodeEvento;
	int aux = 0;

	do {
		ret = WaitForMultipleObjects(3, Events, FALSE, INFINITE);
		nTipodeEvento = ret - WAIT_OBJECT_0;
		if (nTipodeEvento == 1) {
			if (aux == 0) {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_GREEN);
				std::cout << "Exibicao de Dados Processo desbloqueada!" << std::endl;
				aux = 1;
			}
			else {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
				std::cout << "Exibicao de Dados Bloqueada!" << std::endl;
				aux = 0;
			}
		}

	} while (nTipodeEvento != 0); //Loop ocorre enquanto Esc n�o for selecionado
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Processo terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyREvent);

	return EXIT_SUCCESS;
}
