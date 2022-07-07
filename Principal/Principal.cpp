#define WIN32_LEAN_AND_MEAN 
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <process.h>	// _beginthreadex() e _endthreadex() 
#include <conio.h>		// _getch
#include <iostream>
#include <string.h>
#include <string>
#include <time.h>
#include <ctime>
#include <random>
#include <iomanip>
#include <sys/timeb.h>

using namespace std;

//declarando funcoes
DWORD WINAPI ComunicacaoDeDadosOtimizacao(LPVOID id);
DWORD WINAPI ComunicacaoDeDadosProcessos(LPVOID id);
DWORD WINAPI ComunicacaoDeDadosAlarme(LPVOID id);
DWORD WINAPI RetiraDadosOtimizacao(LPVOID id);
DWORD WINAPI RetiraDadosProcesso(LPVOID id);
DWORD WINAPI RetiraAlarme(LPVOID id);
string GeraMensagemAlarme();
string GeraMensagemDadosProcesso();
string GeraMensagemDadosOTM();


#define _CHECKERROR	1
#include "CheckForError.h"

//Adicionando lista circular
#include "Lista_circular.h"
#define SIZE_LIST 100
ListaCircular lista = ListaCircular(SIZE_LIST);
int index = 0;

//Casting para terceiro e sexto par�metros da fun��o _beginthreadex
typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

//Definindo as teclas
#define	ESC		0x1B
#define keyC	0x63
#define keyO	0x6F
#define keyP	0x70
#define keyA	0x61
#define keyT	0x74
#define keyR	0x72
#define keyL	0x6C
#define keyZ	0x7A

//Arquivo circular em disco 
#define MAX_ARQ_SIZE 200
#define MSG_SIZE 4
typedef struct {
	string mensagem_otm;
} BufferOTM;
HANDLE hMutexListHD, hSection;
int* count_arq_circ = 0;

//Handles
HANDLE hEscEvent, hKeyCEvent, hKeyOEvent, hKeyPEvent, hKeyAEvent, hKeyTEvent, 
		hKeyREvent, hKeyLEvent, hKeyZEvent, hFullListEvent, hTimeEvent, hFile, hALMessageR, hALMessageW;
HANDLE hMutex; //Handles para o mutex das listas circulares

// Pipes
#define BUFSIZEOTM 39
#define BUFSIZEAL 28
#define BUFSIZEPRO 47

HANDLE hPipeOTM, hPipeAL, hPipePRO;
LPCTSTR lpszPipenameOTM = L"\\\\.\\pipe\\OTMpipe";
LPCTSTR lpszPipenameAL = L"\\\\.\\pipe\\ALpipe";
LPCTSTR lpszPipenamePRO = L"\\\\.\\pipe\\PROpipe";

//Define cores de texto
#define WHITE  FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define BLUE   FOREGROUND_BLUE| FOREGROUND_INTENSITY
#define RED    FOREGROUND_RED   | FOREGROUND_INTENSITY
#define GREEN    FOREGROUND_GREEN   | FOREGROUND_INTENSITY
HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

