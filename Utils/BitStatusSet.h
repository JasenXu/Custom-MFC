#pragma once
#include <bitset>


typedef UINT	BitType;

// 定义比特位数
#define BYTE_BITS	8
#define BIT_SIZE	(BYTE_BITS*sizeof(BitType))


class CBitStatusSet
{
public:
	CBitStatusSet(void);
	virtual ~CBitStatusSet(void);

protected:
	std::bitset<BIT_SIZE>	m_bitset;

public:
	BOOL SetBitStatus(int nIndex, BOOL bStatus);
	BOOL GetBitStatus(int nIndex, BOOL &bStatus);

	void SetStatusByte(BitType status);
	BitType GetStatusByte(void);

	BOOL SetByteStatus(int nIndex, int nCount, BYTE byStatus);
};

