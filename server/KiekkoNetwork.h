#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <vector>

class KiekkoNetwork
{
public:
	struct SendPackage
	{
		float player1Pos;
		float player2Pos;
		float ballX, ballY;
		float ballXVel, ballYVel;
	};

	struct ReceivePackage
	{
		float player1Pos;
		float player2Pos;
	};


	~KiekkoNetwork(){
		closesocket(ListenSocket);
		for (int i = 0; i < activeSocket.size(); i++)
			closesocket(*activeSocket[i]);
		WSACleanup();
	};

	static KiekkoNetwork* GetInstance();
	
	int SendMsg(SendPackage pckg);
	int InitializeNetwork();
	void Update();
	
	ReceivePackage GetLatestPackage();
	ReceivePackage latestPackage;
	
	std::vector<SOCKET*> activeSocket;
	bool newPackage;


private:

	KiekkoNetwork(){};
	static KiekkoNetwork* instance;
	

	void InitValues();
	char* CreateMessage(SendPackage pckg, int id);

	int sendLength;
	int recvLength;


	WSADATA wsa;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

	struct sockaddr_in si_other;
	int slen;

	bool paskafix;
};