int main() {

	//Criando Processos
	bool bStatus;
	PROCESS_INFORMATION NewProcess; // Informações sobre novo processo criado
	STARTUPINFO si; // StartUpInformation para novo processo

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	bStatus = CreateProcess(
		L"..\\x64\\Debug\\Exibicao_de_dados_do_processo.exe",
		NULL,	// linha de comando
		NULL,	// atributos de seguran�a: Processo
		NULL,	// atributos de seguran�a: Thread
		FALSE,	// heran�a de handles
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,	// CreationFlags
		NULL,	// lpEnvironment
		L"..\\x64\\Debug",
		&si,			// lpStartUpInfo
		&NewProcess);	// lpProcessInformation
	if (!bStatus) printf("Erro na criacao do processo! Codigo = %d\n", GetLastError());

	bStatus = CreateProcess(
		L"..\\x64\\Debug\\Exibicao_de_dados_de_otimizacao.exe",
		NULL,	// linha de comando
		NULL,	// atributos de seguran�a: Processo
		NULL,	// atributos de seguran�a: Thread
		FALSE,	// heran�a de handles
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,	// CreationFlags
		NULL,	// lpEnvironment
		L"..\\x64\\Debug",
		&si,			// lpStartUpInfo
		&NewProcess);	// lpProcessInformation
	if (!bStatus) printf("Erro na criacao do processo! Codigo = %d\n", GetLastError());

	bStatus = CreateProcess(
		L"..\\x64\\Debug\\Exibicao_de_alarmes.exe",
		NULL,	// linha de comando
		NULL,	// atributos de seguran�a: Processo
		NULL,	// atributos de seguran�a: Thread
		FALSE,	// heran�a de handles
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,	// CreationFlags
		NULL,	// lpEnvironment
		L"..\\x64\\Debug",
		&si,			// lpStartUpInfo
		&NewProcess);	// lpProcessInformation
	if (!bStatus) printf("Erro na criacao do processo! Codigo = %d\n", GetLastError());

	//Criando Eventos

	hEscEvent = CreateEvent(NULL, TRUE, FALSE, L"EscEvent");				//Cria evento para a tecla ESC
	//CheckForError(hEscEvent);
	if (!hEscEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyCEvent = CreateEvent(NULL, TRUE, TRUE, L"CEvent");					//Cria evento para a tecla C
	//CheckForError(hKeyCEvent);
	if (!hKeyCEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyOEvent = CreateEvent(NULL, TRUE, TRUE, L"OEvent");					//Cria evento para a tecla L
	//CheckForError(hKeyOEvent);
	if (!hKeyOEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyPEvent = CreateEvent(NULL, TRUE, TRUE, L"PEvent");					//Cria evento para a tecla P
	//CheckForError(hKeyPEvent);
	if (!hKeyPEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyAEvent = CreateEvent(NULL, TRUE, TRUE, L"AEvent");					//Cria evento para a tecla A
	//CheckForError(hKeyAEvent);
	if (!hKeyAEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyTEvent = CreateEvent(NULL, TRUE, TRUE, L"TEvent");					//Cria evento para a tecla T
	//CheckForError(hKeyTEvent);
	if (!hKeyTEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyREvent = CreateEvent(NULL, FALSE, TRUE, L"REvent");					//Cria evento para a tecla R
	//CheckForError(hKeyREvent);
	if (!hKeyREvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyLEvent = CreateEvent(NULL, TRUE, TRUE, L"LEvent");					//Cria evento para a tecla L
	//CheckForError(hKeyLEvent);
	if (!hKeyLEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hKeyZEvent = CreateEvent(NULL, FALSE, FALSE, L"ZEvent");				//Cria evento para a tecla Z
	//CheckForError(hKeyZEvent);
	if (!hKeyZEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hFullListEvent = CreateEvent(NULL, TRUE, FALSE, L"FullListEvent");				//Cria evento para a lista cheia
	//CheckForError(hKeyZEvent);
	if (!hKeyZEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	//CheckForError(hKeyZEvent);
	hTimeEvent = CreateEvent(NULL, TRUE, FALSE, L"TimeEvent");
	if (!hTimeEvent) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hALMessageR = CreateEvent(NULL, TRUE, FALSE, L"ALMessageR");
	if (!hALMessageR) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());
	hALMessageW = CreateEvent(NULL, TRUE, TRUE, L"ALMessageW");
	if (!hALMessageW) printf("Erro na criacao do evento! Codigo = %d\n", GetLastError());


	bool estado_hKeyCEvent = true;
	bool estado_hKeyOEvent = true;
	bool estado_hKeyPEvent = true;
	bool estado_hKeyAEvent = true;
	int contTEvent = 0;
	int contREvent = 0;
	int contLEvent = 0;


	//Criando mutex
	hMutex = CreateMutex(NULL, FALSE, L"Mutex");
	if (!hMutex) printf("Erro na criacao do Mutex! Codigo = %d\n", GetLastError());
	//CheckForError(hMutex);
	hMutexListHD = CreateMutex(NULL, FALSE, L"hMutexListHD");
	if (!hMutexListHD) printf("Erro na criacao do Mutex! Codigo = %d\n", GetLastError());
	//CheckForError(hMutex);

	//Handle para as threads
	HANDLE comunicacao_alarme, comunicacao_processos, comunicacao_otimizacao, retira_otimizacao, retira_processo, retira_alarme;

	//criação das threads
	DWORD dwThreadId;
	DWORD dwExitCode = 0;
	DWORD dwRet;
	int nTecla;


	comunicacao_processos = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ComunicacaoDeDadosProcessos,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	if (!comunicacao_processos) printf("Erro na criacao da thread! Codigo = %d\n", GetLastError());

	comunicacao_alarme = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ComunicacaoDeDadosAlarme,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	if (!comunicacao_alarme) printf("Erro na criacao da thread! Codigo = %d\n", GetLastError());

	comunicacao_otimizacao = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ComunicacaoDeDadosOtimizacao,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	if (!comunicacao_otimizacao) printf("Erro na criacao da thread! Codigo = %d\n", GetLastError());

	retira_otimizacao = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)RetiraDadosOtimizacao,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	if (!retira_otimizacao) printf("Erro na criacao da thread! Codigo = %d\n", GetLastError());

	retira_processo = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)RetiraDadosProcesso,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	if (!retira_processo) printf("Erro na criacao da thread! Codigo = %d\n", GetLastError());

	retira_alarme = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)RetiraAlarme,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	if (!retira_alarme) printf("Erro na criacao da thread! Codigo = %d\n", GetLastError());

	

	

	hPipePRO = CreateNamedPipe(
		lpszPipenamePRO,               // pipe name
		PIPE_ACCESS_DUPLEX,         // read/write access
		PIPE_TYPE_MESSAGE |         // message type pipe
		PIPE_READMODE_MESSAGE | // message-read mode
		PIPE_WAIT,              // blocking mode
		PIPE_UNLIMITED_INSTANCES,   // max. instances
		BUFSIZEPRO,                    // output buffer size
		BUFSIZEPRO,                    // input buffer size
		1000,                          // client time-out
		NULL);


	do {
		nTecla = _getch();
		if (estado_hKeyCEvent == true && nTecla == keyC) {
			ResetEvent(hKeyCEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Comunicacao de Dados Bloqueada\n");
			estado_hKeyCEvent = false;
		}
		else if (estado_hKeyCEvent == false && nTecla == keyC) {
			SetEvent(hKeyCEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Comunicacao de Dados Desbloqueada\n");
			estado_hKeyCEvent = true;
		}
		else if (nTecla == keyO && estado_hKeyOEvent == true) {
			ResetEvent(hKeyOEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Retirada de Dados de Otimizacao Bloqueada\n");
			estado_hKeyOEvent = false;
		}
		else if (nTecla == keyO && estado_hKeyOEvent == false) {
			SetEvent(hKeyOEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Retirada de Dados de Otimizacao Desbloqueada\n");
			estado_hKeyOEvent = true;
		}
		else if (nTecla == keyP && estado_hKeyPEvent == true) {
			ResetEvent(hKeyPEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Retirada de Dados de Processo Bloqueada\n");
			estado_hKeyPEvent = false;
		}
		else if (nTecla == keyP && estado_hKeyPEvent == false) {
			SetEvent(hKeyPEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Retirada de Dados de Processo Desbloqueada\n");
			estado_hKeyPEvent = true;
		}
		else if (nTecla == keyA && estado_hKeyAEvent == true) {
			ResetEvent(hKeyAEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Retirada de Alarmes Bloqueada\n");
			estado_hKeyAEvent = false;
		}
		else if (nTecla == keyA && estado_hKeyAEvent == false) {
			SetEvent(hKeyAEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Retirada de Alarmes Desbloqueada\n");
			estado_hKeyAEvent = true;
		}
		else if (nTecla == keyT) {
			if ((contTEvent % 2) == 0) {
				ResetEvent(hKeyTEvent);
				SetConsoleTextAttribute(cout_handle, RED);
				printf("Exibicao de dados de otimizacao BLOQUEADA\n");
			}
			else {
				SetEvent(hKeyTEvent);
				SetConsoleTextAttribute(cout_handle, GREEN);
				printf("Exibicao de dados de otimizacao DESBLOQUEADA\n");
			}
			contTEvent++;
		}
		else if (nTecla == keyR) {
			SetEvent(hKeyREvent);
			if ((contREvent % 2) == 0) {
				SetConsoleTextAttribute(cout_handle, RED);
				printf("Exibicao de dados de processo BLOQUEADA\n");
			}
			else {
				SetConsoleTextAttribute(cout_handle, GREEN);
				printf("Exibicao de dados de processo DESBLOQUEADA\n");
			}
			contREvent++;
		}
		else if (nTecla == keyL) {
			if ((contLEvent % 2) == 0) {
				ResetEvent(hKeyLEvent);
				SetConsoleTextAttribute(cout_handle, RED);
				printf("Exibicao de dados de alarme BLOQUEADA\n");
			}
			else {
				SetEvent(hKeyLEvent);
				SetConsoleTextAttribute(cout_handle, GREEN);
				printf("Exibicao de dados de alarme DESBLOQUEADA\n");
			}
			contLEvent++;
		}
		else if (nTecla == keyZ) {
			SetEvent(hKeyZEvent);
		}
		else if (nTecla == ESC) {
			ResetEvent(hKeyCEvent);
			ResetEvent(hKeyOEvent);
			ResetEvent(hKeyPEvent);
			ResetEvent(hKeyAEvent);
			ResetEvent(hKeyTEvent);
			ResetEvent(hKeyREvent);
			ResetEvent(hKeyLEvent);
			ResetEvent(hKeyZEvent);
			SetEvent(hEscEvent);
		}
	} while (nTecla != ESC);

	//Espera todas as threads terminarem
	HANDLE hThreads[6] = { comunicacao_processos,comunicacao_alarme,comunicacao_otimizacao, retira_otimizacao, retira_processo, retira_alarme };
	dwRet = WaitForMultipleObjects(6, hThreads, TRUE, INFINITE);
	//CheckForError(dwRet == WAIT_OBJECT_0);

	for (int i = 0; i < 6; ++i) {
		GetExitCodeThread(hThreads[i], &dwExitCode);
		CloseHandle(hThreads[i]);
	}

	CloseHandle(hKeyCEvent);
	CloseHandle(hKeyOEvent);
	CloseHandle(hKeyPEvent);
	CloseHandle(hKeyAEvent);
	CloseHandle(hKeyTEvent);
	CloseHandle(hKeyREvent);
	CloseHandle(hKeyLEvent);
	CloseHandle(hKeyZEvent);
	CloseHandle(hEscEvent);

	return EXIT_SUCCESS;
}


DWORD WINAPI ComunicacaoDeDadosAlarme(LPVOID id)
{

	HANDLE Events[2] = { hKeyCEvent, hEscEvent };
	HANDLE EventsR[2] = { hFullListEvent, hEscEvent };
	DWORD ret;
	int nTipoEvento;
	std::string mensagem;
	float timet;


	do {

		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;


		if (nTipoEvento == 0) {

			timet = (rand() % 4000) + 1000;

			WaitForSingleObject(hTimeEvent, (DWORD)timet);

			WaitForSingleObject(hMutex, INFINITE);
			mensagem = GeraMensagemAlarme();
			if (lista.push(mensagem) == false)
			{
				SetConsoleTextAttribute(cout_handle, RED);
				printf("Lista Cheia\n");
				ReleaseMutex(hMutex);
				ResetEvent(hFullListEvent);
				ret = WaitForMultipleObjects(2, EventsR, FALSE, INFINITE);
				if (nTipoEvento == 1)
				{						//Tecla Esc foi escolhida
					break;
				}
			}
			else {
				SetConsoleTextAttribute(cout_handle, WHITE);
				std::cout << "Mensagem: " << mensagem << " de alarme armazenada\n";
				ReleaseMutex(hMutex);
			}
		}
	} while (nTipoEvento == 0);
	printf("Thread ComunicacaoDeDados terminando...\n");

	_endthreadex(0);
	return 0;


}


DWORD WINAPI ComunicacaoDeDadosProcessos(LPVOID id)
{


	HANDLE Events[2] = { hKeyCEvent, hEscEvent };
	HANDLE EventsR[2] = { hFullListEvent, hEscEvent };
	DWORD ret;
	int nTipoEvento;
	std::string mensagem;
	DWORD TimeProce = 500;


	do {

		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;

		if (nTipoEvento == 0)
		{

			WaitForSingleObject(hTimeEvent, TimeProce);

			WaitForSingleObject(hMutex, INFINITE);
			mensagem = GeraMensagemDadosProcesso();
			if (lista.push(mensagem) == false)
			{
				SetConsoleTextAttribute(cout_handle, RED);
				printf("Lista Cheia\n");
				ReleaseMutex(hMutex);
				ResetEvent(hFullListEvent);
				ret = WaitForMultipleObjects(2, EventsR, FALSE, INFINITE);
				if (nTipoEvento == 1)
				{						//Tecla Esc foi escolhida
					break;
				}
			}
			else
			{
				SetConsoleTextAttribute(cout_handle, WHITE);
				std::cout << "Mensagem: " << mensagem << " de dados processo\n";
				ReleaseMutex(hMutex);
			}

		}
	} while (nTipoEvento == 0);
	printf("Thread ComunicacaoDeDados terminando...\n");

	_endthreadex(0);
	return 0;



}

//Tarefa de comunicação de dados
DWORD WINAPI ComunicacaoDeDadosOtimizacao(LPVOID id)
{

	HANDLE Events[2] = { hKeyCEvent, hEscEvent };
	HANDLE EventsR[2] = { hFullListEvent, hEscEvent };
	DWORD ret;
	int nTipoEvento;
	std::string mensagem;
	float timeotimizacao;

	do {

		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;

		if (nTipoEvento == 0)
		{

			timeotimizacao = (rand() % 4000) + 1000;

			WaitForSingleObject(hTimeEvent, (DWORD)timeotimizacao);

			WaitForSingleObject(hMutex, INFINITE);
			mensagem = GeraMensagemDadosOTM();
			bool confere_lista = lista.push(mensagem);
			if (confere_lista == false)
			{
				SetConsoleTextAttribute(cout_handle, BLUE);
				printf("Lista Cheia\n");
				ReleaseMutex(hMutex);
				ResetEvent(hFullListEvent);
				ret = WaitForMultipleObjects(2, EventsR, FALSE, INFINITE);
				if (nTipoEvento == 1)
				{						//Tecla Esc foi escolhida
					break;
				}
			}
			else
			{
				SetConsoleTextAttribute(cout_handle, WHITE);
				std::cout << "Mensagem: " << mensagem << " de otimizacao armazenada\n";
				ReleaseMutex(hMutex);
			}


		}
	} while (nTipoEvento == 0);
	printf("Thread ComunicacaoDeDados terminando...\n");

	_endthreadex(0);
	return 0;

}

//verificar erros
DWORD WINAPI RetiraDadosOtimizacao(LPVOID id) {
	HANDLE Events[2] = { hKeyOEvent, hEscEvent };
	DWORD ret;
	int nTipoEvento;
	string mensagem;
	char TIPO = { '1' };
	LONG lDistanceToMove = 0;
	LONG lDistanceToMoveHigh = 0l;
	DWORD dwBytesEscritos;
	int tamanho_atual = 1;
	char buffer[39];

	WaitForSingleObject(hMutexListHD, INFINITE);

	//Criando PIPES
	/*hPipeOTM = CreateNamedPipe(
		lpszPipenameOTM,               // pipe name
		PIPE_ACCESS_DUPLEX,         // read/write access
		PIPE_TYPE_MESSAGE |         // message type pipe
		PIPE_READMODE_MESSAGE | // message-read mode
		PIPE_WAIT,              // blocking mode
		PIPE_UNLIMITED_INSTANCES,   // max. instances
		BUFSIZEOTM,                    // output buffer size
		BUFSIZEOTM,                    // input buffer size
		1000,                          // client time-out
		NULL);
		*/
	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			if (lista.getCounter() < (index + 1)) {
				ReleaseMutex(hMutex);
			}
			else {
				WaitForSingleObject(hMutex, INFINITE);
				//WaitForSingleObject(hMutexListHD, INFINITE);
				mensagem = lista.getItem(index);
				if (mensagem[8] == TIPO) {
					mensagem = lista.readItem(index);
				/*
					hFile = CreateFile(L"..\\Arquivo_Circular\\ArquivoCircular.txt",
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE, // abre para leitura e escrita
						NULL,								// atributos de seguran�a 
						OPEN_ALWAYS,						// Sempre abre o arquivo
						FILE_ATTRIBUTE_NORMAL,				// acesso síncrono
						NULL);								// Template para atributos e flags																	// Template para atributos e flags
					//CheckForError(hFile != INVALID_HANDLE_VALUE);

					memset(buffer, 0, 39);
					for (int i = 0; i < 39; i++)
					{
						buffer[i] = mensagem[i];
					}

					SetFilePointer(hFile, lDistanceToMove, &lDistanceToMoveHigh, FILE_BEGIN);

					int retornoEscrita = WriteFile(hFile, &buffer, sizeof(buffer), &dwBytesEscritos, NULL);
					lDistanceToMove += sizeof(mensagem);

					tamanho_atual++;
					if (tamanho_atual > MAX_ARQ_SIZE)
					{
						lDistanceToMove = 0;
						tamanho_atual = 1;
					}
					*/
					SetConsoleTextAttribute(cout_handle, RED);
					cout << "mensagem de otimizacao:" << mensagem << " retirada\n";
					index++;
					SetEvent(hFullListEvent);
					//ReleaseMutex(hMutexListHD);
					ReleaseMutex(hMutex);
				}
				else {
					//ReleaseMutex(hMutexListHD);
					ReleaseMutex(hMutex);
				}
			}
			//ReleaseMutex(hMutexListHD);
			ReleaseMutex(hMutex);
		}
	} while (nTipoEvento == 0);
	printf("Thread RetiraDadosOtimizacao terminando...\n");

	_endthreadex(0);
	return 0;
}

//verificar erros
DWORD WINAPI RetiraDadosProcesso(LPVOID id) {
	HANDLE Events[2] = { hKeyPEvent, hEscEvent };
	DWORD ret;
	int nTipoEvento;
	string mensagem;
	char TIPO = { '2' };

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			if (lista.getCounter() < (index + 1)) {
				ReleaseMutex(hMutex);
			}
			else {
				WaitForSingleObject(hMutex, INFINITE);
				mensagem = lista.getItem(index);
				if (mensagem[8] == TIPO) {
					mensagem = lista.readItem(index);
					SetConsoleTextAttribute(cout_handle, BLUE);
					cout << "mensagem de Processo:" << mensagem << " retirada\n";
					index++;
					ReleaseMutex(hMutex);
					SetEvent(hFullListEvent); // indica que liberou um espaço na lista
				}
				else {
					ReleaseMutex(hMutex);
				}
			}
			ReleaseMutex(hMutex);
		}
	} while (nTipoEvento == 0);
	printf("Thread RetiraDadosProcesso terminando...\n");

	_endthreadex(0);
	return 0;
}

//verificar erros
DWORD WINAPI RetiraAlarme(LPVOID id) {
	HANDLE Events[2] = { hKeyAEvent, hEscEvent };
	HANDLE Events2[2] = { hALMessageW, hEscEvent };
	DWORD ret, ret2;
	int nTipoEvento, nTipoEvento2;
	string mensagem;
	char TIPO = { '5' };
	char buffer[28];
	DWORD cbWritten;

	
	hPipeAL = CreateNamedPipe(
		lpszPipenameAL,               // pipe name
		PIPE_ACCESS_DUPLEX,         // read/write access
		PIPE_TYPE_MESSAGE |         // message type pipe
		PIPE_READMODE_MESSAGE | // message-read mode
		PIPE_WAIT,              // blocking mode
		PIPE_UNLIMITED_INSTANCES,   // max. instances
		BUFSIZEAL,                    // output buffer size
		BUFSIZEAL,                    // input buffer size
		1000,                          // client time-out
		NULL);
	BOOL fConnected = ConnectNamedPipe(hPipeAL, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

	if (fConnected)
	{
		printf("Client connected\n");
	}
	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			if (lista.getCounter() < (index + 1)) {
				ReleaseMutex(hMutex);
			}
			else {
				WaitForSingleObject(hMutex, INFINITE);
				ret2 = WaitForMultipleObjects(2, Events2, FALSE, INFINITE);
				nTipoEvento2 = ret2 - WAIT_OBJECT_0;
				if (nTipoEvento == 0) {
					mensagem = lista.getItem(index);
					if (mensagem[8] == TIPO) {
						mensagem = lista.readItem(index);
						SetConsoleTextAttribute(cout_handle, BLUE);

						memset(buffer, 0, 28);
						for (int i = 0; i < 28; i++)
						{
							buffer[i] = mensagem[i];
						}
						cout << "MENSAGEM DE ALARME SENDO ENVIADA\n";
						BOOL fSuccess = WriteFile(
							hPipeAL,        // handle to pipe
							buffer,     // buffer to write from
							BUFSIZEAL, // number of bytes to write
							&cbWritten,   // number of bytes written
							NULL);        // not overlapped I/O

						if (!fSuccess)
						{
							printf("InstanceThread WriteFile failed, GLE= %d.\n", GetLastError());
							break;
						}
						cout << "mensagem de Alarme:" << mensagem << " retirada\n";
						index++;
						SetEvent(hALMessageR);
						SetEvent(hFullListEvent);
						ResetEvent(hALMessageW);
						ReleaseMutex(hMutex);
					}
					else {
						ReleaseMutex(hMutex);
					}
				}
				else {
					ReleaseMutex(hMutex);
				}
			}
			ReleaseMutex(hMutex);
		}
	} while (nTipoEvento == 0);
	printf("Thread RetiraDadosAlarme terminando...\n");

	_endthreadex(0);
	return 0;
}

int Nseq_dados_otm = 1;
std::string GeraMensagemDadosOTM() {
	srand(time(NULL));
	std::string mensagem;
	char aux[20];
	float dado;
	if (Nseq_dados_otm > 999999) {
		Nseq_dados_otm = 1;
	}
	sprintf(aux, "%06d|11|", Nseq_dados_otm);
	mensagem = aux;
	//set point injeção de gas
	dado = rand() % 99999;
	dado /= 10;
	sprintf(aux, "%06.1f|", dado);
	mensagem = mensagem + aux;
	//set point da temperatura
	dado = rand() % 99999;
	dado /= 10;
	sprintf(aux, "%06.1f|", dado);
	mensagem = mensagem + aux;
	//volume do gas
	int dadoint = rand() % 99999;
	sprintf(aux, "%05d|", dadoint);
	mensagem = mensagem + aux;
	// time stamp
	SYSTEMTIME local;
	GetLocalTime(&local);
	int hora = local.wHour;
	int min = local.wMinute;
	int sec = local.wSecond;
	sprintf(aux, "%02d:%02d:%02d", hora, min, sec);
	mensagem = mensagem + aux;
	Nseq_dados_otm++;
	return mensagem;
}

int Nseq_dados_pro = 1;
std::string GeraMensagemDadosProcesso() {
	srand(time(NULL));
	std::string mensagem;
	char aux[20];
	float dado;
	if (Nseq_dados_pro > 999999) {
		Nseq_dados_pro = 1;
	}
	sprintf(aux, "%06d|22|", Nseq_dados_pro);
	mensagem = aux;
	//pressao tubo extracao
	dado = rand() % 99999;
	dado /= 10;
	sprintf(aux, "%06.1f|", dado);
	mensagem = mensagem + aux;
	//temperatura do tubo
	dado = rand() % 99999;
	dado /= 10;
	sprintf(aux, "%06.1f|", dado);
	mensagem = mensagem + aux;
	//pressao reservatorio
	dado = rand() % 99999;
	dado /= 10;
	sprintf(aux, "%06.1f|", dado);
	mensagem = mensagem + aux;
	//nivel reservatorio
	dado = rand() % 99999;
	dado /= 10;
	sprintf(aux, "%06.1f|", dado);
	mensagem = mensagem + aux;
	// time stamp
	SYSTEMTIME local;
	GetLocalTime(&local);
	int hora = local.wHour;
	int min = local.wMinute;
	int sec = local.wSecond;
	sprintf(aux, "%02d:%02d:%02d", hora, min, sec);
	mensagem = mensagem + aux;
	Nseq_dados_pro++;
	return mensagem;
}

int Nseq_alarme = 1;
std::string GeraMensagemAlarme() {
	srand(time(NULL));
	std::string mensagem;
	char aux[20];
	int dado;
	if (Nseq_alarme > 999999) {
		Nseq_alarme = 1;
	}
	sprintf(aux, "%06d|55|", Nseq_dados_pro);
	mensagem = aux;
	//Identificacao do alarme
	dado = rand() % 9999;
	sprintf(aux, "%04d|", dado);
	mensagem = mensagem + aux;
	//Prioridade do alarme
	dado = rand() % 999;
	sprintf(aux, "%03d|", dado);
	mensagem = mensagem + aux;
	// time stamp
	SYSTEMTIME local;
	GetLocalTime(&local);
	int hora = local.wHour;
	int min = local.wMinute;
	int sec = local.wSecond;
	sprintf(aux, "%02d:%02d:%02d", hora, min, sec);
	mensagem = mensagem + aux;
	Nseq_alarme++;
	return mensagem;
}