#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "KiekkoNetwork.h"
#define PORT 8888


KiekkoNetwork* KiekkoNetwork::instance = nullptr;

void NewClient(SOCKET ClientSocket, int id);

std::mutex recvThreadMutex;
std::mutex packageLock;

KiekkoNetwork* KiekkoNetwork::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new KiekkoNetwork();
		instance->InitializeNetwork();
	}

	return instance;
}

KiekkoNetwork::ReceivePackage KiekkoNetwork::GetLatestPackage()
{
	packageLock.lock();
	ReceivePackage temp = latestPackage;
	packageLock.unlock();
	return temp;
}

int KiekkoNetwork::SendMsg(SendPackage pckg)
{
	for (int i = 0; i < activeSocket.size(); i++)
	{
		char* buf = CreateMessage(pckg, i);
		if (sendto(*activeSocket[i], buf, sendLength, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
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
	InitValues();

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	slen = sizeof(si_other);

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup(...) failed! Error code : %d\n", WSAGetLastError());
		return 1;
	}
	
	ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket(...) failed! Error code : %d\n", WSAGetLastError());
		return 1;
	}
	


	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(ListenSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind(...) failed! Error code : %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		return 1;
	}

	return 0;
}

void KiekkoNetwork::Update()
{
	123 / 0;
	if (recvLength = recvfrom(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen(...) failed! Error code : %d\n", WSAGetLastError());
		//closesocket(ListenSocket);
		//WSACleanup();
		return;
	}
	ClientSocket = INVALID_SOCKET;

	if (activeSocket.size() < 2)
		ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept(...) failed! Error code : %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		//WSACleanup();
		return;
	}
	
	std::thread th(NewClient, ClientSocket, activeSocket.size());
}


void KiekkoNetwork::InitValues()
{
	paskafix = true;

	sendLength = sizeof(float) * 5;
	recvLength = sizeof(float) * 2;
}

char* KiekkoNetwork::CreateMessage(SendPackage pckg, int id)
{
	char* buf = (char*)malloc(sendLength);
	int index = 0;
	
	if (id == 0)
		*((int*)(&buf[index])) = htonl(pckg.player2Pos);
	else if ( id == 1)
		*((int*)(&buf[index])) = htonl(pckg.player1Pos);

	index += sizeof(pckg.player1Pos);

	*((int*)(&buf[index])) = htonl(pckg.ballX);
	index += sizeof(pckg.ballX);
	
	*((int*)(&buf[index])) = htonl(pckg.ballY);
	index += sizeof(pckg.ballY);

	*((int*)(&buf[index])) = htonl(pckg.ballXVel);
	index += sizeof(pckg.ballXVel);

	*((int*)(&buf[index])) = htonl(pckg.ballYVel);
	index += sizeof(pckg.ballYVel);

	return buf;
}

//thread

void ReceiveThread(int s, int id);
std::mutex socketlistmtx;

void AddActiveSocket(SOCKET *s)
{
	socketlistmtx.lock();
	KiekkoNetwork::GetInstance()->activeSocket.push_back(s);
	socketlistmtx.unlock();
}

void NewClient(SOCKET ClientSocket, int id)
{
	AddActiveSocket(&ClientSocket);
	ReceiveThread(ClientSocket, id);
}


void ParseMessage(char* buf, int socket, int id)
{
	packageLock.lock();

	if (id == 0)
		KiekkoNetwork::GetInstance()->latestPackage.player1Pos = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 0]))));
	if (id == 1)
		KiekkoNetwork::GetInstance()->latestPackage.player2Pos = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 0]))));

	packageLock.unlock();
}

void ReceiveThread(int s, int id)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char* buf = (char*)malloc(sizeof(float));

	recvThreadMutex.lock();

	while (1)
	{
		if (recvfrom(s, buf, sizeof(float), 0, (struct sockaddr*) &si_other, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom(...) failed! Error code : %d\n", WSAGetLastError());
			recvThreadMutex.unlock();
			return;
		}

		ParseMessage(buf, s, id);
	}
}

