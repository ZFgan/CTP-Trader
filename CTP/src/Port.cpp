#include "Port.h"
#include <iostream>
#include <stdio.h>  
#include <winsock2.h>
#include <ctime>
//#include <windows.h>
#include "CustomTradeSpi.h"
#pragma comment(lib,"ws2_32.lib")  
//using namespace std;

/*
1�������׽��ֿ⣬�����׽��֣�WSAStartup()/socket()��;
2�����׽��ֵ�һ��IP��ַ��һ���˿��ϣ�bind()��;
3�����׽�������Ϊ����ģʽ�ȴ���������listen()����
4���������󣬽����������󣬷���һ���µĶ�Ӧ�ڴ˴����ӵ��׽��֣�accept()����
5���÷��ص��׽��ֺͿͻ��˽���ͨ�ţ�send()/recv()����
6�����أ��ȴ���һ����������
7���ر��׽��֣��رռ��ص��׽��ֿ⣨closesocket()/WSACleanup()����
---------------------
ԭ�Ĳο���https://blog.csdn.net/xiaoquantouer/article/details/58001960
*/


// ---- ȫ�ֲ������� ---- //
extern CustomTradeSpi *pTradeSpi;
extern int DAY_START_HOUR;
extern int DAY_START_MIN;
extern int DAY_END_HOUR;
extern int DAY_END_MIN;

Port::Port()
{
		//��ʼ��WSA  
		WORD  sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			cout << "��ʼ��WSAʧ��" << endl;
		}
		//�����׽���  
		SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (slisten == INVALID_SOCKET)
		{
			cout << "�׽��ֽ���ʧ��!��socket��" << endl;
		}

		//��IP�Ͷ˿�  
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(8888);
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
		//int n = ::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin));
		if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			cout << "�˿ڰ�ʧ�ܣ�bind�� !" << endl;
		}
		//��ʼ����  
		if (listen(slisten, 5) == SOCKET_ERROR)
		{
			cout << "����ʧ�ܣ�listen�� !" << endl;
		}
		//ѭ����������  
		SOCKET sClient;
		sockaddr_in remoteAddr;
		int nAddrlen = sizeof(remoteAddr);
		char revData[255];
		//std::cout << "====������������====" << std::endl;
		while (true)
		{
			sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
			if (sClient == INVALID_SOCKET){cout << "��ͻ������Ӵ��󣡣�accept��" << endl;}
			printf("���ܵ�һ�����ӣ�%s \r\n", inet_ntoa(remoteAddr.sin_addr));
			//�������� 
			while (true)
			{
				lock_guard<mutex> guard(server);  //�߳���
				int ret = recv(sClient, revData, 255, 0);
				if (ret > 0)
				{
					string data[4], direction;
					int count = 0, lots = 0;
					double price = 0;
					revData[ret] = 0x00;
					for (int i = 0; i < ret; i++)
					{
						if (revData[i] != ' ')
						{
							data[count] += revData[i];
						}
						else
						{
							cout << data[count] << "  ";
							count++;
						}
					}
					cout << endl;
					char* instrumentID;
					int len = data[0].length();
					instrumentID = new char[len + 1];
					strcpy(instrumentID, data[0].data());
					direction = data[3];
					price = stod(data[1]);
					lots = stoi(data[2]);	

					time_t now = time(0);
					tm *ltm = localtime(&now);
					bool recording = false;
					//�жϵ�ǰ����ʱ���(time>=8:30 and time<=15:30)
					if ((ltm->tm_hour == DAY_START_HOUR && ltm->tm_min > DAY_START_MIN)
						|| (ltm->tm_hour > DAY_START_HOUR	&& ltm->tm_hour < DAY_END_HOUR)
						|| (ltm->tm_hour == DAY_END_HOUR && ltm->tm_min < DAY_END_MIN))
						recording = true;
					else
						recording = false;
					if (!recording)
					{
						cout << "�ǽ���ʱ���޷��µ�" << endl;
						const char * sendData = "not trade time";
						send(sClient, sendData, strlen(sendData), 0);	
					}
				    else if (count > 0 )
					{
						//��������  
						if (direction == "buy" || direction == "sell" || direction == "short" || direction == "cover")
						{
							Normal_trader(instrumentID, price, lots, direction);
						}
						if (direction == "bpk")
						{
							BPK(instrumentID, price, lots, direction);
						}
						if (direction == "spk")
						{
							SPK(instrumentID, price, lots, direction);
						}
						if (direction == "notrader")
						{
							No_trader(instrumentID);
						}
						string send_singal = data[0] + " send to "+direction;
						const char * sendData = send_singal.data();
						send(sClient, sendData, strlen(sendData), 0);
					}
					delete instrumentID;
				}
				else 
				{
						cout << "No Data" << endl;
						break;
				}
			}
			//closesocket(sClient);
		}
		closesocket(slisten);
		WSACleanup();
}



Port::~Port()
{

}


void Port::No_trader(TThostFtdcInstrumentIDType instrumentID)
{
	cout << instrumentID << " no trader" << endl;
}

void Port::BPK(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction)
{
	pTradeSpi->reqOrderInsert(instrumentID, 0, volume, "cover");
	Sleep(1000);
	pTradeSpi->reqOrderInsert(instrumentID, price, volume, "buy");
}

void Port::SPK(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction)
{
	pTradeSpi->reqOrderInsert(instrumentID, 0, volume, "sell");
	Sleep(1000);
	pTradeSpi->reqOrderInsert(instrumentID, price, volume, "short");
}


void Port::Normal_trader(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction)
{
	pTradeSpi->reqOrderInsert(instrumentID, price, volume, direction);
}


