//////////////////////////////////////////////////////////////////////////
// 获取IP地址或数据通道编码

#pragma once

class CGetFieldsCode
{
public:
	CGetFieldsCode(void): m_byField0(0), m_byField1(0), m_byField2(0), m_byField3(0) {};
	virtual ~CGetFieldsCode(void) {};

protected:
	BYTE	m_byField0;
	BYTE	m_byField1;
	BYTE	m_byField2;
	BYTE	m_byField3;

public:
	void Set1stField(BYTE byField) {m_byField0 = byField;}
	void Set2ndField(BYTE byUnitType, BYTE byUnitNum) { m_byField1 = byUnitType << 4 | byUnitNum;}
	void Set3rdField(BYTE bySlot) { m_byField2 = bySlot; }
	void Set4thField(BYTE byChannelType, BYTE byChannelNum) { m_byField3 = byChannelType << 4 | byChannelNum; }

	BYTE Get1stField(void) { return m_byField0; }
	BYTE Get2ndField(void) { return m_byField1; }
	BYTE Get3rdField(void) { return m_byField2; }
	BYTE Get4thField(void) { return m_byField3; }

	// 返回值由大端法进行处理
	DWORD GetFields(BYTE byField0, BYTE byUnitType, BYTE byUnitNum, BYTE bySlot, BYTE byChannelType, BYTE byChannelNum)
	{
		DWORD nFields(0);
		BYTE *pbyField = (BYTE *)&nFields;

		*pbyField = byField0;
		*(pbyField+1) = byUnitType << 4 | byUnitNum;
		*(pbyField+2) = bySlot;
		*(pbyField+3) = byChannelType << 4 | byChannelNum;

		m_byField0 = *pbyField;
		m_byField1 = *(pbyField+1);
		m_byField2 = *(pbyField+2);
		m_byField3 = *(pbyField+3);

		return nFields;
	}

	CString GetIPString(BYTE byField0, BYTE byUnitType, BYTE byUnitNum, BYTE bySlot, BYTE byChannelType, BYTE byChannelNum)
	{
		CString strFields(_T(""));
		strFields.Format(_T("%d.%d.%d.%d"), byField0, byUnitType << 4 | byUnitNum, bySlot, byChannelType << 4 | byChannelNum);
		return strFields;
	}
};