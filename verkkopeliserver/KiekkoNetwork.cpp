#include <cstring>
#include "KiekkoNetwork.h"

#define PORT 8888
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

KiekkoNetwork* purkka = 0;
KiekkoNetwork* KiekkoNetwork::instance = 0;
bool KiekkoNetwork::newPackage = false;
void NewClient(int  ClientSocket, int id);

std::mutex recvThreadMutex;
std::mutex packageLock;
std::mutex socketlistmtx;

KiekkoNetwork* KiekkoNetwork::GetInstance()
{
	int hi = 1;
	if (instance == 0)
	{
		instance = new KiekkoNetwork();
		printf("New Instance\n");
	}

	while(hi != 0)
		hi = instance->InitializeNetwork();

	purkka = instance;
	return instance;
}

KiekkoNetwork::KiekkoNetwork()
: latestPackage({125, 125})
, sendLength(sizeof(int)*5)
, recvLength(sizeof(int))
, ListenSocket(INVALID_SOCKET)
, ClientSocket(INVALID_SOCKET)
, paskafix(true)
{}

KiekkoNetwork::~KiekkoNetwork()
{
	instance = 0;
}

void KiekkoNetwork::DeleteInstance()
{
	printf("Deleting KiekkoNetwork::instance\n");
	instance->CloseConnections();

	delete instance;
	purkka = 0;
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
	for (unsigned int i = 0; i < activeSocket.size(); i++)
	{
		if (activeSocket[i] == 0)
			return 1;

		char* buf = CreateMessage(pckg, i);
		if (send(*activeSocket[i], buf, sendLength, 0) == SOCKET_ERROR)
		{
			int errsave = errno;
			printf("send(...) failed! Error code : %d\n", errsave);
			return 1;
		}
		free(buf);
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
	int ret = 0;
	printf("Setting up network...\n");

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;


	struct sockaddr_in server;
	memset((void*)(&server), 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("socket number %d\n", ListenSocket);
	if (ListenSocket == INVALID_SOCKET)
	{
		int errsav = errno;
		shutdown(ListenSocket, SHUT_RDWR);
		ListenSocket = INVALID_SOCKET;
		printf("socket(...) failed! Error code : %d\n", errsav);
		ret = 1;
	}

	if (bind(ListenSocket, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		int errsav = errno;
		printf("bind(...) failed! Error code : %d\n", errsav );
		shutdown(ListenSocket, SHUT_RDWR);
		ListenSocket = INVALID_SOCKET;
		ret = 2;
	}

	return ret;
}

int KiekkoNetwork::Update(int threadCount)
{
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		int errsave = errno;
		printf("listen(...) failed! Error code : %d\n", errsave);
		return 0;
	}
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, 0, 0);

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept(...) failed! Error code : EI KINOSTA\n");
		return 0;
	}
	recvThreadMutex.lock();

	std::thread th(NewClient, ClientSocket, threadCount);
	th.detach();

	SendPackage temp = { 0, 0, 0, 0, 0 };

	paskafix = true;
	SendMsg(temp);
	return 1;
}

void KiekkoNetwork::CloseConnections()
{
	printf("Shutting down connections...\n");
	unsigned int i;
	for ( i = 0; i < activeSocket.size(); i++)
	{
		if(*activeSocket[i] != INVALID_SOCKET)
		{
			shutdown(*(activeSocket[i]), SHUT_RDWR);
			*activeSocket[i] = INVALID_SOCKET;
			activeSocket[i] = 0;
		}
	}

	activeSocket.clear();

//	shutdown(ListenSocket, SHUT_RDWR);
	shutdown(ClientSocket, SHUT_RDWR);
//	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	printf("Connections shut down %d\n", i);
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

void AddActiveSocket(int *s, int id)
{
	socketlistmtx.lock();
	purkka->activeSocket[id] = s;
	socketlistmtx.unlock();
}

void NewClient(int ClientSocket, int id)
{
	printf("Connection %d added...\n", id);
	AddActiveSocket(&ClientSocket, id);
	ReceiveThread(&ClientSocket, id);

	printf("Connection %d dead...\n", id);
}

void ParseMessage(char* buf, int socket, int id)
{
	int tempFloat = 0;

	tempFloat = ntohl(*((int*)(&buf[0])));

	packageLock.lock();

	if (id == 0)
		purkka->latestPackage.player1Pos = tempFloat;
	if (id == 1)
		purkka->latestPackage.player2Pos = 250-tempFloat;

	KiekkoNetwork::newPackage = true;

	packageLock.unlock();
}

void ReceiveThread(int* s, int id)
{
	bool paskafix = true;
	char* buf = (char*)malloc(sizeof(int));

	recvThreadMutex.lock();

	while (1)
	{
		if(purkka != 0)
		{
			memset((void*)buf, 0, sizeof(int));
			if (recv(*s, buf, sizeof(int), 0) == SOCKET_ERROR)
			{
				int errsave = errno;
				printf("recvfrom(...) failed! Error code : %d\n", errsave);
				free(buf);
				return;
			}

			if (paskafix)
			{
				recvThreadMutex.unlock();
				paskafix = false;
			}

			ParseMessage(buf, *s, id);
		}
		else
			return;
	}
}

