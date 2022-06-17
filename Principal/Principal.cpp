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
DWORD WINAPI ComunicacaoDeDados(LPVOID id);
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

//Handles
HANDLE hEscEvent, hKeyCEvent, hKeyOEvent, hKeyPEvent, hKeyAEvent, hKeyTEvent, hKeyREvent, hKeyLEvent, hKeyZEvent;
HANDLE hMutex; //Handle para o mutex da lista circular

//Define cores de texto
#define PURPLE  FOREGROUND_RED | FOREGROUND_GREEN
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

	bStatus = CreateProcess(
		L".\\Exibicao_de_dados_do_processo\\x64\\Debug\\Exb_dados_processo.exe",
		NULL,	// linha de comando
		NULL,	// atributos de seguran�a: Processo
		NULL,	// atributos de seguran�a: Thread
		FALSE,	// heran�a de handles
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,	// CreationFlags
		NULL,	// lpEnvironment
		L".\\Exibicao_de_dados_do_processo\\x64\\Debug",
		&si,			// lpStartUpInfo
		&NewProcess);	// lpProcessInformation
	if (!bStatus) printf("Erro na criacao do processo! Codigo = %d\n", GetLastError());

	bStatus = CreateProcess(
		L".\\Exibicao_de_dados_de_otimizacao\\x64\\Debug\\Exb_dados_otmz.exe",
		NULL,	// linha de comando
		NULL,	// atributos de seguran�a: Processo
		NULL,	// atributos de seguran�a: Thread
		FALSE,	// heran�a de handles
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,	// CreationFlags
		NULL,	// lpEnvironment
		L".\\Exibicao_de_dados_de_otimizacao\\x64\\Debug\\",
		&si,			// lpStartUpInfo
		&NewProcess);	// lpProcessInformation
	if (!bStatus) printf("Erro na criacao do processo! Codigo = %d\n", GetLastError());

	bStatus = CreateProcess(
		L".\\Exibicao_de_alarmes\\x64\\Debug\\Exb_alarmes.exe",
		NULL,	// linha de comando
		NULL,	// atributos de seguran�a: Processo
		NULL,	// atributos de seguran�a: Thread
		FALSE,	// heran�a de handles
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,	// CreationFlags
		NULL,	// lpEnvironment
		L".\\Exibicao_de_alarmes\\x64\\\\Debug",
		&si,			// lpStartUpInfo
		&NewProcess);	// lpProcessInformation
	if (!bStatus) printf("Erro na criacao do processo! Codigo = %d\n", GetLastError());

	//Criando Eventos
	
	hEscEvent = CreateEvent(NULL, TRUE, FALSE, L"EscEvent");				//Cria do evento para a tecla ESC
	//CheckForError(hEscEvent);
	hKeyCEvent = CreateEvent(NULL, TRUE, TRUE, L"CEvent");					//Cria do evento para a tecla C
	//CheckForError(hKeyCEvent);
	hKeyOEvent = CreateEvent(NULL, TRUE, TRUE, L"OEvent");					//Cria do evento para a tecla L
	//CheckForError(hKeyOEvent);
	hKeyPEvent = CreateEvent(NULL, TRUE, TRUE, L"PEvent");					//Cria do evento para a tecla P
	//CheckForError(hKeyPEvent);
	hKeyAEvent = CreateEvent(NULL, FALSE, TRUE, L"AEvent");					//Cria do evento para a tecla A
	//CheckForError(hKeyAEvent);
	hKeyTEvent = CreateEvent(NULL, FALSE, TRUE, L"TEvent");					//Cria do evento para a tecla T
	//CheckForError(hKeyTEvent);
	hKeyREvent = CreateEvent(NULL, FALSE, FALSE, L"REvent");				//Cria do evento para a tecla R
	//CheckForError(hKeyREvent);
	hKeyLEvent = CreateEvent(NULL, FALSE, FALSE, L"LEvent");				//Cria do evento para a tecla L
	//CheckForError(hKeyLEvent);
	hKeyZEvent = CreateEvent(NULL, FALSE, FALSE, L"ZEvent");				//Cria do evento para a tecla Z
	//CheckForError(hKeyZEvent);

	bool estado_hKeyCEvent = true;
	bool estado_hKeyOEvent = true;
	bool estado_hKeyPEvent = true;
	bool estado_hKeyAEvent = true;
	bool estado_hKeyTEvent = true;
	bool estado_hKeyREvent = true;
	bool estado_hKeyLEvent = true;

	//Criando mutex
	hMutex = CreateMutex(NULL, FALSE, L"Mutex");							
	//CheckForError(hMutex);

	//Handle para as threads
	HANDLE comunicacao, retira_otimizacao, retira_processo, retira_alarme;

	//criação das threads
	DWORD dwThreadId;
	DWORD dwExitCode = 0;
	DWORD dwRet;
	int i, nTecla;


	comunicacao = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)ComunicacaoDeDados,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	// colocar checagem de erro
	retira_otimizacao = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)RetiraDadosOtimizacao,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	// colocar checagem de erro
	retira_processo = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)RetiraDadosProcesso,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	// colocar checagem de erro
	retira_alarme = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)RetiraAlarme,
		(LPVOID)0,
		0,
		(CAST_LPDWORD)&dwThreadId
	);
	// colocar checagem de erro (if ou checkerror...)


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
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Retirada de Dados de Otimizacao Desbloqueada\n");
			estado_hKeyOEvent = true;
			SetEvent(hKeyOEvent);
		}
		else if (nTecla == keyP && estado_hKeyPEvent == true) {	//Deixa o evento hLEvent n�o sinalizado
			ResetEvent(hKeyPEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Retirada de Dados de Processo Bloqueada\n");
			estado_hKeyPEvent = false;
		}
		else if (nTecla == keyP && estado_hKeyPEvent == false) {	//Deixa o evento hLEvent sinalizado
			SetEvent(hKeyPEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Retirada de Dados de Processo Desbloqueada\n");
			estado_hKeyPEvent = true;
		}
		else if (nTecla == keyA && estado_hKeyAEvent == true) {	//Deixa o evento hLEvent n�o sinalizado
			ResetEvent(hKeyAEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Retirada de Alarmes Bloqueada\n");
			estado_hKeyAEvent = false;
		}
		else if (nTecla == keyA && estado_hKeyAEvent == false) {	//Deixa o evento hLEvent sinalizado
			SetEvent(hKeyAEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Retirada de Alarmes Desbloqueada\n");
			estado_hKeyAEvent = true;
		}
		else if (nTecla == keyT && estado_hKeyTEvent == true) {	//Deixa o evento hLEvent n�o sinalizado
			ResetEvent(hKeyTEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Exibicao de dados de otimizacao Bloqueada\n");
			estado_hKeyTEvent = false;
		}
		else if (nTecla == keyT && estado_hKeyTEvent == false) {	//Deixa o evento hLEvent sinalizado
			SetEvent(hKeyTEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Exibicao de dados de otimizacao Desbloqueada\n");
			estado_hKeyTEvent = true;
		}
		else if (nTecla == keyR && estado_hKeyREvent == true) {	//Deixa o evento hLEvent n�o sinalizado
			ResetEvent(hKeyREvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Exibicao de dados de Processo Bloqueada\n");
			estado_hKeyREvent = false;
		}
		else if (nTecla == keyR && estado_hKeyREvent == false) {	//Deixa o evento hLEvent sinalizado
			SetEvent(hKeyREvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Exibicao de dados de Processo Desbloqueada\n");
			estado_hKeyREvent = true;
		}
		else if (nTecla == keyL && estado_hKeyLEvent == true) {	//Deixa o evento hLEvent n�o sinalizado
			ResetEvent(hKeyLEvent);
			SetConsoleTextAttribute(cout_handle, RED);
			printf("Exibicao de alarmes Bloqueada\n");
			estado_hKeyLEvent = false;
		}
		else if (nTecla == keyL && estado_hKeyLEvent == false) {	//Deixa o evento hLEvent sinalizado
			SetEvent(hKeyLEvent);
			SetConsoleTextAttribute(cout_handle, GREEN);
			printf("Exibicao de alarmes Desbloqueada\n");
			estado_hKeyLEvent = true;
		}
		else if (nTecla == keyZ) {
			SetEvent(hKeyZEvent);
		}
		else if (nTecla == ESC) {	//Deixa o evento hEscEvent sinalizado e os eventos hLEvent, hPEvent, hREvent n�o sinalizados
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
	HANDLE hThreads[4] = { comunicacao, retira_otimizacao, retira_processo, retira_alarme };
	dwRet = WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);
	//CheckForError(dwRet == WAIT_OBJECT_0);

	for (int i = 0; i < 4; ++i) {
		GetExitCodeThread(hThreads[i], &dwExitCode);
		CloseHandle(hThreads[i]);						//Apaga refer�ncia ao objeto
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

//Tarefa de comunicação de dados
DWORD WINAPI ComunicacaoDeDados(LPVOID id) {

	HANDLE Events[2] = { hKeyCEvent, hEscEvent };
	HANDLE EventsR[4] = { hKeyAEvent ,hKeyPEvent,hKeyOEvent, hEscEvent };
	DWORD ret;
	int nTipoEvento;
	std::string mensagem;

	do {
		Sleep(1000);
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;

		if (nTipoEvento == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			mensagem = GeraMensagemDadosOTM();
			bool confere_lista = lista.push(mensagem);
			if (confere_lista == false) {			
				SetConsoleTextAttribute(cout_handle, BLUE);
				printf("Lista Cheia\n");
				ReleaseMutex(hMutex);
				ret = WaitForMultipleObjects(4, EventsR, FALSE, INFINITE);		//Bloqueia a thread de leitura enquanto os eventos hREvent ou hEscEvent n�o forem sinalizados
				if (nTipoEvento != 0) {						//Tecla Esc foi escolhida
					break;
				}
			}
			else {
				SetConsoleTextAttribute(cout_handle, WHITE);
				std::cout << "Mensagem: " << mensagem << " de otimizacao armazenada\n";
				ReleaseMutex(hMutex);
			}
			WaitForSingleObject(hMutex, INFINITE);
			mensagem = GeraMensagemDadosProcesso();
			if (lista.push(mensagem) == false) {
				SetConsoleTextAttribute(cout_handle, PURPLE);
				printf("Lista Cheia\n");
				ReleaseMutex(hMutex);
				ret = WaitForMultipleObjects(4, EventsR, FALSE, INFINITE);		//Bloqueia a thread de leitura enquanto os eventos hREvent ou hEscEvent n�o forem sinalizados
				if (nTipoEvento != 0) {						//Tecla Esc foi escolhida
					break;
				}
			}
			else {
				SetConsoleTextAttribute(cout_handle, WHITE);
				std::cout << "Mensagem: " << mensagem << " de dados processo\n";
				ReleaseMutex(hMutex);
			}
			WaitForSingleObject(hMutex, INFINITE);
			mensagem = GeraMensagemAlarme();
			if (lista.push(mensagem) == false) {
				SetConsoleTextAttribute(cout_handle, PURPLE);
				printf("Lista Cheia\n");
				ReleaseMutex(hMutex);
				ret = WaitForMultipleObjects(4, EventsR, FALSE, INFINITE);		//Bloqueia a thread de leitura enquanto os eventos hREvent ou hEscEvent n�o forem sinalizados
				if (nTipoEvento != 0) {						//Tecla Esc foi escolhida
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
	return 0;

}

DWORD WINAPI RetiraDadosOtimizacao(LPVOID id) {
	HANDLE Events[2] = { hKeyOEvent, hEscEvent };
	DWORD ret;
	int nTipoEvento, pos_lista;
	string mensagem;
	string ID = {'1'};

	do {
		ret = WaitForMultipleObjects(2, Events, FALSE, INFINITE);
		nTipoEvento = ret - WAIT_OBJECT_0;
		if (nTipoEvento == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			pos_lista = lista.getPointer();
			mensagem = lista.getItem(pos_lista);
			if (mensagem[8] == '1') {
				mensagem = lista.readItem(pos_lista);
				SetConsoleTextAttribute(cout_handle, BLUE);
				printf("mensagem: %s retirada\n", mensagem);
				ReleaseMutex(hMutex);
			}
			else {
				ReleaseMutex(hMutex);
			}
		}
	} while (nTipoEvento == 0);
	return 0;
}

DWORD WINAPI RetiraDadosProcesso(LPVOID id) {
	return 0;
}

DWORD WINAPI RetiraAlarme(LPVOID id) {
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