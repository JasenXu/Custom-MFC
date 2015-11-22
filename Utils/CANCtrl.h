#pragma once
#include "ControlCAN.h"
#pragma comment(lib, "ControlCAN.lib")

// CAN读取失败返回码
#define CAN_ERROR		0xffffffff

class CCANCtrl
{
public:
	CCANCtrl(void);
	virtual ~CCANCtrl(void);

protected:
	DWORD	m_dwDeviceType;			// 设备类型，默认为USBCAN2（m_dwDeviceType = 4）
	DWORD	m_dwDeviceIndex;		// 设备索引号
	BOOL	m_bWork;				// 设备运行状态标识，TRUE：已启动，FALSE：未启动

	typedef enum tagBaudRate
	{
		BR_5 = 5, BR_10 = 10, BR_20 = 20, BR_40 = 40, BR_50 = 50, BR_80 = 80, 
		BR_100 = 100, BR_125 = 125, BR_200 = 200, BR_250 = 250, BR_400 = 400, 
		BR_500 = 500, BR_666 = 666, BR_800 = 800, BR_1000 = 1000
	};

public:
	//	打开设备
	BOOL Open(DWORD dwDevType, DWORD dwDevIndex = 0);

	//	初始化CAN
	BOOL Init(DWORD dwCANIndex, UINT nBaudRate);

	//	启动CAN
	BOOL Start(DWORD dwCANIndex);

	//	复位CAN
	BOOL Reset(DWORD dwCANIndex);

	//	关闭设备
	void Close(void);

	//	获取设备运行状态
	BOOL GetStatus(void);

	//	获取硬件信息
	BOOL GetDeviceInfo(PVCI_BOARD_INFO pInfo);

	// 获取错误
	UINT GetLastError(DWORD dwCANIndex);

	// 发送数据
	ULONG Send(DWORD dwCANIndex, PVCI_CAN_OBJ pSend, ULONG ulLen);

	// 接收数据
	ULONG Receive(DWORD dwCANIndex, PVCI_CAN_OBJ pReceive, ULONG ulLen, int nWaitTime = 400);

protected:
	void GetTimerValue(__in UINT nBaudrate, __out UCHAR * pTimer0, __out UCHAR * pTimer1);
};

