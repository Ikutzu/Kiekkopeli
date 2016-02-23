#include <memory>
#include "KiekkoNetwork.h"

#define PORT 8888
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

KiekkoNetwork* KiekkoNetwork::instance = 0;

void NewClient(int  ClientSocket, int id);

std::mutex recvThreadMutex;
std::mutex packageLock;

KiekkoNetwork* KiekkoNetwork::GetInstance()
{
	if (instance == 0)
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
		if (instance->activeSocket[i] == 0)
			i++;
	}

	delete instance;
	instance = 0;
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
		if (activeSocket[i] == 0)
			return 1;

		char* buf = CreateMessage(pckg, i);
		if (send(*activeSocket[i], buf, sendLength, 0) == SOCKET_ERROR)
		{
			printf("sendto(...) failed! Error code ; EI KINOSTA\n");
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


	struct sockaddr_in server;

	wmemset((wchar_t*)(&server), 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = PORT;

	ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket(...) failed! Error code : EI KINOSTA\n");
		return 1;
	}

	if (bind(ListenSocket, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		printf("bind(...) failed! Error code : EI KINOSTA\n" );
		shutdown(ListenSocket, SHUT_RDWR);
		ListenSocket = INVALID_SOCKET;
		return 1;
	}

	return 0;
}

void KiekkoNetwork::Update(int threadCount)
{
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen(...) failed! Error code : EI KINOSTA\n");
		return;
	}
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, 0, 0);

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept(...) failed! Error code : EI KINOSTA\n");
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
		if (activeSocket[i] != 0)
			shutdown(*(activeSocket[i]), SHUT_RDWR);
	}

	activeSocket.clear();

	shutdown(ListenSocket, SHUT_RDWR);
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

void ReceiveThread(int *s, int id);
std::mutex socketlistmtx;

void AddActiveSocket(int *s, int id)
{
	socketlistmtx.lock();
	KiekkoNetwork::GetInstance()->activeSocket[id] = s;
	socketlistmtx.unlock();
}

void NewClient(int ClientSocket, int id)
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

void ReceiveThread(int* s, int id)
{
	bool paskafix = true;
	char* buf = (char*)malloc(sizeof(float));

	recvThreadMutex.lock();

	while (1)
	{
		wmemset((wchar_t*)buf, 0, sizeof(buf));
		if (recv(*s, buf, sizeof(float), 0) == SOCKET_ERROR)
		{
			printf("recvfrom(...) failed! Error code : EI KINOSTA\n");

			printf("shutdown failed with error: EI KINOSTA\n");
			shutdown(*s, SHUT_RDWR);

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

