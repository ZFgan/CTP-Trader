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
1、加载套接字库，创建套接字（WSAStartup()/socket()）;
2、绑定套接字到一个IP地址和一个端口上（bind()）;
3、将套接字设置为监听模式等待连接请求（listen()）；
4、请求到来后，接受连接请求，返回一个新的对应于此次连接的套接字（accept()）；
5、用返回的套接字和客户端进行通信（send()/recv()）；
6、返回，等待另一个连接请求；
7、关闭套接字，关闭加载的套接字库（closesocket()/WSACleanup()）；
---------------------
原文参考：https://blog.csdn.net/xiaoquantouer/article/details/58001960
*/


// ---- 全局参数声明 ---- //
extern CustomTradeSpi *pTradeSpi;
extern int DAY_START_HOUR;
extern int DAY_START_MIN;
extern int DAY_END_HOUR;
extern int DAY_END_MIN;

Port::Port()
{
		//初始化WSA  
		WORD  sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			cout << "初始化WSA失败" << endl;
		}
		//创建套接字  
		SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (slisten == INVALID_SOCKET)
		{
			cout << "套接字建立失败!（socket）" << endl;
		}

		//绑定IP和端口  
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(8888);
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
		//int n = ::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin));
		if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			cout << "端口绑定失败（bind） !" << endl;
		}
		//开始监听  
		if (listen(slisten, 5) == SOCKET_ERROR)
		{
			cout << "监听失败（listen） !" << endl;
		}
		//循环接收数据  
		SOCKET sClient;
		sockaddr_in remoteAddr;
		int nAddrlen = sizeof(remoteAddr);
		char revData[255];
		//std::cout << "====交易主机启动====" << std::endl;
		while (true)
		{
			sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
			if (sClient == INVALID_SOCKET){cout << "与客户端连接错误！（accept）" << endl;}
			printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
			//接收数据 
			while (true)
			{
				lock_guard<mutex> guard(server);  //线程锁
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
					//判断当前所属时间段(time>=8:30 and time<=15:30)
					if ((ltm->tm_hour == DAY_START_HOUR && ltm->tm_min > DAY_START_MIN)
						|| (ltm->tm_hour > DAY_START_HOUR	&& ltm->tm_hour < DAY_END_HOUR)
						|| (ltm->tm_hour == DAY_END_HOUR && ltm->tm_min < DAY_END_MIN))
						recording = true;
					else
						recording = false;
					if (!recording)
					{
						cout << "非交易时间无法下单" << endl;
						const char * sendData = "not trade time";
						send(sClient, sendData, strlen(sendData), 0);	
					}
				    else if (count > 0 )
					{
						//发送数据  
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


