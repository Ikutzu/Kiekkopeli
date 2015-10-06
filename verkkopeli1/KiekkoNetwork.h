#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <thread>
#include <mutex>

class KiekkoNetwork
{
public:
	struct ReceivePackage
	{
		float enemyPos;
		float ballX, ballY;
		float ballXVel, ballYVel;
	};

	struct SendPackage
	{
		float ownPos;
	};


	~KiekkoNetwork(){
		closesocket(s);
		WSACleanup();
	};

	static KiekkoNetwork* GetInstance();

	int SendMsg(SendPackage pckg);
	int InitializeNetwork();
	ReceivePackage GetLatestPackage();
	void SetLatestPackage(ReceivePackage pckg);

	bool newPackage;

private:

	KiekkoNetwork(){};
	static KiekkoNetwork* instance;
	

	void InitValues();
	char* CreateMessage(SendPackage pckg);

	struct sockaddr_in si_other;
	int s;
	int slen;
	int sendLength;
	int recvLength;
	WSADATA wsa;

	bool paskafix;
	

	ReceivePackage latestPackage;
};

