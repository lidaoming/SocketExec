// �ص�IO.cpp : �������̨Ӧ�ó������ڵ㡣
//��bug 
//bug �龰
//���ж���ͻ������ӵ�ʱ���˳�����һ����Ȼ�����´�һ���µĿͻ��ˣ���ʱֱ�Ӳ����ÿͻ��˷�����Ϣ������˾ͻ���Զ���ܲ����ÿͻ�����Ϣ
//��������´��µĿͻ��ˣ������л��������ͻ��˷�����Ϣ��Ȼ���л�����������

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

//���ܻ�����
char g_strRecv[MAX_RECV_COUNT];

//��������
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



	//�첽Socket
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
		//���������
		WSACleanup();
		return 0;
	}

	//�����첽��ʱ����
	while (1)
	{
		for (int i = 0; i < g_count;i++)
		{
			int nres=WSAWaitForMultipleEvents(1, &(g_allOlp[i].hEvent), FALSE, 0, FALSE);
			if (WSA_WAIT_FAILED == nres || WSA_WAIT_TIMEOUT == nres)
			{
				continue;
			}
			//���ź���
			DWORD dwState;
			DWORD dwFlag;
			BOOL bFlag = WSAGetOverlappedResult(g_allSock[i], &g_allOlp[i], &dwState, TRUE, &dwFlag);
			//�ź��ÿ�
			WSAResetEvent(g_allOlp[i].hEvent);

			if (FALSE == bFlag)
			{
				
				int a = WSAGetLastError();
				if (10054 == a)
				{
					printf("force close\n");
					//�ͻ�������
					//�ر�
					closesocket(g_allSock[i]);
					WSACloseEvent(g_allOlp[i].hEvent);
					//��������ɾ��
					g_allSock[i] = g_allSock[g_count - 1];
					g_allOlp[i] = g_allOlp[g_count - 1];
					//ѭ�����Ʊ���-1
					i--;
					//������-1
					g_count--;
				}
				continue;
			}

			if (i==0)
			{
				printf("accept\n");
				//����������� i=0  serversocket=allsocket[0(����i)]
				//Ͷ��accept
				PostRecv(g_count);
				//�������Ͷ��send
				//�ͻ�������++
				g_count++;
				PostAccept();
				continue;
			}


			if (0 == dwState)
			{
				printf("close\n");
				//�ͻ�������
				//�ر�
				closesocket(g_allSock[i]);
				WSACloseEvent(g_allOlp[i].hEvent);
				//��������ɾ��
				g_allSock[i] = g_allSock[g_count - 1];
				g_allOlp[i] = g_allOlp[g_count - 1];
				//ѭ�����Ʊ���-1
				i--;
				//������-1
				g_count--;
				continue;
			}
			
			if (dwState!=0)
			{
				//���ͻ��߽��ܳɹ���
				//�ж��ǽ��ܺ��·���
				if (g_strRecv[0]!=0)
				{
					printf("%s\n", g_strRecv);
					memset(g_strRecv, 0, 1024);
					//����sendһЩ����
					//�ٴε���PostRecv������һ��ֻ�ܽ���һ�����ݣ����ܺ󻹵ü�������
					PostRecv(i);
				}
				else
				{
					//������Ϣ
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
	//�����ͻ��˵�socket
	g_allSock[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_allOlp[g_count].hEvent = WSACreateEvent();
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
		//Ͷ��accept ��AcceptEx ����һ��ֻ�ܶ�Ӧһ���ͻ��ˣ�����֮����Ҫ��������Ͷ�ݣ�Ȼ����գ�
		PostAccept();
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
		//���������
		printf("%s\n",wsabuf.buf);
		memset(wsabuf.buf, 0,1024);
		//����sendһЩ����
		//�ٴε���PostRecv������һ��ֻ�ܽ���һ�����ݣ����ܺ󻹵ü�������
		PostRecv(index);
		return 0;
	}

	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//��ʱ����
			return 0;
		}
		else
		{
			return a;
		}
	}


}


//����socket
void Clear()
{
	for (int i = 0; i < g_count; i++)
	{
		closesocket(g_allSock[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
	}
}