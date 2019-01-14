#pragma once
// ---- �����Ľ����� ---- //
#include "CTP_API/ThostFtdcTraderApi.h"
#include <string>

using namespace std;

class CustomTradeSpi : public CThostFtdcTraderSpi
{
	// ---- ctp_api���ֻص��ӿ� ---- //
public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected();
	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///����Ӧ��
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	void OnFrontDisconnected(int nReason);
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	void OnHeartBeatWarning(int nTimeLapse);

	//------------------------------------�µ������ص�����------------------------------------------//
	//������
	//��ȷ�ı������� �ر�����OnRtnOrder��Ӧ
	//�ɽ���OnRtnTrade��Ӧ�� 
	//����������OnRspOrderInsert����Ӧ������������OnRspOrderAction
	///������֪ͨ��,���˴����������д�ӡ�µ���Լ��ί�кţ����㳷��ʹ��
	void OnRtnOrder(CThostFtdcOrderField *pOrder);
	///���ɽ�֪ͨ��
	void OnRtnTrade(CThostFtdcTradeField *pTrade);
	///����¼��������Ӧ
	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///��������������Ӧ
	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	// ---- �Զ��庯�� ---- //
public:
	bool isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo); // �Ƿ��յ�������Ϣ
	//�����Ի��µ�����Ͷ���ߴ��루���˻�������Լ���룬�۸��µ���������
	//֮�������Ҫ�������ID���Դﵽ��������˻���������µ�
	void reqOrderInsert(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume,string direction);
	//���������������Զ���Ƶ�Զ��������ƺ�������Ҫ�������ֹ�����
	//void reqOrderAction()
private:
	void reqUserLogin(); // ��¼����
	//��дһ���µ�����������
};