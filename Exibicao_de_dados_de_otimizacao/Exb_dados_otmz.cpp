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
HANDLE hMutexListHD, hListHDRead, hSemaphore;

//pipes
#define BUFSIZEOTM 39
HANDLE  hPipeOTM;
HANDLE hOTMMessageR;
HANDLE hOTMMessageW;
LPCTSTR lpszPipenameOTM = L"\\\\.\\pipe\\OTMpipe";


//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY

HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {
	hEscEvent = OpenEvent(SYNCHRONIZE, FALSE, L"EscEvent");
	hKeyTEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"TEvent");
	hMutexListHD = OpenMutex(EVENT_ALL_ACCESS, TRUE, L"hMutexListHD");
	hListHDRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"hListHDRead");
	hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"ListSemaphore");
	hOTMMessageR = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"OTMMessageR");
	hOTMMessageW = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"OTMMessageW");
	
	HANDLE Events[2] = { hEscEvent, hKeyTEvent};
	HANDLE Events2[2] = { hEscEvent, hOTMMessageR};
	DWORD ret, cbRead;
	int nTipoEvento, nTipoEvento2;
	bool estado = true;
	LONG lDistanceToMove = 0;
	LONG lDistanceToMoveHigh = 0l;
	//DWORD dwBytesEscritos;
	int tamanho_atual = 1;
	char buffer[BUFSIZEOTM];
	char frase[7][9];
	
	printf("EXIBICAO DE DADOS DE OTIMIZACAO:\n");

	if (!WaitNamedPipe(lpszPipenameOTM, 10000))
	{
		printf("Could not open pipe: 20 second wait timed out.");
		return -1;
	}
	hPipeOTM = CreateFile(
		lpszPipenameOTM,  // pipe name
		GENERIC_READ | // read and write access
		GENERIC_WRITE,
		0,             // no sharing
		NULL,          // default security attributes
		OPEN_EXISTING, // opens existing pipe
		0,             // default attributes
		NULL);
	if (!hPipeOTM) printf("nao conectado\n");
	//WaitForMultipleObjects(2, Events2, FALSE, INFINITE);
	/*HANDLE hFile2 = CreateFile(L"..\\Exibicao_de_dados_de_otimizacao\\ArquivoCircular.txt",
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, // abre para leitura e escrita
		NULL,								// atributos de seguran�a 
		OPEN_ALWAYS,						// Sempre abre o arquivo
		FILE_ATTRIBUTE_NORMAL,				// acesso síncrono
		NULL);								// Template para atributos e flags	
	//CheckForError(hFile != INVALID_HANDLE_VALUE);

	if (hFile2 == INVALID_HANDLE_VALUE) printf("\nErro na criação do arquivo\n");
	//cout << hFile2 << endl;
	*/

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		//printf("ntipoenvento = %d", nTipoEvento);
		if (nTipoEvento == 1) {
			ret = WaitForMultipleObjects(2, Events2, FALSE, INFINITE);
			nTipoEvento2 = ret - WAIT_OBJECT_0;
			//printf("ntipoenvento2 = %d", nTipoEvento2);
			if (nTipoEvento2 == 1) {
				/*BOOL status = SetFilePointer(hFile2, lDistanceToMove, &lDistanceToMoveHigh, FILE_BEGIN);
				if (!status) printf("Erro no ponteiro Codigo = %d\n", GetLastError());

				BOOL retornoEscrita = ReadFile(hFile2, &buffer, sizeof(buffer), &dwBytesEscritos, NULL);
				if (!retornoEscrita) printf("erro na leitura Codigo = %d\n", GetLastError());

				lDistanceToMove += sizeof(buffer) + 1;

				tamanho_atual++;
				cout << buffer << endl;
				if (tamanho_atual > MAX_ARQ_SIZE)
				{
					lDistanceToMove = 0;
					tamanho_atual = 1;
				}
				//CloseHandle(hFile2);
				ResetEvent(hListHDRead);*/

				DWORD fSuccess = ReadFile(
					hPipeOTM,                   // pipe handle
					buffer,                   // buffer to receive reply
					sizeof(buffer),			// size of buffer
					&cbRead,                 // number of bytes read
					NULL);                   // not overlapped
				if (fSuccess) {
					int pos = 0;
					int letra = 0;
					for (int i = 0; i < BUFSIZEOTM; i++)
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
					printf("\nNSEQ:%s SP (TEMP):%sC SP (PRE):%spsi VOL:%sm3 %s",
						frase[0], frase[3], frase[2], frase[4], frase[5]);

				}
				ResetEvent(hOTMMessageR);
				//SetEvent(hOTMMessageW);
			}
		}

	} while (nTipoEvento != 0); //Loop ocorre enquanto Esc nao for selecionado
	SetConsoleTextAttribute(cout_handle, FOREGROUND_RED);
	std::cout << "Exibicao de Dados de Otimização terminando...\n";

	CloseHandle(hEscEvent);
	CloseHandle(hKeyTEvent);
	CloseHandle(hMutexListHD);
	CloseHandle(hListHDRead);
	//CloseHandle(hFile2);
	CloseHandle(hSemaphore);
	CloseHandle(hOTMMessageR);

	return EXIT_SUCCESS;
}
