// Client1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{

	//ʹ�������İ汾
	//WORD wdVserion = MAKEWORD(2, 1);//2.1
	WORD wdVserion = MAKEWORD(2, 2);//2.2
	WSADATA wdSockMsg;
	int nres = WSAStartup(wdVserion, &wdSockMsg);
	if (nres != 0)
	{
		return 0;
	}

	//�汾У��
	nres = WSAStartup(wdVserion, &wdSockMsg);
	if (2 != HIBYTE(wdSockMsg.wVersion) || 2 != LOBYTE(wdSockMsg.wVersion))
	{
		//�汾����
		//�ر�
		WSACleanup();
		return 0;
	}


	//����������socket  
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketServer == INVALID_SOCKET)
	{
		//ʧ��
		WSACleanup();
		return 0;

	}

	//���ӷ�����  
	sockaddr_in serverMsg;
	serverMsg.sin_family = AF_INET;
	serverMsg.sin_port = htons(12345);
	serverMsg.sin_addr.s_addr = inet_addr("127.0.0.1");


	if (SOCKET_ERROR == connect(socketServer, (sockaddr*)(&serverMsg), sizeof(serverMsg)))
	{
		//����
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	char buf1[1024] = { 0 };
	int lres=recv(socketServer, buf1, 1023, 0);

	printf("�ͻ����յ�: %s", buf1);
	//������Ϣ
	if (SOCKET_ERROR == send(socketServer, "���ǿͻ���", strlen("���ǿͻ���"), 0))
	{

		//���ͳ���
		return 0;
	}



	//������
	
	closesocket(socketServer);
	WSACleanup();
	system("pause");
	return 0;
}

