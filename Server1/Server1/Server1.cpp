// Server1.cpp : 定义控制台应用程序的入口点。
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
	int nres=WSAStartup(wdVserion, &wdSockMsg);
	if (nres!=0)
	{
		return 0;
	}

	//版本校验
	nres=WSAStartup(wdVserion, &wdSockMsg);
	if (2 != HIBYTE(wdSockMsg.wVersion) || 2 != LOBYTE(wdSockMsg.wVersion))
	{
		//版本不对
		//关闭
		WSACleanup();
		return 0;
	}


	//创建socket  TCP
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketServer==INVALID_SOCKET)
	{
		//失败
		WSACleanup();
		return 0;

	}

	//bind
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr= inet_addr("127.0.0.1");
	service.sin_port = htons(12345);


	int bres =bind(socketServer, (const sockaddr *)(&service),sizeof(service));
	if (bres==SOCKET_ERROR)
	{
		//执行出错

		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//listen 监听

	int lres=listen(socketServer, SOMAXCONN);//SOMAXCONN  最大队列连接数
	if (lres == SOCKET_ERROR)
	{
		//监听出错
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//accept  获取客户端的socket
	SOCKET client;
	//客户端连接信息结构体
	sockaddr_in clientMsg;
	//客户端结构体大小
	int len_clientMsg = sizeof(clientMsg);
	client = accept(socketServer, (sockaddr *)(&clientMsg), &len_clientMsg);
	if (client==INVALID_SOCKET)
	{
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	printf("客户端上线\n");

	//通知客户端
	send(client, "我是服务器，我知道你上线了\n", sizeof("我是服务器，我知道你上线了\n"), 0);


	while (1)
	{
		//recv  获取系统协议缓冲区中的内容
		char buf[1024] = { 0 };
		int rres = recv(client, buf, 1023, 0);
		//rres =0 客户端下线
		if (rres == 0)
		{
			//客户端下线 连接中断
			closesocket(client);
			return 0;
		}
		else if (rres == SOCKET_ERROR)
		{
			//出错
			closesocket(client);
			return 0;

		}
		else
		{
			printf("服务器收到内容：%s ===长度 %d\n", buf, rres);

		}
		//sned
		//向client客户端发送消息
		TCHAR sendbuf[1024] = { 0 };
		scanf("%s", sendbuf);
		//sprintf(sendbuf, "服务端收到消息: %s", buf);
		if (SOCKET_ERROR == send(client, sendbuf, strlen(sendbuf), 0))
		{

			//发送出错
			return 0;
		}


	}
	


	//清理工作
	closesocket(client);
	closesocket(socketServer);
	WSACleanup();

	system("pause");
	return 0;
}

