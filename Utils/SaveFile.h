#pragma once
#include "atlcomtime.h"


class CSaveFile
{
public:
	CSaveFile(void);
	virtual ~CSaveFile(void);

protected:
	CString			m_strAppDirectory;
	CFile			m_file;
	COleDateTime	m_olePrevTime;

	CString			m_str1stDirPath;
	CString			m_str2ndDirPath;
	CString			m_str3rdDirPath;
	CString			m_strExtend;

protected:
	BOOL OpenFile(LPCTSTR lpszFilePath);

public:
	void CreateFileDirectory(LPCTSTR lpPathName);
	void WriteFile(LPVOID lpBuf, UINT nCount);
	
	CString GetAppDirectory(void);
	void Create1stDirectory(LPCTSTR lpDirectoryPath);
	void Create2ndDirectory(LPCTSTR lpDirectoryName);
	void CreateLastDirectory(LPCTSTR lpDirectoryName);

	void SetExtend(LPCTSTR lpszExtend);

protected:
	BOOL IsHourChanged(void);
	BOOL IsMinuteChanged(void);
};

