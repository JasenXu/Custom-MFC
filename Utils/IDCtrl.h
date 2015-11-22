#pragma once
#include "BitStatusSet.h"

class CIDCtrl
{
public:
	CIDCtrl(void);
	virtual ~CIDCtrl(void);

protected:
	CBitStatusSet	m_bitset;

public:
	BOOL SetUnitID(BYTE byID);
	BOOL SetSlotID(BYTE byID);
	BOOL SetChannelID(BYTE byID);
	BOOL SetFunctionCode(BYTE byCode);
	BOOL SetFrameNum(BYTE byNum);
	BOOL SetSerialNum(BYTE byNum);
	UINT GetIDValue(void);
	BOOL SetDeviceNodeID(BYTE byUnitID, BYTE bySlotID, BYTE byChannelID);
	void SetDeviceNodeID(UINT nNodeID);
	UINT GetDeviceNodeID(void);
};

