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
HANDLE hALMessageR;
HANDLE hALMessageW;

//Pipes
#define BUFSIZEAL 28
HANDLE hPipeAL;
LPCTSTR lpszPipenameAL = L"\\\\.\\pipe\\ALpipe";

//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY

HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {
	hEscEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EscEvent");
	hKeyZEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"ZEvent");
	hKeyLEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, L"LEvent");
	hALMessageR = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"ALMessageR");
	hALMessageW = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"ALMessageW");

	HANDLE Events[3] = { hEscEvent, hKeyLEvent, hKeyZEvent };
	HANDLE Events2[3] = { hEscEvent, hALMessageR };
	DWORD ret, ret2, cbRead;
	int nTipoEvento, nTipoEvento2;
	bool estado = true;
	char buffer[28];
	printf("\ntentando conectar");
	if (!WaitNamedPipe(lpszPipenameAL, 10000))
	{
		printf("Could not open pipe: 20 second wait timed out.");
		return -1;
	}
	hPipeAL = CreateFile(
		lpszPipenameAL,  // pipe name
		GENERIC_READ | // read and write access
		GENERIC_WRITE,
		0,             // no sharing
		NULL,          // default security attributes
		OPEN_EXISTING, // opens existing pipe
		0,             // default attributes
		NULL);
	printf("\nconectado");

	do {
		ret = WaitForMultipleObjects(3, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 1) {
			/*if (estado) {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_GREEN);
				std::cout << "Exibicao de Alarmes DESBLOQUEADA!" << std::endl;
				estado = false;
			}
			else if (!estado) {
				SetConsoleTextAttribute(cout_handle, RED);
				std::cout << "Exibicao de ALARMES BLOQUEADA!" << std::endl;
				estado = true;
			}*/
			printf("OOOOOOOOOOOOOOO");
			ret2 = WaitForMultipleObjects(2, Events2, FALSE, INFINITE);
			nTipoEvento2 = ret2 - WAIT_OBJECT_0;
			if (/*nTipoEvento2 ==*/ 1)
			{
				printf("leitura!!!!!!!!!");
				DWORD fSuccess = ReadFile(
					hPipeAL,                   // pipe handle
					buffer,                   // buffer to receive reply
					BUFSIZEAL, // size of buffer
					&cbRead,                 // number of bytes read
					NULL);                   // not overlapped

				printf("%s\n", buffer);
				ResetEvent(hALMessageR);
				SetEvent(hALMessageW);
			}
		}
		else if (nTipoEvento == 2) {
			system("cls"); //Limpa as mensagens do console
		}

	} while (nTipoEvento != 0); //Loop ocorre enquanto Esc n�o for selecionado
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Alarme terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyLEvent);
	CloseHandle(hKeyZEvent);

	return EXIT_SUCCESS;
}
