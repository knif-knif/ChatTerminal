#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <cstdio>

#pragma comment(lib, "Ws2_32.lib")
#define BUFF_SIZE 1024

char szMsg[BUFF_SIZE];

unsigned sendMsg(void* arg) {
	SOCKET sock = *((SOCKET*)arg);
	while (1) {
		scanf("%s", szMsg);
		if (!strcmp(szMsg, "QUIT\n") || !strcmp(szMsg, "quit\n")) {
			closesocket(sock);
			exit(0);
		}

		send(sock, szMsg, strlen(szMsg), 0);
	}
	return 0;
}

unsigned recvMsg(void* arg) {
	SOCKET sock = *((SOCKET*)arg);
	char msg[BUFF_SIZE];
	while (1) {
		int len = recv(sock, msg, sizeof(msg) - 1, 0);
		if (len == -1) {
			return -1;
		}
		msg[len] = '\0';
		printf("%s\n", msg);
	}
	return 0;
}

int main() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		perror("[Error], WSA");
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return -1;
	}

	SOCKET hSock;
	hSock = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN servAdr;
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_port = htons(9999);
	inet_pton(AF_INET, "111.229.143.40", &servAdr.sin_addr);
		
	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
		printf("[Error]: connect %d", GetLastError());
		return -1;
	}
	else {
		printf("Welcome. Enter yourname");
	}

	HANDLE hsend = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendMsg, (void*)&hSock, 0, NULL);
	HANDLE hrecv = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvMsg, (void*)&hSock, 0, NULL);

	WaitForSingleObject(hsend, INFINITE);
	WaitForSingleObject(hrecv, INFINITE);

	closesocket(hSock);
	WSACleanup();
	return 0;
}