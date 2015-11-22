#pragma once
#include "afxmt.h"

// 打开写入测试语句
//#define WRITE_TEST		1


//////////////////////////////////////////////////////////////////////////
//自动锁
class CAutoLock
{
public:
	CAutoLock(CCriticalSection& lock)
		: m_cs(lock)
	{
		m_cs.Lock();
	}

	virtual ~CAutoLock()
	{
		m_cs.Unlock();
	}

private:
	CCriticalSection&	m_cs;
};


//////////////////////////////////////////////////////////////////////////
// 环形缓冲类
class CCircleBuffer
{
public:
	CCircleBuffer(int nBufferSize);
	virtual ~CCircleBuffer(void);

private:
	CCriticalSection	m_cs;			// 互斥锁
	int					m_nBufferSize;	// 缓冲区长度
	int					m_nReadPos;		// 读位置
	int					m_nWritePos;	// 写位置
	BYTE				*m_pBuffer;		// 缓冲区首地址指针

public:
	int Write(LPVOID lpData, int nLen);
	int Read(LPVOID lpData, int nLen);

	BOOL IsFull(void);
	BOOL IsEmpty(void);

private:
	int GetDataLength(void) const;
	int GetFreeLength(void) const;

#ifdef WRITE_TEST
	CFile				m_file;			// 写入测试时，用于保存写入的数据
#endif
};

