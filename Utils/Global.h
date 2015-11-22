// 线程退出等待时间
#define WAIT_THREAD_EXIT	10000


// 安全删除数组
#define SAFE_DELETE_BUFFER(x) {if (x) { delete [] (x); (x)=NULL;}}
#define SAFE_DELETE_ARRAY(x)  { if(x) { delete [] (x); (x)=NULL;}}

//////////////////////////////////////////////////////////////////////////
// 安全删除对象
#define SAFE_DELETE_OBJECT(x) {if (x) {delete (x);(x)=NULL;}}
#define SAFE_DELETE(x) { if (x)	{delete (x); (x) = NULL;}}


//安全释放COM接口
#define SAFE_RELEASE(x) { if (x) {(x)->Release(); (x) = NULL; }}


//安全关闭对象
#define SAFE_CLOSE(x) { if (x) {x->Close(); delete x; x = NULL;}}


//安全退出线程
#define SAFE_EXIT_THREAD(x) { \
	if (x) \
	{ \
		PostThreadMessage(x->m_nThreadID, PWM_EXIT, 0, 0); \
		if (::WaitForSingleObject(x->m_hThread, WAIT_THREAD_EXIT) == WAIT_TIMEOUT) \
		{ \
			::TerminateThread(x->m_hThread, 1); \
		} \
	} \
	x = NULL; \
}


//安全退出线程
#define SAFE_QUIT_THREAD(x, time) { \
	if (x) \
	{ \
		if (::WaitForSingleObject(x->m_hThread, time) == WAIT_TIMEOUT) \
		{ \
			::TerminateThread(x->m_hThread, 1); \
		} \
	} \
	x = NULL; \
}