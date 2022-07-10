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
HANDLE hPROMessageR;
HANDLE hPROMessageW;

//Pipes
#define BUFSIZEPRO 47
HANDLE hPipePRO;
LPCTSTR lpszPipenamePRO = L"\\\\.\\pipe\\PROpipe";

//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY

HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {
	hEscEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EscEvent");
	hKeyREvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"REvent");
	hPROMessageR = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"PROMessageR");
	hPROMessageW = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"PROMessageW");


	HANDLE Events[2] = { hEscEvent, hKeyREvent };
	HANDLE Events2[3] = { hEscEvent, hPROMessageR };
	DWORD ret, ret2, cbRead;
	int nTipodeEvento;// nTipoEvento2;
	bool estado = true;
	char buffer[BUFSIZEPRO];
	char frase[7][9];
	printf("EXIBICAO DE DADOS DE PROCESSO:\n");
	if (!WaitNamedPipe(lpszPipenamePRO, 10000))
	{
		printf("Could not open pipe: 20 second wait timed out.");
		return -1;
	}
	hPipePRO = CreateFile(
		lpszPipenamePRO,  // pipe name
		GENERIC_READ | // read and write access
		GENERIC_WRITE,
		0,             // no sharing
		NULL,          // default security attributes
		OPEN_EXISTING, // opens existing pipe
		0,             // default attributes
		NULL);
	if(!hPipePRO) printf("nao conectado\n");


	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipodeEvento = ret - WAIT_OBJECT_0;
		if (nTipodeEvento == 1) {
			ret2 = WaitForMultipleObjects(2, Events2, FALSE, INFINITE);
				DWORD fSuccess = ReadFile(
					hPipePRO,                   // pipe handle
					buffer,                   // buffer to receive reply
					BUFSIZEPRO, // size of buffer
					&cbRead,                 // number of bytes read
					NULL);                   // not overlapped
				if (fSuccess) {
					int pos = 0;
					int letra = 0;
					for (int i = 0; i < BUFSIZEPRO; i++)
					{
						if (buffer[i] == '|')
						{
							frase[pos][letra] = '\0';
							letra = 0;
							i++;
							pos++;
						}
						frase[pos][letra] = buffer[i];
						letra++;
					}
					SetConsoleTextAttribute(cout_handle, BLUE);
					printf("\n%s NSEQ:%s PR (T):%spsi TEMP:%sC PR (G):%spsi NIVEL:%scm",
						frase[6], frase[0], frase[2], frase[3], frase[4], frase[5]);

				}
				ResetEvent(hPROMessageR);
				SetEvent(hPROMessageW);
			
		}
	} while (nTipodeEvento != 0); //Loop ocorre enquanto Esc n�o for selecionado
	
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Processo terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyREvent);

	return EXIT_SUCCESS;
}
