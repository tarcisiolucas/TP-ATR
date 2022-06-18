#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include "../Principal/CheckForError.h"

#define _CHECKERROR	1	//Ativa fun??o CheckForError

HANDLE hEscEvent;
HANDLE hKeyZEvent;
HANDLE hKeyLEvent;

//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY

HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {
	hEscEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EscEvent");
	hKeyZEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"ZEvent");
	hKeyLEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"LEvent");

	HANDLE Events[3] = { hEscEvent, hKeyLEvent, hKeyZEvent };
	DWORD ret;
	int nTipoEvento;
	int aux = 0;

	do {
		ret = WaitForMultipleObjects(3, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 1) {
			if (aux == 0) {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_GREEN);
				std::cout << "Exibicao de Dados Alarmes!" << std::endl;
				aux = 1;
			}
			else {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
				std::cout << "Exibicao de Dados Bloqueada!" << std::endl;
				aux = 0;
			}
		}
		else if (nTipoEvento == 2) {
			system("cls"); //Limpa as mensagens da janela
		}

	} while (nTipoEvento != 0); //Loop ocorre enquanto Esc n�o for selecionado
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Alarme terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyLEvent);
	CloseHandle(hKeyZEvent);

	return EXIT_SUCCESS;
}
