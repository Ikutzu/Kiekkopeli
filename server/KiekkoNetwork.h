#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <vector>

class KiekkoNetwork
{
public:
	struct SendPackage
	{
		float playerPos;
		float enemyPos;
		float ballX, ballY;
		float ballXVel, ballYVel;
	};

	struct ReceivePackage
	{
		float playerPos;
	};


	~KiekkoNetwork(){
		closesocket(ListenSocket);
		WSACleanup();
	};

	static KiekkoNetwork* GetInstance();

	int SendMsg(SendPackage pckg);
	int InitializeNetwork();
	void Update();
	ReceivePackage GetLatestPackage();
	void SetLatestPackage(ReceivePackage pckg);
	
	
	std::vector<SOCKET*> activeSocket;


private:

	KiekkoNetwork(){};
	static KiekkoNetwork* instance;
	

	void InitValues();
	char* CreateMessage(SendPackage pckg);

	int sendLength;
	int recvLength;


	WSADATA wsa;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

	struct sockaddr_in server;
	struct sockaddr_in si_other;
	int slen;
};

