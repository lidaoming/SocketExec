// Server1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include<stdio.h>
#include "stdafx.h"
//��������ͷ�ļ�

#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")


int _tmain(int argc, _TCHAR* argv[])
{
	//ʹ�������İ汾
	//WORD wdVserion = MAKEWORD(2, 1);//2.1
	WORD wdVserion = MAKEWORD(2, 2);//2.2
	WSADATA wdSockMsg;
	int nres=WSAStartup(wdVserion, &wdSockMsg);
	if (nres!=0)
	{
		return 0;
	}

	//�汾У��
	nres=WSAStartup(wdVserion, &wdSockMsg);
	if (2 != HIBYTE(wdSockMsg.wVersion) || 2 != LOBYTE(wdSockMsg.wVersion))
	{
		//�汾����
		//�ر�
		WSACleanup();
		return 0;
	}


	//����socket  TCP
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketServer==INVALID_SOCKET)
	{
		//ʧ��
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
		//ִ�г���

		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//listen ����

	int lres=listen(socketServer, SOMAXCONN);//SOMAXCONN  ������������
	if (lres == SOCKET_ERROR)
	{
		//��������
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//accept  ��ȡ�ͻ��˵�socket
	SOCKET client;
	//�ͻ���������Ϣ�ṹ��
	sockaddr_in clientMsg;
	//�ͻ��˽ṹ���С
	int len_clientMsg = sizeof(clientMsg);
	client = accept(socketServer, (sockaddr *)(&clientMsg), &len_clientMsg);
	if (client==INVALID_SOCKET)
	{
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	printf("�ͻ�������\n");

	//֪ͨ�ͻ���
	send(client, "���Ƿ���������֪����������\n", sizeof("���Ƿ���������֪����������\n"), 0);


	while (1)
	{
		//recv  ��ȡϵͳЭ�黺�����е�����
		char buf[1024] = { 0 };
		int rres = recv(client, buf, 1023, 0);
		//rres =0 �ͻ�������
		if (rres == 0)
		{
			//�ͻ������� �����ж�
			closesocket(client);
			return 0;
		}
		else if (rres == SOCKET_ERROR)
		{
			//����
			closesocket(client);
			return 0;

		}
		else
		{
			printf("�������յ����ݣ�%s ===���� %d\n", buf, rres);

		}
		//sned
		//��client�ͻ��˷�����Ϣ
		TCHAR sendbuf[1024] = { 0 };
		scanf("%s", sendbuf);
		//sprintf(sendbuf, "������յ���Ϣ: %s", buf);
		if (SOCKET_ERROR == send(client, sendbuf, strlen(sendbuf), 0))
		{

			//���ͳ���
			return 0;
		}


	}
	


	//������
	closesocket(client);
	closesocket(socketServer);
	WSACleanup();

	system("pause");
	return 0;
}

