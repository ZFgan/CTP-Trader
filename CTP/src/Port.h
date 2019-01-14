#pragma once
#include <stdio.h>  
#include <mutex>
#include <winsock2.h> 
#include "CustomTradeSpi.h"
#pragma comment(lib,"ws2_32.lib")  
class Port
{
public:
	Port();
	~Port();
	void Normal_trader(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction);
	void No_trader(TThostFtdcInstrumentIDType instrumentID);
	void BPK(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction);
	void SPK(TThostFtdcInstrumentIDType instrumentID, TThostFtdcPriceType price, TThostFtdcVolumeType volume, string direction);
private:
	mutex server;
};



