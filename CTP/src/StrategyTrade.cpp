#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "StrategyTrade.h"
#include "CustomTradeSpi.h"



////���Բ��ô����ݿ��ȡ����
//extern std::unordered_map<std::string, TickToKlineHelper> g_KlineHash;
//
//// �̻߳�����
//std::mutex marketDataMutex;
//
//void StrategyCheckAndTrade(TThostFtdcInstrumentIDType instrumentID, CustomTradeSpi *customTradeSpi)
//{
//	// ����
//	std::lock_guard<std::mutex> lk(marketDataMutex);
//	TickToKlineHelper tickToKlineObject = g_KlineHash.at(std::string(instrumentID));
//	// ����
//	std::vector<double> priceVec = tickToKlineObject.m_priceVec;
//	//std::cout << priceVec.size() << std::endl;
//	if (priceVec.size() >= 3)
//	{
//		int len = priceVec.size();
//		//std::cout << priceVec[len - 1] << std::endl;
//		//����� ���򿪲�,��֮��������,������ʱ�����һ���۸��µ�
//		if (priceVec[len - 1] > priceVec[len - 2] && priceVec[len - 2] > priceVec[len - 3])
//			customTradeSpi->reqOrderInsert(instrumentID, priceVec[len - 1], 1, THOST_FTDC_D_Buy);
//		else if (priceVec[len - 1] < priceVec[len - 2] && priceVec[len - 2] < priceVec[len - 3])
//			customTradeSpi->reqOrderInsert(instrumentID, priceVec[len - 1], 1, THOST_FTDC_D_Buy);
//	}
//}
