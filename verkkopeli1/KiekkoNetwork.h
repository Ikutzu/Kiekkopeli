#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <string>

class KiekkoNetwork
{
public:
	struct ReceivePackage
	{
		int enemyPos;
		int ballX, ballY;
		int ballXVel, ballYVel;
	};

	struct SendPackage
	{
		int ownPos;
	};


	~KiekkoNetwork(){
		closesocket(ConnectSocket);
		WSACleanup();
	};

	static KiekkoNetwork* GetInstance();

	int SendMsg(SendPackage pckg);
	int InitializeNetwork();
	ReceivePackage GetLatestPackage();
	void SetLatestPackage(ReceivePackage pckg);

	bool newPackage;
	std::string SERVER;

private:

	KiekkoNetwork(){};
	static KiekkoNetwork* instance;

	void InitValues();
	char* CreateMessage(SendPackage pckg);

	struct sockaddr_in si_other;
	SOCKET ConnectSocket;
	int slen;
	int sendLength;
	int recvLength;
	WSADATA wsa;

	bool paskafix;
	

	ReceivePackage latestPackage;
};

