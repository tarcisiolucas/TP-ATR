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
	HANDLE Events2[2] = { hEscEvent, hALMessageR };
	DWORD ret, ret2, cbRead;
	int nTipoEvento; //nTipoEvento2;
	bool estado = true;
	char buffer[28];
	char frase[6][9];

	printf("EXIBICAO DE ALARMES\n\n");
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
	if (!hPipeAL) printf("nao conectado\n");

	do {
		ret = WaitForMultipleObjects(3, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 1) {
			ret2 = WaitForMultipleObjects(2, Events2, FALSE, INFINITE);
			DWORD fSuccess = ReadFile(
				hPipeAL,                 // pipe handle
				buffer,                  // buffer to receive reply
				BUFSIZEAL,				 // size of buffer
				&cbRead,                 // number of bytes read
				NULL);                   // not overlapped

			int pos = 0;
			int letra = 0;
			for (int i = 0; i < BUFSIZEAL; i++)
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

			if (buffer[13] == '1') {
				printf("\n%s NSEQ:%s PRESSAO ELEVADA NA TUBULACAO   PRI:%s",
					frase[4], frase[0], frase[3]);

			}
			else if (buffer[13] == '2') {
				printf("\n%s NSEQ:%s PRESSAO BAIXA NA TUBULACAO     PRI:%s",
					frase[4], frase[0], frase[3]);

			}
			else if (buffer[13] == '3') {
				printf("\n%s NSEQ:%s BAIXO VOLUME INTERIOR DO POCO  PRI:%s",
					frase[4], frase[0], frase[3]);

			}
			else if (buffer[13] == '4') {
				printf("\n%s NSEQ:%s VOLUME ELEVADO INTERIOR POCO   PRI:%s",
					frase[4], frase[0], frase[3]);
			}
			else if (buffer[13] == '5') {
				printf("\n%s NSEQ:%s PEPTROLEO SUPERAQUECIDO TUB    PRI:%s",
					frase[4], frase[0], frase[3]);
			}
			else if (buffer[13] == '6') {
				printf("\n%s NSEQ:%s GERADOR PRINCIPAL COM DEFEITO  PRI:%s",
					frase[4], frase[0], frase[3]);
			}
			else if (buffer[13] == '7') {
				printf("\n%s NSEQ:%s BAIXA VAZAO TUBULACAO PRINC    PRI:%s",
					frase[4], frase[0], frase[3]);
			}
			else if (buffer[13] == '8') {
				printf("\n%s NSEQ:%s ALTA VISCOSIDADE DO SUBSTRATO  PRI:%s",
					frase[4], frase[0], frase[3]);
			}
			else if (buffer[13] == '9') {
				printf("\n%s NSEQ:%s BAIXA PRESSAO GAS DE INJECAO   PRI:%s",
					frase[4], frase[0], frase[3]);
			}
			else{
				printf("\n%s NSEQ:%s BOMBA DE GAS COM DEFEITO       PRI:%s",
					frase[4], frase[0], frase[3]);
			}

			ResetEvent(hALMessageR);
			SetEvent(hALMessageW);
		
		}
		else if (nTipoEvento == 2) {
			system("cls"); //Limpa as mensagens do console
			printf("EXIBICAO DE ALARMES\n\n");
			SetEvent(hKeyLEvent);
		}

	} while (nTipoEvento != 0); //Loop ocorre enquanto Esc n�o for selecionado
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Alarme terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyLEvent);
	CloseHandle(hKeyZEvent);

	return EXIT_SUCCESS;
}
