#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "KiekkoNetwork.h"
#define SERVER "127.0.0.1"
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

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup(...) failed! Error code : %d\n", WSAGetLastError());
		return 1;
	}

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket(...) failed! Error code : %d\n", WSAGetLastError());
		return 1;
	}

	memset((char*)&si_other, 0, slen);
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

	recvThreadMutex.lock();
	std::thread th(ReceiveThread, s, recvLength);
	th.detach();

	SendPackage temp;
	temp.ownPos = 125.0;

	if (SendMsg(temp))
		return 1;

	return 0;
}

KiekkoNetwork::ReceivePackage KiekkoNetwork::GetLatestPackage()
{
	newPackage = false;
	return latestPackage; 
};

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

	sendLength = sizeof(float) * 1;
	recvLength = sizeof(float) * 5;

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

	temp.enemyPos = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 0]))));
	temp.ballX = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 1]))));
	temp.ballY = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 2]))));
	temp.ballXVel = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 3]))));
	temp.ballYVel = *((float*)ntohl(*((int*)(&buf[sizeof(float) * 4]))));

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

		ParseMessage(buf);
	}
}

