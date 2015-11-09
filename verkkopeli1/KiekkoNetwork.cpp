#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "KiekkoNetwork.h"
#define SERVER "127.0.0.1"
#define PORT "8888"


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

	if (send(ConnectSocket, buf, sendLength, 0) == SOCKET_ERROR)
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

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup(...) failed! Error code : %d\n", WSAGetLastError());
		return 1;
	}

	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *result = NULL;
	if (getaddrinfo("127.0.0.1", PORT, &hints, &result) != 0)
	{
		printf("getaddrinfo failed");
		//WSACleanup();
		return 1;
	}

	struct addrinfo* ptr = NULL;

	for (ptr = result; ptr != NULL; ptr=ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("socket(...) failed! Error code : %d\n", WSAGetLastError());
			//WSACleanup();
			return 1;
		}
		
		if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}


	recvThreadMutex.lock();
	std::thread th(ReceiveThread, ConnectSocket, recvLength);
	th.detach();

	SendPackage temp;
	temp.ownPos = 125.0;

	if (SendMsg(temp))
		return 1;

	return 0;
}

KiekkoNetwork::ReceivePackage KiekkoNetwork::GetLatestPackage()
{
	packageLock.lock();
	ReceivePackage temp = latestPackage;
	packageLock.unlock();
	newPackage = false;
	return temp;
}

void KiekkoNetwork::SetLatestPackage(ReceivePackage pckg)
{
	newPackage = true;
	latestPackage = pckg;
}

void KiekkoNetwork::InitValues()
{
	paskafix = true;
	newPackage = true;

	latestPackage.enemyPos = 0;
	latestPackage.ballX = 0;
	latestPackage.ballY = 0;
	latestPackage.ballXVel = 0;
	latestPackage.ballYVel = 0;

	sendLength = sizeof(int) * 1;
	recvLength = sizeof(int) * 5;

	slen = sizeof(si_other);
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
void ParseMessage(char* buf)
{
	packageLock.lock();

	KiekkoNetwork::ReceivePackage temp;

	temp.enemyPos = ntohl(*((int*)(&buf[sizeof(int) * 0])));
	temp.ballX =	ntohl(*((int*)(&buf[sizeof(int) * 1])));
	temp.ballY =	ntohl(*((int*)(&buf[sizeof(int) * 2])));
	temp.ballXVel = ntohl(*((int*)(&buf[sizeof(int) * 3])));
	temp.ballYVel = ntohl(*((int*)(&buf[sizeof(int) * 4])));

	KiekkoNetwork::GetInstance()->SetLatestPackage(temp);

	packageLock.unlock();
}


void ReceiveThread(int s, int recvLength)
{
	bool paskafix = true;
	char* buf = (char*)malloc(recvLength);

	recvThreadMutex.lock();

	while (1)
	{
		if (recv(s, buf, recvLength, 0) == SOCKET_ERROR)
		{
			printf("recvfrom(...) failed! Error code : %d\n", WSAGetLastError());
			recvThreadMutex.unlock();
			return;
		}
		if (paskafix)
		{
			recvThreadMutex.unlock();
			paskafix = false;
		}

		ParseMessage(buf);
	}
}

