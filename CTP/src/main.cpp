#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <thread>
//#include <windows.h>
#include "CustomTradeSpi.h"
#include "Port.h"
using namespace std;
//���ӿ�
#pragma comment (lib, "thosttraderapi.lib")
#pragma comment (lib, "thostmduserapi.lib")

//------------------------------- ȫ�ֱ��� -------------------------------------------- //
//ʹ��ȫ�ֱ���Υ�����������ĳ��ԣ��˴�������Ϊ�˷���
//��������
TThostFtdcBrokerIDType gBrokerID = "9999";                          // ģ�⾭���̴���
TThostFtdcInvestorIDType gInvesterID = "";                    // �����Լ���Ͷ�����˻�����sinmowģ���˻����ɣ�
TThostFtdcPasswordType gInvesterPassword = "";                // Ͷ��������
//���ײ���
CustomTradeSpi *pTradeSpi = new CustomTradeSpi;					   // �������׻ص�ʵ��
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;                    // ����ָ��
char gTradeFrontAddr[] = "tcp://180.168.146.187:10001";            // ģ�⽻��ǰ�õ�ַ
/*
	1�����˻���ʹ��XMLD��д  https://www.cnblogs.com/lyggqm/p/7204472.html
	2�����˻������ԣ���Ҫ�ȵ�½����˻���ÿ���˻��½�һ������ָ�뼴�ɣ�
*/

//����ֵ�ز���
//ҵ���ϵ�������Ҫҹ���µ�,������Լ��޸�
int DAY_START_HOUR=8, DAY_START_MIN=45, DAY_END_HOUR=15, DAY_END_MIN=30;

void thread_server()
{
	cout << "����������" << endl;
	Port *port = new Port();
	delete port;
};

void trader_server()
{

}
//----------------------------��������----------------------------------------------
int main()
{
	//����ֵ��,һ�ַ�ʽ��ʹ������������ֹ�̣߳����߳�ʹ��notify_all��������
	//�����ʹ��while��ʽ��ֹ
	//����һ���������̣߳�һֱ���У���ʵ����ֻ��Ҫ��ʱȡ�������̼߳���
	bool recording = false;
	bool thread_ = false;

	thread server(thread_server);  	       //������һ���̣߳�ʹ�÷�����һֱ����
	while (true)
	{
		time_t now = time(0);
		tm *ltm = localtime(&now);
		cout << ltm->tm_min <<":"<<ltm->tm_sec << "�Ƿ���ʱ�䣺"<<" "<<recording<<"�߳��Ƿ�������"<<thread_<<endl;
		Sleep(1000);
		recording = false;
		//�жϵ�ǰ����ʱ���(time>=8:30 and time<=15:30)
		if ((ltm->tm_hour == DAY_START_HOUR && ltm->tm_min > DAY_START_MIN)
			|| (ltm->tm_hour > DAY_START_HOUR	&& ltm->tm_hour < DAY_END_HOUR)
			|| (ltm->tm_hour == DAY_END_HOUR && ltm->tm_min < DAY_END_MIN))
			recording = true;
		else
			recording = false;

		if (recording  && !thread_)
		{
			cout << "������������" << endl;
			thread_ = true;
			g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // ��������ʵ��
			g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // ע���¼���
			g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // ���Ĺ�����
			g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // ����˽����
			g_pTradeUserApi->RegisterFront(gTradeFrontAddr);              // ���ý���ǰ�õ�ַ
			g_pTradeUserApi->Init();     //����
			//g_pTraderUserApi->Join();  //�̹߳���
		}
		if (!recording && thread_)
		{
			cout << "�رս�������" << endl;
			g_pTradeUserApi->Release();
			thread_ = false;
			cout << "���������رճɹ�" << endl;
		}	
	}
	delete pTradeSpi;
	server.join();
	return 0;
}
