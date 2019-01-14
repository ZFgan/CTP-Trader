#include <iostream>
#include <string>
#include <time.h>
#include <thread>
#include <chrono>
#include "CustomTradeSpi.h"
#include "StrategyTrade.h"
using namespace std;

// ---- ȫ�ֲ������� ---- //
extern TThostFtdcBrokerIDType gBrokerID;                      // ģ�⾭���̴���
extern TThostFtdcInvestorIDType gInvesterID;                  // Ͷ�����˻���
extern TThostFtdcPasswordType gInvesterPassword;              // Ͷ��������
extern CThostFtdcTraderApi *g_pTradeUserApi;                  // ����ָ��
// ------�Ự����--------//
TThostFtdcOrderRefType	order_ref;	//��������

void CustomTradeSpi::OnFrontConnected()
{
	std::cout << "=====�����������ӳɹ�=====" << std::endl;
	// ��ʼ��¼
	reqUserLogin();
}

void CustomTradeSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====�������ӶϿ�=====" << std::endl;
	std::cerr << "�����룺 " << nReason << std::endl;
}

void CustomTradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====����������ʱ=====" << std::endl;
	std::cerr << "���ϴ�����ʱ�䣺 " << nTimeLapse << std::endl;
}

void CustomTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	isErrorRspInfo(pRspInfo);
}

void CustomTradeSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (!isErrorRspInfo(pRspInfo))
	{
		std::cout << "=====�˻���¼�ɹ�=====" << std::endl;
		std::cout << "�����գ� " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "��¼ʱ�䣺 " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "�ʻ����� " << pRspUserLogin->UserID << std::endl;
		// ����Ự����
		std::cout << "���������źż��" << std::endl;
		// Ͷ���߽�����ȷ��
		//reqSettlementInfoConfirm();
	}
}

void CustomTradeSpi::reqUserLogin()
{
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvesterID);
	strcpy(loginReq.Password, gInvesterPassword);
	static int requestID = 0; //������
	int rt = g_pTradeUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		cout << "====���͵�½����ɹ�====" << endl;
	else
		cerr << "���͵�½����ʧ��,������룺" << rt << endl;
}

void CustomTradeSpi::OnRtnOrder(CThostFtdcOrderField * pOrder)
{
	//ί�к����ľ���ϸ�ڴ˴������������Ϊ�������ֱ�ӿ��ͻ���
	//������������Լ�����ί�к�
	//cout << pOrder->InstrumentID << "  "<<"ί�кţ�" << pOrder->OrderSysID << endl;
}

void CustomTradeSpi::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	cout << pTrade->InstrumentID << "  " << "�ѳɽ�" << endl;
}
void CustomTradeSpi::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}
void CustomTradeSpi::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

bool CustomTradeSpi::isErrorRspInfo(CThostFtdcRspInfoField * pRspInfo)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

//�µ�����
void CustomTradeSpi::reqOrderInsert( TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction)
{
	CThostFtdcInputOrderField orderInsertReq;//�µ��ṹ

	//�˻�����
	memset(&orderInsertReq, 0, sizeof(orderInsertReq));
	///���͹�˾����
	strcpy(orderInsertReq.BrokerID, gBrokerID);
	///Ͷ���ߴ���
	strcpy(orderInsertReq.InvestorID, gInvesterID);
	///��Լ����
	strcpy(orderInsertReq.InstrumentID, instrumentID);
	///��������
	strcpy(orderInsertReq.OrderRef, order_ref);

	//�۸�ͽ�������
	///������1
	orderInsertReq.VolumeTotalOriginal = volume;
	///�۸�
	orderInsertReq.LimitPrice = price;
	///�����۸�����: �޼�
	orderInsertReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///��Ч������: ������Ч
	orderInsertReq.TimeCondition = THOST_FTDC_TC_GFD;


	//��ƽ����
	if (direction == "buy")
	{
		cout << "���Խ��յ�buy" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Buy;       //����
		orderInsertReq.TimeCondition = THOST_FTDC_OF_Open; //����
	}
	if (direction == "sell" )
	{	
		cout << "���Խ��յ�sell" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Sell;
		orderInsertReq.TimeCondition = THOST_FTDC_OF_CloseToday;//ƽ������������
	}
	if (direction == "short")
	{
		cout << "���Խ��յ�short" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Sell;
		orderInsertReq.CombOffsetFlag[0] = THOST_FTDC_OF_Open; 
	}
	if (direction == "cover")
	{
		cout << "���Խ��յ�cover" << endl;
		orderInsertReq.Direction = THOST_FTDC_D_Buy;
		orderInsertReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	}


	//BPK SPK��Ҫ���ⶨ�壬ģ�⽻�ײ�֧���м۵�
	if (price == 0)
	{
		///�����۸�����: �޼�
		orderInsertReq.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		//�м���������
		orderInsertReq.TimeCondition = THOST_FTDC_TC_IOC;
	}


	//�������ã������ϻ�������ı䣬Ĭ�ϼ���
	///���Ͷ���ױ���־
	orderInsertReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///�ɽ�������: �κ�����
	orderInsertReq.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	orderInsertReq.MinVolume = 1;
	///��������: ����
	orderInsertReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ǿƽԭ��: ��ǿƽ
	orderInsertReq.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	orderInsertReq.IsAutoSuspend = 0;
	///�û�ǿ����־: ��
	orderInsertReq.UserForceClose = 0;


	//�µ�
	static int requestID = 0;//������
	int rt = g_pTradeUserApi->ReqOrderInsert(&orderInsertReq, ++requestID);
	if (rt == -1)
		cout << "====��������ʧ��====" << endl;
	else if (rt == -2)
		cout << "====δ�������󳬹������====" << endl;
	else if (rt == -3)
		cout << "====ÿ�뷢�����������������Ŀ====" << endl;
	else if (rt == 0)
		cout << "�ѷ��Ͷ���" << endl;
}








