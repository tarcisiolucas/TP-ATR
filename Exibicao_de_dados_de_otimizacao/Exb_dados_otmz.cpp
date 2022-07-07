#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include <string>
#include "../Principal/CheckForError.h"

using namespace std;

#define _CHECKERROR	1	//Ativa fun??o CheckForError

#define MSG_SIZE 4
#define MAX_ARQ_SIZE 200

HANDLE hEscEvent;
HANDLE hKeyTEvent;
HANDLE hMutexListHD, hFile;



//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY

HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {
	hEscEvent = OpenEvent(SYNCHRONIZE, FALSE, L"EscEvent");
	hKeyTEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"TEvent");
	hMutexListHD = OpenMutex(SYNCHRONIZE, TRUE, L"hMutexListHD");

	HANDLE Events[2] = { hEscEvent, hKeyTEvent};
	DWORD ret;
	int nTipoEvento;
	bool estado = true;
	LONG lDistanceToMove = 0;
	LONG lDistanceToMoveHigh = 0l;
	DWORD dwBytesEscritos;
	int tamanho_atual = 1;
	char buffer[39];

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 1) {
			/*if (estado) {
				SetConsoleTextAttribute(cout_handle, FOREGROUND_GREEN);
				std::cout << "Exibicao de Dados de Otimizacao DESBLOQUEADA!" << std::endl;
				estado = false;
			}
			else if (!estado) {
				SetConsoleTextAttribute(cout_handle, RED);
				std::cout << "Exibicao de Dados Otimizacao BLOQUEADA!" << std::endl;
				estado = true;
			}*/
			//std::cout << "Exibicao de Dados de Otimizacao DESBLOQUEADA!" << std::endl;
			WaitForSingleObject(hMutexListHD, INFINITE);
			hFile = CreateFile(L"..\\Arquivo_Circular\\ArquivoCircular.txt",
				GENERIC_READ,
				FILE_SHARE_READ,					// abre para leitura e escrita
				NULL,								// atributos de seguran�a 
				OPEN_ALWAYS,						// Sempre abre o arquivo
				FILE_ATTRIBUTE_NORMAL,				// acesso síncrono
				NULL);								// Template para atributos e flags																	// Template para atributos e flags
			//CheckForError(hFile != INVALID_HANDLE_VALUE);


			SetFilePointer(hFile, lDistanceToMove, &lDistanceToMoveHigh, FILE_BEGIN);

			int retornoEscrita = ReadFile(hFile, &buffer, sizeof(buffer), &dwBytesEscritos, NULL);
			lDistanceToMove += sizeof(buffer);

			tamanho_atual++;
			cout << buffer << endl;
			if (tamanho_atual > MAX_ARQ_SIZE)
			{
				lDistanceToMove = 0;
				tamanho_atual = 1;
			}
			ReleaseMutex(hMutexListHD);
			
		}

	} while (nTipoEvento != 0); //Loop ocorre enquanto Esc nao for selecionado
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Otimização terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyTEvent);

	return EXIT_SUCCESS;
}
