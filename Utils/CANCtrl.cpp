#include "StdAfx.h"
#include "CANCtrl.h"


CCANCtrl::CCANCtrl(void)
	: m_dwDeviceType(VCI_USBCAN2)
	, m_dwDeviceIndex(0)
	, m_bWork(FALSE)
{
}


CCANCtrl::~CCANCtrl(void)
{
	Close();
}

//	----------------------------------------------------------------
//	打开设备
//	----------------------------------------------------------------
BOOL CCANCtrl::Open(DWORD dwDevType, DWORD dwDevIndex)
{
	m_dwDeviceIndex = dwDevIndex;
	m_dwDeviceType = dwDevType;

	DWORD dwRet = VCI_OpenDevice(m_dwDeviceType, m_dwDeviceIndex, 0);
	if (dwRet == STATUS_OK)
	{
		return (m_bWork = TRUE);
	}

	return (m_bWork = FALSE);
}

//	----------------------------------------------------------------
//	初始化CAN
//	----------------------------------------------------------------
BOOL CCANCtrl::Init(DWORD dwCANIndex, UINT nBaudRate)
{
	if (m_bWork)
	{
		VCI_INIT_CONFIG vic = {0};
		vic.AccCode	= 0x00000000;
		vic.AccMask	= 0xFFFFFFFF;
		vic.Filter	= 0x01;
		vic.Mode	= 0x00;
		GetTimerValue(nBaudRate, &vic.Timing0, &vic.Timing1);

		DWORD dwRet = VCI_InitCAN(m_dwDeviceType, m_dwDeviceIndex, dwCANIndex, &vic);
		if (dwRet == STATUS_OK)
		{
			return TRUE;
		}
	}
	
	return (m_bWork = FALSE);
}

//	----------------------------------------------------------------
//	启动CAN
//	----------------------------------------------------------------
BOOL CCANCtrl::Start(DWORD dwCANIndex)
{
	if (m_bWork)
	{
		DWORD dwRet = VCI_StartCAN(m_dwDeviceType, m_dwDeviceIndex, dwCANIndex);
		if (dwRet == STATUS_OK)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

//	----------------------------------------------------------------
//	复位CAN
//	----------------------------------------------------------------
BOOL CCANCtrl::Reset(DWORD dwCANIndex)
{
	if (m_bWork)
	{
		DWORD dwRet = VCI_ResetCAN(m_dwDeviceType, m_dwDeviceIndex, dwCANIndex);
		if (dwRet != STATUS_OK)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

//	----------------------------------------------------------------
//	关闭设备
//	----------------------------------------------------------------
void CCANCtrl::Close(void)
{
	if (m_bWork)
	{
		VCI_ResetCAN(m_dwDeviceType, m_dwDeviceIndex, 0);		
		VCI_ResetCAN(m_dwDeviceType, m_dwDeviceIndex, 1);

		VCI_CloseDevice(m_dwDeviceType, m_dwDeviceIndex);

		m_bWork = FALSE;
	}
}

//	----------------------------------------------------------------
//	获取设备运行状态
//	----------------------------------------------------------------
BOOL CCANCtrl::GetStatus(void)
{
	return m_bWork;
}

//	----------------------------------------------------------------
//	获取硬件信息
//	----------------------------------------------------------------
BOOL CCANCtrl::GetDeviceInfo(PVCI_BOARD_INFO pInfo)
{
	if (m_bWork)
	{
		DWORD dwRet = VCI_ReadBoardInfo(m_dwDeviceType, m_dwDeviceIndex, pInfo);
		if (dwRet == STATUS_OK)
		{
			return TRUE;
		}
	}

	return FALSE;
}


UINT CCANCtrl::GetLastError(DWORD dwCANIndex)
{
	if (m_bWork)
	{
		VCI_ERR_INFO vei = {0};

		DWORD dwRet = VCI_ReadErrInfo(m_dwDeviceType, m_dwDeviceIndex, dwCANIndex, &vei);
		if (dwRet == STATUS_OK)
		{
			return vei.ErrCode;
		}
	}
	
	return 0;
}


ULONG CCANCtrl::Send(DWORD dwCANIndex, PVCI_CAN_OBJ pSend, ULONG ulLen)
{
	return VCI_Transmit(m_dwDeviceType, m_dwDeviceIndex, dwCANIndex, pSend, ulLen);
}


ULONG CCANCtrl::Receive(DWORD dwCANIndex, PVCI_CAN_OBJ pReceive, ULONG ulLen, int nWaitTime)
{
	return VCI_Receive(m_dwDeviceType, m_dwDeviceIndex, dwCANIndex, pReceive, ulLen, nWaitTime);
}


void CCANCtrl::GetTimerValue(__in UINT nBaudrate, __out UCHAR * pTimer0, __out UCHAR * pTimer1)
{
	switch (nBaudrate)
	{
	case BR_5:
		{
			*pTimer0 = 0xbf;
			*pTimer1 = 0xff;
		}
		break;
	case BR_10:
		{
			*pTimer0 = 0x31;
			*pTimer1 = 0x1c;
		}
		break;
	case BR_20:
		{
			*pTimer0 = 0x18;
			*pTimer1 = 0x1c;
		}
		break;
	case BR_40:
		{
			*pTimer0 = 0x87;
			*pTimer1 = 0xff;
		}
		break;
	case BR_50:
		{
			*pTimer0 = 0x09;
			*pTimer1 = 0x1c;
		}
		break;
	case BR_80:
		{
			*pTimer0 = 0x83;
			*pTimer1 = 0xff;
		}
		break;
	case BR_100:
		{
			*pTimer0 = 0x04;
			*pTimer1 = 0x1c;
		}
		break;
	case BR_125:
		{
			*pTimer0 = 0x03;
			*pTimer1 = 0x1c;
		}
		break;
	case BR_200:
		{
			*pTimer0 = 0x81;
			*pTimer1 = 0xfa;
		}
		break;
	case BR_250:
		{
			*pTimer0 = 0x01;
			*pTimer1 = 0x1c;
		}
		break;
	case BR_400:
		{
			*pTimer0 = 0x80;
			*pTimer1 = 0xfa;
		}
		break;
	case BR_500:
		{
			*pTimer0 = 0x00;
			*pTimer1 = 0x1c;
		}
		break;
	case BR_666:
		{
			*pTimer0 = 0x80;
			*pTimer1 = 0xb6;
		}
		break;
	case BR_800:
		{
			*pTimer0 = 0x00;
			*pTimer1 = 0x16;
		}
		break;
	case BR_1000:
		{
			*pTimer0 = 0x00;
			*pTimer1 = 0x14;
		}
		break;
	}
}

