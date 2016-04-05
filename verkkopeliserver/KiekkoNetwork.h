#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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

	~KiekkoNetwork();

	static KiekkoNetwork* GetInstance();

	int SendMsg(SendPackage pckg);
	int InitializeNetwork();
	int Update(int threadCount);
	void CloseConnections();

	ReceivePackage GetLatestPackage();
	ReceivePackage latestPackage;

	std::map<int, int*> activeSocket;
	static bool newPackage;

private:

	KiekkoNetwork();

	static KiekkoNetwork* instance;

	void InitValues();
	char* CreateMessage(SendPackage pckg, int id);

	int sendLength;
	int recvLength;

	int ListenSocket;
	int ClientSocket;


	bool paskafix;
};

