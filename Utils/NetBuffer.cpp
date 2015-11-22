// NetBuffer.cpp : 实现文件
//

#include "stdafx.h"
#include "NetBuffer.h"


// CNetBuffer

CNetBuffer::CNetBuffer()
	: m_nCount(0)
	, m_nBufSize(0)
	, m_nWriteIndex(-1)
	, m_nReadIndex(-1)
{
	
}

CNetBuffer::~CNetBuffer()
{
	while (m_arBuf.GetCount() > 0)
	{
		PUCHAR ptr = m_arBuf.GetAt(0);
		delete[] ptr;
		m_arBuf.RemoveAt(0);
	}
}

// CNetBuffer 消息处理程序




void CNetBuffer::Init(UINT nBufSize, UINT nBufCount)
{
	m_arBuf.SetSize(nBufCount);
	m_nBufSize = nBufSize;
	m_nCount = nBufCount;

	for (UINT i(0); i < m_nCount; ++i)
	{
		PUCHAR ptr = new UCHAR[m_nBufSize];
		SecureZeroMemory(ptr, m_nBufSize);
		m_arBuf.SetAtGrow(i, ptr);
	}
}


int CNetBuffer::AddData(PUCHAR pBuf, UINT nLen)
{
	if (nLen > m_nBufSize-5)
	{
		return -1;
	}

	for (UINT i(0); i < m_nCount; ++i)
	{
		m_nWriteIndex = ++m_nWriteIndex % m_nCount;
		PUCHAR ptr = m_arBuf.GetAt(m_nWriteIndex);
		if (*ptr == 0)
		{
			memcpy_s(ptr+1, 4, &nLen, sizeof(UINT));
			memcpy_s(ptr+5, m_nBufSize-5, pBuf, nLen);
			*ptr = 1;
			return i;
		}	
	}

	return -2;
}


UINT CNetBuffer::GetCount(void) const
{
	return m_nCount;
}


void CNetBuffer::GetData(PUCHAR& pBuf, UINT& nLen)
{
	for (UINT i(0); i < m_nCount; ++i)
	{
		m_nReadIndex = ++m_nReadIndex % m_nCount;
		PUCHAR ptr = m_arBuf.GetAt(m_nReadIndex);
		if (*ptr == 1)
		{
			memcpy_s(&nLen, sizeof(UINT), ptr+1, 4);
			pBuf = ptr+5;
//			*ptr = 0;
			return;
		}
	}
	
	pBuf = nullptr;
	nLen = 0;
}


int CNetBuffer::AddDataAt(PUCHAR pBuf, UINT nLen, int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)m_nCount)
	{
		PUCHAR ptr = m_arBuf.GetAt(nIndex);

		UINT nBufLen(0);
		memcpy_s(&nBufLen, sizeof(UINT), ptr+1, 4);
			
		if (nBufLen+nLen > m_nBufSize)
		{
			return -1;
		}

		memcpy_s(ptr+nBufLen, m_nBufSize-nBufLen, pBuf, nLen);
		return nIndex;	
	}
	return -2;
}


void CNetBuffer::DeleteData(void)
{
	PUCHAR ptr = m_arBuf.GetAt(m_nReadIndex);
	*ptr = 0;
}
