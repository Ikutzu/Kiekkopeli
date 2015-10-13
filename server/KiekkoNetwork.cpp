#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "KiekkoNetwork.h"
#define PORT 8888


KiekkoNetwork* KiekkoNetwork::instance = nullptr;

void ReceiveThread(int s, int recvLength);
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

int KiekkoNetwork::SendMsg(SendPackage pckg)
{
	char* buf = CreateMessage(pckg);

	if (sendto(s, buf, sendLength, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto(...) failed! Error code : %d\n", WSAGetLastError());
		return 1;
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
	
	if ((ListenSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
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
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen(...) failed! Error code : %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept(...) failed! Error code : %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	std::thread th(NewClient, ClientSocket);
}

KiekkoNetwork::ReceivePackage KiekkoNetwork::GetLatestPackage()
{
	
}

void KiekkoNetwork::SetLatestPackage(ReceivePackage pckg)
{

}

void KiekkoNetwork::InitValues()
{
	sendLength = sizeof(float) * 6;
	recvLength = sizeof(float) * 1;
}

char* KiekkoNetwork::CreateMessage(SendPackage pckg)
{
	char* buf = (char*)malloc(sendLength);
	int index = 0;

	*((int*)(&buf[index])) = htonl(pckg.ownPos);
	index += sizeof(pckg.ownPos);

	return buf;
}

//thread


std::mutex socketlistmtx;

void AddActiveSocket(SOCKET *s)
{
	socketlistmtx.lock();
	KiekkoNetwork::GetInstance()->activeSocket.push_back(s);
	socketlistmtx.unlock();
}

void NewClient(SOCKET ClientSocket)
{
	AddActiveSocket(&ClientSocket);

}


void ParseMessage(char* buf, int socket)
{
	packageLock.lock();

	KiekkoNetwork::ReceivePackage temp;

	temp.playerPos = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 0]))));
	temp.enemyPos = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 1]))));
	temp.ballX = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 2]))));
	temp.ballY = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 3]))));
	temp.ballXVel = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 4]))));
	temp.ballYVel = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 5]))));

	KiekkoNetwork::GetInstance()->SetLatestPackage(temp);

	packageLock.unlock();
}

void ReceiveThread(int s, int recvLength)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char* buf = (char*)malloc(recvLength);

	recvThreadMutex.lock();

	while (1)
	{
		if (recvfrom(s, buf, recvLength, 0, (struct sockaddr*) &si_other, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom(...) failed! Error code : %d\n", WSAGetLastError());
			recvThreadMutex.unlock();
			return;
		}

		ParseMessage(buf, s);
	}
}

