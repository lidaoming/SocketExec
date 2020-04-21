// 重叠IO.cpp : 定义控制台应用程序的入口点。
//有bug 
//bug 情景
//当有多个客户端连接的时候，退出其中一个，然后重新打开一个新的客户端，此时直接操作该客户端发送消息，服务端就会永远接受不到该客户端消息
//而如果重新打开新的客户端，但是切换到其他客户端发送信息，然后切回来就正常。

#include "stdafx.h"
#include <Winsock2.h>
#include <mswsock.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#define MAX_COUNT  1024
#define MAX_RECV_COUNT  1024
SOCKET g_allSock[MAX_COUNT];
OVERLAPPED g_allOlp[MAX_COUNT];
int g_count;

//接受缓冲区
char g_strRecv[MAX_RECV_COUNT];

//函数声明
int PostAccept();

int PostRecv(int index);


void Clear();




int _tmain(int argc, _TCHAR* argv[])
{
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScokMsg;
	int nRes = WSAStartup(wdVersion, &wdScokMsg);
	if (2 != HIBYTE(wdScokMsg.wVersion) || 2 != LOBYTE(wdScokMsg.wVersion))
	{
		WSACleanup();
		return 0;
	}



	//异步Socket
	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return 0;
	}


	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr *)&si, sizeof(si)))
	{
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	g_allSock[g_count] = socketServer;
	g_allOlp[g_count].hEvent = WSACreateEvent();
	g_count++;
	if (0 != PostAccept())
	{
		Clear();
		//清理网络库
		WSACleanup();
		return 0;
	}

	//进入异步延时处理
	while (1)
	{
		for (int i = 0; i < g_count;i++)
		{
			int nres=WSAWaitForMultipleEvents(1, &(g_allOlp[i].hEvent), FALSE, 0, FALSE);
			if (WSA_WAIT_FAILED == nres || WSA_WAIT_TIMEOUT == nres)
			{
				continue;
			}
			//有信号了
			DWORD dwState;
			DWORD dwFlag;
			BOOL bFlag = WSAGetOverlappedResult(g_allSock[i], &g_allOlp[i], &dwState, TRUE, &dwFlag);
			//信号置空
			WSAResetEvent(g_allOlp[i].hEvent);

			if (FALSE == bFlag)
			{
				
				int a = WSAGetLastError();
				if (10054 == a)
				{
					printf("force close\n");
					//客户端下线
					//关闭
					closesocket(g_allSock[i]);
					WSACloseEvent(g_allOlp[i].hEvent);
					//从数组中删掉
					g_allSock[i] = g_allSock[g_count - 1];
					g_allOlp[i] = g_allOlp[g_count - 1];
					//循环控制变量-1
					i--;
					//个数减-1
					g_count--;
				}
				continue;
			}

			if (i==0)
			{
				printf("accept\n");
				//接受链接完成 i=0  serversocket=allsocket[0(就是i)]
				//投递accept
				PostRecv(g_count);
				//根据情况投递send
				//客户端适量++
				g_count++;
				PostAccept();
				continue;
			}


			if (0 == dwState)
			{
				printf("close\n");
				//客户端下线
				//关闭
				closesocket(g_allSock[i]);
				WSACloseEvent(g_allOlp[i].hEvent);
				//从数组中删掉
				g_allSock[i] = g_allSock[g_count - 1];
				g_allOlp[i] = g_allOlp[g_count - 1];
				//循环控制变量-1
				i--;
				//个数减-1
				g_count--;
				continue;
			}
			
			if (dwState!=0)
			{
				//发送或者接受成功了
				//判断是接受好事发送
				if (g_strRecv[0]!=0)
				{
					printf("%s\n", g_strRecv);
					memset(g_strRecv, 0, 1024);
					//可以send一些东西
					//再次调用PostRecv，调用一次只能接受一次数据，接受后还得继续调用
					PostRecv(i);
				}
				else
				{
					//发送消息
				}
			}
			
		}
	}
	return 0;
}



int PostAccept()
{
	char str[1024] = { 0 };
	DWORD dwRecvcount;
	//创建客户端的socket
	g_allSock[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_allOlp[g_count].hEvent = WSACreateEvent();
	BOOL bRes = AcceptEx(g_allSock[0], g_allSock[g_count], str, 0, sizeof(struct sockaddr_in) + 16,
		sizeof(struct sockaddr_in) + 16, &dwRecvcount, &g_allOlp[0]);
	if (TRUE == bRes)
	{
		//立即完成了
		//投递recv
		PostRecv(g_count);
		//根据情况投递send
		//客户端适量++
		g_count++;
		//投递accept （AcceptEx 函数一次只能对应一个客户端，用完之后需要进行重新投递，然后接收）
		PostAccept();
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//延迟处理
			return 0;
		}
		else
		{
			return a;
		}
	}


}


int PostRecv(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = g_strRecv;
	wsabuf.len = 1024;
	DWORD dwRecvCount=0;
	DWORD dwFlag = 0;
	int nRes = WSARecv(g_allSock[index], &wsabuf, 1, &dwRecvCount, &dwFlag, &g_allOlp[index], NULL);
	if (nRes==0)
	{
		//立即完成了
		printf("%s\n",wsabuf.buf);
		memset(wsabuf.buf, 0,1024);
		//可以send一些东西
		//再次调用PostRecv，调用一次只能接受一次数据，接受后还得继续调用
		PostRecv(index);
		return 0;
	}

	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//延时处理
			return 0;
		}
		else
		{
			return a;
		}
	}


}


//清理socket
void Clear()
{
	for (int i = 0; i < g_count; i++)
	{
		closesocket(g_allSock[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
	}
}