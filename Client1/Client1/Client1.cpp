// Client1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{

	//使用网络库的版本
	//WORD wdVserion = MAKEWORD(2, 1);//2.1
	WORD wdVserion = MAKEWORD(2, 2);//2.2
	WSADATA wdSockMsg;
	int nres = WSAStartup(wdVserion, &wdSockMsg);
	if (nres != 0)
	{
		return 0;
	}

	//版本校验
	nres = WSAStartup(wdVserion, &wdSockMsg);
	if (2 != HIBYTE(wdSockMsg.wVersion) || 2 != LOBYTE(wdSockMsg.wVersion))
	{
		//版本不对
		//关闭
		WSACleanup();
		return 0;
	}


	//创建服务器socket  
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketServer == INVALID_SOCKET)
	{
		//失败
		WSACleanup();
		return 0;

	}

	//链接服务器  
	sockaddr_in serverMsg;
	serverMsg.sin_family = AF_INET;
	serverMsg.sin_port = htons(12345);
	serverMsg.sin_addr.s_addr = inet_addr("127.0.0.1");


	if (SOCKET_ERROR == connect(socketServer, (sockaddr*)(&serverMsg), sizeof(serverMsg)))
	{
		//出错
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}



	while (1)
	{
		char sendbuf[1024] = { 0 };
		char recvbuf[1024] = { 0 };
		//发送消息
		scanf("%s", sendbuf);
		if (SOCKET_ERROR == send(socketServer, sendbuf, strlen(sendbuf), 0))
		{

			//发送出错
			return 0;
		}

		//recv(socketServer, recvbuf, 1023, 0);
		//printf("客户端收到: %s\n", recvbuf);



	}




	//清理工作
	
	closesocket(socketServer);
	WSACleanup();
	system("pause");
	return 0;
}

