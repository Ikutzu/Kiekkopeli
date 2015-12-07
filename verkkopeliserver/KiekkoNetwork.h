#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <map>

class KiekkoNetwork
{
public:
	struct SendPackage
	{
		int player1Pos;
		int player2Pos;
		int ballX, ballY;
		int ballAngle, ballVelocity;
	};

	struct ReceivePackage
	{
		int player1Pos;
		int player2Pos;
	};


	~KiekkoNetwork()
	{
		closesocket(ListenSocket);
		for (int i = 0; i < activeSocket.size(); i++)
		{
			if (activeSocket[i] != nullptr)
				closesocket(*activeSocket[i]);
		}
		WSACleanup();
	};


	static KiekkoNetwork* GetInstance();
	static void DeleteInstance();
	
	int SendMsg(SendPackage pckg);
	int InitializeNetwork();
	void Update(int threadCount);
	void CloseConnections();
	
	ReceivePackage GetLatestPackage();
	ReceivePackage latestPackage;
	
	std::map<int, SOCKET*> activeSocket;
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

