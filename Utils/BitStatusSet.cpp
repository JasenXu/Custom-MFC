#include "StdAfx.h"
#include "BitStatusSet.h"


CBitStatusSet::CBitStatusSet(void)
{
}


CBitStatusSet::~CBitStatusSet(void)
{
}


BOOL CBitStatusSet::SetBitStatus(int nIndex, BOOL bStatus)
{
	if (nIndex >= 0 && nIndex <= BIT_SIZE-1)
	{
		if (bStatus)
		{
			m_bitset.reset(nIndex);
		} 
		else
		{
			m_bitset.set(nIndex);
		}

		return TRUE;
	}

	return FALSE;
}


BOOL CBitStatusSet::GetBitStatus(int nIndex, BOOL &bStatus)
{
	if (nIndex >= 0 && nIndex <= BIT_SIZE-1)
	{
		bStatus = !m_bitset[nIndex];
		return TRUE;
	}

	return FALSE;
}


void CBitStatusSet::SetStatusByte(BitType status)
{
	memcpy_s(&m_bitset, BIT_SIZE, &status, min(BIT_SIZE, sizeof(BitType)));
}


BitType CBitStatusSet::GetStatusByte(void)
{
	return (BitType)m_bitset.to_ullong();
}


BOOL CBitStatusSet::SetByteStatus(int nIndex, int nCount, BYTE byStatus)
{
	if (nIndex >= 0 && nIndex+nCount <= BIT_SIZE)
	{
		if (nCount <= BYTE_BITS)
		{
			std::bitset<BYTE_BITS> byteSet = (BYTE)byStatus;

			for (int i(0); i < nCount; ++i)
			{
				m_bitset[nIndex+i] = byteSet[i];
			}

			return TRUE;
		}
	}

	return FALSE;
}
