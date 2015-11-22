#include "StdAfx.h"
#include "IDCtrl.h"


CIDCtrl::CIDCtrl()
{
}


CIDCtrl::~CIDCtrl(void)
{
}


BOOL CIDCtrl::SetUnitID(BYTE byID)
{
	return m_bitset.SetByteStatus(25, 4, byID);
}


BOOL CIDCtrl::SetSlotID(BYTE byID)
{
	return m_bitset.SetByteStatus(21, 4, byID);
}


BOOL CIDCtrl::SetChannelID(BYTE byID)
{
	return m_bitset.SetByteStatus(18, 3, byID);
}


BOOL CIDCtrl::SetFunctionCode(BYTE byCode)
{
	return m_bitset.SetByteStatus(13, 5, byCode);
}


BOOL CIDCtrl::SetFrameNum(BYTE byNum)
{
	return m_bitset.SetByteStatus(5, 8, byNum);
}


BOOL CIDCtrl::SetSerialNum(BYTE byNum)
{
	return m_bitset.SetByteStatus(0, 5, byNum);
}


UINT CIDCtrl::GetIDValue(void)
{
	return m_bitset.GetStatusByte();
}


BOOL CIDCtrl::SetDeviceNodeID(BYTE byUnitID, BYTE bySlotID, BYTE byChannelID)
{
	if (SetUnitID(byUnitID))
	{
		if (SetSlotID(bySlotID))
		{
			return SetChannelID(byChannelID);
		}
	}
	return FALSE;
}


void CIDCtrl::SetDeviceNodeID(UINT nNodeID)
{
	m_bitset.SetStatusByte(nNodeID<<18);
}


UINT CIDCtrl::GetDeviceNodeID(void)
{
	return (GetIDValue() >> 18);
}
