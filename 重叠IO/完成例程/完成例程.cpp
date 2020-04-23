// �������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#include <Winsock2.h>
#include <mswsock.h>

#include <string.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")


#define MAX_COUNT  1024
#define MAX_RECV_COUNT  1024

SOCKET g_allSock[MAX_COUNT];
OVERLAPPED g_allOlp[MAX_COUNT];
int g_count;

//���ջ�����
char g_strRecv[MAX_RECV_COUNT];

int PostAccept();
int PostRecv(int index);
int PostSend(int index);
void Clear();

int _tmain(int argc, _TCHAR* argv[])
{
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScokMsg;
	int nRes = WSAStartup(wdVersion, &wdScokMsg);
	if (0 != nRes)
	{
		return 0;
	}

	//У��汾
	if (2 != HIBYTE(wdScokMsg.wVersion) || 2 != LOBYTE(wdScokMsg.wVersion))
	{
		//˵���汾����
		//���������
		WSACleanup();
		return 0;
	}
	

	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	//int a = WSAGetLastError();
	if (INVALID_SOCKET == socketServer)
	{
		int a = WSAGetLastError();
		//���������
		WSACleanup();
		return 0;
	}
	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr *)&si, sizeof(si)))
	{
		//������
		int a = WSAGetLastError();
		//�ͷ�
		closesocket(socketServer);
		//���������
		WSACleanup();
		return 0;
	}
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		//������
		int a = WSAGetLastError();
		//�ͷ�
		closesocket(socketServer);
		//���������
		WSACleanup();
		return 0;
	}

	g_allSock[g_count] = socketServer;
	g_allOlp[g_count].hEvent = WSACreateEvent();
	g_count++;

	if (0 != PostAccept())
	{
		Clear();
		//���������
		WSACleanup();
		return 0;
	}

	while (1)
	{
		//ֻҪ�ȷ�������socket�źž��У��ͻ��˵Ľ����ص��ص�������
		int nRes = WSAWaitForMultipleEvents(1, &(g_allOlp[0].hEvent), FALSE, WSA_INFINITE, TRUE);
		if (WSA_WAIT_FAILED == nRes || WSA_WAIT_IO_COMPLETION == nRes)
		{
			continue;
		}


		
		//�ź��ÿ�
		WSAResetEvent(g_allOlp[0].hEvent);
		//PostSend(g_count);
		printf("accept\n");
		//�������������
		//Ͷ��recv
		PostRecv(g_count);
		//�������Ͷ��send
		//�ͻ�������++
		g_count++;
		//Ͷ��accept
		PostAccept();
	}
	
}


void Clear()
{
	for (int i = 0; i < g_count; i++)
	{
		closesocket(g_allSock[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
	}
}


int PostAccept()
{
	while (1)
	{
		g_allSock[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		g_allOlp[g_count].hEvent = WSACreateEvent();

		char str[1024] = { 0 };
		DWORD dwRecvcount;

		BOOL bRes = AcceptEx(g_allSock[0], g_allSock[g_count], str, 0, sizeof(struct sockaddr_in) + 16,
			sizeof(struct sockaddr_in) + 16, &dwRecvcount, &g_allOlp[0]);

		if (TRUE == bRes)
		{
			//���������
			//Ͷ��recv
			PostRecv(g_count);
			//�������Ͷ��send
			//�ͻ�������++
			g_count++;
			//Ͷ��accept
			//PostAccept();
			continue;
		}
		else
		{
			int a = WSAGetLastError();
			if (ERROR_IO_PENDING == a)
			{
				//�ӳٴ���
				break;
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}


void CALLBACK RecvCall(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{


	int i = lpOverlapped - &g_allOlp[0];

	if (10054 == dwError || 0 == cbTransferred)
	{
		//ɾ���ͻ���
		printf("close\n");
		//�ͻ�������
		//�ر�
		closesocket(g_allSock[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
		//��������ɾ��
		g_allSock[i] = g_allSock[g_count];
		g_allOlp[i] = g_allOlp[g_count];
		//������-1
		g_count--;
	}
	else
	{
		printf("%s\n", g_strRecv);
		memset(g_strRecv, 0, MAX_RECV_COUNT);
		//�������Ͷ��send
		//���Լ�Ͷ�ݽ���
		PostRecv(i);
	}
}





int PostRecv(int index)
{

	while (1)
	{
		WSABUF wsabuf;
		wsabuf.buf = g_strRecv;
		wsabuf.len = MAX_RECV_COUNT;

		DWORD dwRecvCount;
		DWORD dwFlag = 0;
		int nRes = WSARecv(g_allSock[index], &wsabuf, 1, &dwRecvCount, &dwFlag, &g_allOlp[index], RecvCall);
		if (0 == nRes)
		{
			//������ɵ�
			//��ӡ��Ϣ
			printf("%s\n", wsabuf.buf);
			memset(g_strRecv, 0, MAX_RECV_COUNT);
			//�������Ͷ��send
			//���Լ�Ͷ�ݽ���
			//PostRecv(index);
			return 0;
		}
		else
		{
			int a = WSAGetLastError();
			if (ERROR_IO_PENDING == a)
			{
				//�ӳٴ���
				return 0;
			}
			else
			{
				return a;
			}
		}
	}
	
}



void CALLBACK SendCall(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	printf("send over\n");
}

int PostSend(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = "���";
	wsabuf.len = MAX_RECV_COUNT;

	DWORD dwSendCount;
	DWORD dwFlag = 0;
	int nRes = WSASend(g_allSock[index], &wsabuf, 1, &dwSendCount, dwFlag, &g_allOlp[index], SendCall);

	if (0 == nRes)
	{
		//������ɵ�
		//��ӡ��Ϣ
		printf("send�ɹ�\n");

		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//�ӳٴ���
			return 0;
		}
		else
		{
			return a;
		}
	}
}