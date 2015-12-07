#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "KiekkoNetwork.h"
#define PORT "8888"


KiekkoNetwork* KiekkoNetwork::instance = nullptr;

void NewClient(SOCKET ClientSocket, int id);

std::mutex recvThreadMutex;
std::mutex packageLock;

KiekkoNetwork* KiekkoNetwork::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new KiekkoNetwork();
		instance->InitValues();
		instance->InitializeNetwork();
	}

	return instance;
}

void KiekkoNetwork::DeleteInstance()
{
	for (int i = 0; i < instance->activeSocket.size();)
	{
		if (instance->activeSocket[i] == nullptr)
			i++;
	}
	
	delete instance;
	instance = nullptr;
}

KiekkoNetwork::ReceivePackage KiekkoNetwork::GetLatestPackage()
{
	packageLock.lock();
	ReceivePackage temp = latestPackage;
	packageLock.unlock();
	newPackage = false;
	return temp;
}

int KiekkoNetwork::SendMsg(SendPackage pckg)
{
	for (int i = 0; i < activeSocket.size(); i++)
	{
		if (activeSocket[i] == nullptr)
			return 1;

		char* buf = CreateMessage(pckg, i);
		if (send(*activeSocket[i], buf, sendLength, 0) == SOCKET_ERROR)
		{
			printf("sendto(...) failed! Error code : %d\n", WSAGetLastError());
			return 1;
		}
	}
	if (paskafix)
	{
		recvThreadMutex.unlock();
		paskafix = false;
	}
	return 0;
}

int KiekkoNetwork::InitializeNetwork()
{
	printf("Setting up network...\n");

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	slen = sizeof(si_other);

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup(...) failed! Error code : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *result = NULL;
	int iResult = getaddrinfo(NULL, PORT, &hints, &result);


	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket(...) failed! Error code : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	
	if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
	{
		printf("bind(...) failed! Error code : %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		ListenSocket = INVALID_SOCKET;
		WSACleanup();
		return 1;
	}

	return 0;
}

void KiekkoNetwork::Update(int threadCount)
{
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen(...) failed! Error code : %d\n", WSAGetLastError());
		//closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept(...) failed! Error code : %d\n", WSAGetLastError());
		//closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	recvThreadMutex.lock();
	
	std::thread th(NewClient, ClientSocket, threadCount);
	th.detach();

	SendPackage temp = { 0, 0, 0, 0, 0 };
	
	paskafix = true;
	SendMsg(temp);
}

void KiekkoNetwork::CloseConnections()
{
	printf("Shutting down connections...\n");

	for (int i = 0; i < activeSocket.size(); i++)
	{
		if (activeSocket[i] != nullptr)
			closesocket(*(activeSocket[i]));
	}

	activeSocket.clear();

	closesocket(ListenSocket);
	WSACleanup();
}

void KiekkoNetwork::InitValues()
{
	paskafix = true;

	latestPackage.player1Pos = 125;
	latestPackage.player2Pos = 125;

	sendLength = sizeof(int) * 5;
	recvLength = sizeof(int) * 2;
}


char* KiekkoNetwork::CreateMessage(SendPackage pckg, int id)
{
	char* buf = (char*)malloc(sendLength);
	int index = 0;
	
	if (id == 0)
		*((int*)(&buf[index])) = htonl(pckg.player2Pos);
	else if ( id == 1)
		*((int*)(&buf[index])) = htonl(250-pckg.player1Pos);
	index += sizeof(pckg.player1Pos);

	if (id == 0)
		*((int*)(&buf[index])) = htonl(pckg.ballX);
	else if (id == 1)
		*((int*)(&buf[index])) = htonl(250-pckg.ballX);
	index += sizeof(pckg.ballX);
	
	if (id == 0)
		*((int*)(&buf[index])) = htonl(pckg.ballY);
	else if (id == 1)
		*((int*)(&buf[index])) = htonl(500-pckg.ballY);
	index += sizeof(pckg.ballY);

	if (id == 0)
		*((int*)(&buf[index])) = htonl(pckg.ballAngle);
	else if (id == 1)
		*((int*)(&buf[index])) = htonl(pckg.ballAngle + 180);
	index += sizeof(pckg.ballAngle);

	*((int*)(&buf[index])) = htonl(pckg.ballVelocity);
	index += sizeof(pckg.ballVelocity);

	return buf;
}

//thread

void ReceiveThread(SOCKET *s, int id);
std::mutex socketlistmtx;

void AddActiveSocket(SOCKET *s, int id)
{
	socketlistmtx.lock();
	KiekkoNetwork::GetInstance()->activeSocket[id] = s;
	socketlistmtx.unlock();
}

void NewClient(SOCKET ClientSocket, int id)
{
	printf("Connection %d added...\n", id);
	AddActiveSocket(&ClientSocket, id);
	ReceiveThread(&ClientSocket, id);

	KiekkoNetwork::GetInstance()->activeSocket[id] = nullptr;
	
	printf("Connection %d dead...\n", id);
}

void ParseMessage(char* buf, int socket, int id)
{
	int tempFloat = 0;
	
	tempFloat = ntohl(*((int*)(&buf[0])));

	packageLock.lock();

	if (id == 0)
		KiekkoNetwork::GetInstance()->latestPackage.player1Pos = tempFloat;
	if (id == 1)
		KiekkoNetwork::GetInstance()->latestPackage.player2Pos = 250-tempFloat;

	KiekkoNetwork::GetInstance()->newPackage = true;
	packageLock.unlock();
}

void ReceiveThread(SOCKET* s, int id)
{
	bool paskafix = true;
	char* buf = (char*)malloc(sizeof(float));

	recvThreadMutex.lock();

	while (1)
	{
		memset(buf, 0, sizeof(buf));
		if (recv(*s, buf, sizeof(float), 0) == SOCKET_ERROR)
		{
			printf("recvfrom(...) failed! Error code : %d\n", WSAGetLastError());

			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(*s);

			return;
		}
		if (paskafix)
		{
			recvThreadMutex.unlock();
			paskafix = false;
		}
	
		ParseMessage(buf, *s, id);
	}
}

