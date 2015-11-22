#include "StdAfx.h"
#include "CircleBuffer.h"


CCircleBuffer::CCircleBuffer(int nBufferSize)
	: m_nBufferSize(nBufferSize)
	, m_nReadPos(0)
	, m_nWritePos(m_nReadPos)
	, m_pBuffer(NULL)
{
	m_pBuffer = new BYTE[m_nBufferSize+1]();
#ifdef WRITE_TEST
	m_file.Open(_T("~circle_buffer_write_test.dat"), CFile::modeCreate | CFile::modeWrite);
#endif
}


CCircleBuffer::~CCircleBuffer(void)
{
	if (m_pBuffer != NULL)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
#ifdef WRITE_TEST
	m_file.Close();
#endif
}

//////////////////////////////////////////////////////////////////////////
// 写入数据
int CCircleBuffer::Write(LPVOID lpData, int nLen)
{
	CAutoLock lock(m_cs);

	// 输入的数据指针不正确
	if (lpData == NULL)
	{
		return 0;
	}

	// 输入的数据长度不正确，返回0
	if (nLen <= 0)
	{
		return 0;
	}

	// 没有空闲空间时，返回0
	int nFreeLen = GetFreeLength() - 1;
	if (nFreeLen <= 0)
	{
		return 0;
	}

	// 确定实际可写入的数据长度
	int nValidDataLen = min(nFreeLen, nLen);

	if (m_nReadPos > m_nWritePos)
	{
		// 当写位置在读位置后面时，不用回环
		memcpy_s(m_pBuffer+m_nWritePos, nFreeLen, (BYTE *)lpData, nValidDataLen);
#ifdef WRITE_TEST
		m_file.Write(m_pBuffer+m_nWritePos, nValidDataLen);
#endif
		// 写位置向尾部移动
		m_nWritePos += nValidDataLen;	
	}
	else
	{
		// 当写位置在读位置前面时，可能需要回环
		// 确定缓冲尾部实际可写入的数据长度
		int nTailLen = m_nBufferSize - m_nWritePos;
		int nTailWriteLen = min(nValidDataLen, nTailLen);

		memcpy_s(m_pBuffer+m_nWritePos, nTailLen, (BYTE *)lpData, nTailWriteLen);
#ifdef WRITE_TEST
		m_file.Write(m_pBuffer+m_nWritePos, nTailWriteLen);
#endif
		m_nWritePos = (m_nWritePos+nValidDataLen) % m_nBufferSize;
		memcpy_s(m_pBuffer, nFreeLen-nTailWriteLen, (BYTE *)lpData+nTailWriteLen, nValidDataLen-nTailWriteLen);
#ifdef WRITE_TEST
		m_file.Write(m_pBuffer, nValidDataLen-nTailWriteLen);
#endif
	}

	return nValidDataLen;
}

//////////////////////////////////////////////////////////////////////////
// 读取数据
int CCircleBuffer::Read(LPVOID lpData, int nLen)
{
	CAutoLock lock(m_cs);

	// 输入的数据指针不正确
	if (lpData == NULL)
	{
		return 0;
	}

	// 输入的数据长度不正确，返回0
	if (nLen <= 0)
	{
		return 0;
	}

	// 缓冲已空时，返回0
	int nDataLen = GetDataLength();
	if (nDataLen <= 0)
	{
		return 0;
	}

	// 确定实际可读取的数据长度
	int nValidDataLen = min(nDataLen, nLen);

	if (m_nWritePos > m_nReadPos)
	{
		// 当写位置在读位置前面时，不用回环
		memcpy_s((BYTE *)lpData, nLen, m_pBuffer+m_nReadPos, nValidDataLen);

		// 读位置向尾部移动
		m_nReadPos += nValidDataLen;
	} 
	else
	{
		// 当写位置在读位置后面时，需要回环
		// 确定缓冲区尾部实际可读取的数据长度
		int nTailLen = m_nBufferSize - m_nReadPos;
		int nTailReadLen = min(nTailLen, nValidDataLen);

		memcpy_s((BYTE *)lpData, nLen, m_pBuffer+m_nReadPos, nTailReadLen);
		m_nReadPos = (m_nReadPos+nValidDataLen) % m_nBufferSize;
		memcpy_s((BYTE *)lpData+nTailLen, nLen-nTailReadLen, m_pBuffer, nValidDataLen-nTailReadLen);
	}

	return nValidDataLen;
}

//////////////////////////////////////////////////////////////////////////
// 是否缓冲区已满
BOOL CCircleBuffer::IsFull(void)
{
	CAutoLock lock(m_cs);

	return BOOL(GetFreeLength() == 1);
}

//////////////////////////////////////////////////////////////////////////
// 是否缓冲区为空
BOOL CCircleBuffer::IsEmpty(void)
{
	CAutoLock lock(m_cs);

	return BOOL(m_nWritePos == m_nReadPos);
}

//////////////////////////////////////////////////////////////////////////
// 获取数据长度
int CCircleBuffer::GetDataLength(void) const
{
	int nLen = m_nWritePos - m_nReadPos;
	if (nLen < 0)
	{
		// 如果写任务已经回环，而读任务未回环
		nLen = m_nBufferSize + nLen;
	}

	return nLen;
}

//////////////////////////////////////////////////////////////////////////
// 获取空闲空间长度
int CCircleBuffer::GetFreeLength(void) const
{
	int nLen  = m_nReadPos - m_nWritePos;
	if (nLen <= 0)
	{
		// 如果读写任务都没有回环
		nLen = m_nBufferSize + nLen;
	}

	return nLen;
}
