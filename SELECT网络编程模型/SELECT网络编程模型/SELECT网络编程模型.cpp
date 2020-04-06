// SELECT网络编程模型.cpp : 定义控制台应用程序的入口点。
//


#include<stdio.h>
#include "stdafx.h"
//引入网络头文件

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


	//创建socket  TCP
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketServer == INVALID_SOCKET)
	{
		//失败
		WSACleanup();
		return 0;

	}

	//bind
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(12345);


	int bres = bind(socketServer, (const sockaddr *)(&service), sizeof(service));
	if (bres == SOCKET_ERROR)
	{
		//执行出错

		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//listen 监听

	int lres = listen(socketServer, SOMAXCONN);//SOMAXCONN  最大队列连接数
	if (lres == SOCKET_ERROR)
	{
		//监听出错
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//select模型
	//创建allSocket数组，里面存放所有的客户端和服务器的Socket
	fd_set allSockets;
	//初始化
	FD_ZERO(&allSockets);
	//首相将服务器的Socket装入数组
	FD_SET(socketServer, &allSockets);


	//循环  
	while (true)
	{
		timeval timeout;
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		//创建临时填入的数组 select函数会将其改变
		fd_set tmpallSockets = allSockets;
		//不停的select 
		int nres= select(0, &allSockets, &allSockets, NULL, &timeout);

		if (nres == 0)
		{
			//代表没有事件发生
			continue;
		}
		else if (nres > 0)
		{
			//有事件发生，处理自定义代码

		}
		else
		{
			//发生错误
		}
	}




	return 0;
}

