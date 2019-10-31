#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

template<class T>
class Thread
{
	typedef DWORD(T::* Method)(LPVOID mParams);

protected:
	HANDLE  hThread;
private:
	DWORD   threadID;
	T* object;
	Method  method;
	HANDLE  hInterrupt;
	HANDLE  hSingleStart;
	LPVOID  mParams;

private:
	static DWORD run(LPVOID thread_obj)
	{
		Thread<T>* thread = (Thread<T>*)thread_obj;
		return (thread->object->*thread->method) (thread->mParams);
	}
	Thread(const Thread<T>& other) = delete;
	Thread<T>& operator =(const Thread<T>& other) = delete;

public:
	explicit Thread(T* object, DWORD(T::* method)(LPVOID mParams), LPVOID mParams = NULL)
	{
		this->hThread = NULL;
		this->object = object;
		this->method = method;
		this->threadID = 0;
		this->hInterrupt = CreateSemaphore(NULL, 1, 1, NULL);
		this->hSingleStart = CreateMutex(NULL, FALSE, NULL);
		this->mParams = mParams;
	}
	~Thread(void)
	{
		if (hInterrupt)
			CloseHandle(hInterrupt);
		if (hThread)
			CloseHandle(hThread);
	}
	bool start()
	{
		__try {
			if (WaitForSingleObject(hSingleStart, 0) != WAIT_OBJECT_0)
				return false;
			if (hThread)
			{
				if (WaitForSingleObject(hThread, 0) == WAIT_TIMEOUT)
				{
					return false;
				}
				CloseHandle(hThread);
			}

			WaitForSingleObject(hInterrupt, 0);

			hThread = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)Thread<T>::run,
				this,
				0,
				&this->threadID
			);
			if (hThread)
				return true;
			return false;
		}
		__finally
		{
			ReleaseMutex(hSingleStart);
		}
	}
	
	inline void join()
	{
		WaitForSingleObject(hThread, INFINITE);
	}

	inline bool interrupt()
	{
		if (hInterrupt)
		{
			return ((ReleaseSemaphore(hInterrupt, 1, NULL) == FALSE) ?
				false : true);
		}
		return false;
	}

	inline bool isInterrupted()
	{
		return this->isInterrupted(0);
	}

	inline bool isInterrupted(DWORD millisec)
	{
		if (WaitForSingleObject(hInterrupt, millisec) == WAIT_TIMEOUT)
		{
			return false;
		}
		ReleaseSemaphore(hInterrupt, 1, NULL);
		return true;
	}

	inline bool isRunning()
	{
		DWORD exitCode = 0;
		if (hThread)
			GetExitCodeThread(hThread, &exitCode);
		if (exitCode == STILL_ACTIVE)
			return true;
		return false;
	}

	__declspec(property(get = getThreadHandle)) HANDLE ThreadHandle;
	inline HANDLE getThreadHandle()
	{
		return hThread;
	}

	__declspec(property(get = getThreadID)) DWORD ThreadID;
	inline DWORD getThreadID()
	{
		return threadID;
	}

};