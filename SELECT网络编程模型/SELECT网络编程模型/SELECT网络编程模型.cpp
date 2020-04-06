// SELECT������ģ��.cpp : �������̨Ӧ�ó������ڵ㡣
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


	//����socket  TCP
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketServer == INVALID_SOCKET)
	{
		//ʧ��
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
		//ִ�г���

		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//listen ����

	int lres = listen(socketServer, SOMAXCONN);//SOMAXCONN  ������������
	if (lres == SOCKET_ERROR)
	{
		//��������
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//selectģ��
	//����allSocket���飬���������еĿͻ��˺ͷ�������Socket
	fd_set allSockets;
	//��ʼ��
	FD_ZERO(&allSockets);
	//���ཫ��������Socketװ������
	FD_SET(socketServer, &allSockets);


	//ѭ��  
	while (true)
	{
		timeval timeout;
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		//������ʱ��������� select�����Ὣ��ı�
		fd_set tmpallSockets = allSockets;
		//��ͣ��select 
		//MessageBox(0, L"select", 0, 0);
		int nres = select(0, &tmpallSockets, NULL, NULL, &timeout);

		if (nres == 0)
		{
			//����û���¼�����
			continue;
		}


		else if (nres > 0)
		{
			//���¼������������Զ������
			//����
			for (size_t i = 0; i < tmpallSockets.fd_count; i++)
			{
				if (tmpallSockets.fd_array[i] == socketServer)
				{
					SOCKET clientSocket = accept(socketServer, NULL, NULL);
					if (clientSocket == INVALID_SOCKET)
					{
						//����
						continue;
					}
					FD_SET(clientSocket, &allSockets);
					printf("���µĿͻ�������\n");
				}
				//�ͻ����׽���
				else
				{
					char bufstr[1024] = { 0 };
					
					int nrecv = recv(tmpallSockets.fd_array[i], bufstr, sizeof(bufstr), 0);
					if (nrecv > 0)
					{
						printf("CLIENT===>Server :%s\n", bufstr);
					}
					else if (nrecv == 0)
					{
						//�ͻ�������
						SOCKET deltmpSocket = tmpallSockets.fd_array[i];
						FD_CLR(tmpallSockets.fd_array[i], &allSockets);
						closesocket(deltmpSocket);
					}
					else if (nrecv == SOCKET_ERROR)
					{
						//���ܳ���
						continue;
					}
				}
			}
			

		}
		else
		{
			//��������
			;
		}
	}




	return 0;
}

