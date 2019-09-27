#ifndef _THREADMODEL_H_
#define _THREADMODEL_H_

#include "windows.h"
#include "NonCopyable.h"

template<class Host>
class MemFunThread : NonCopyable
{
    typedef void (Host::*mfun)();
private:
    HANDLE _hThread;
    Host* _host;     // object
    mfun _tfun;      // thread function

    static DWORD WINAPI ThreadProc(LPVOID param)
    {
        MemFunThread* thread = (MemFunThread*)param;    
        ((thread->_host)->*(thread->_tfun))(); // (object->*member_function)();
        return 0;
    }

public:
    MemFunThread(Host* host, mfun thrFun):_hThread(NULL),_host(host),_tfun(thrFun){ }

    ~MemFunThread()
    {
        ::WaitForSingleObject(_hThread, INFINITE);
        ::CloseHandle(_hThread);
    }

    void Execute()
    {
        if(!_hThread) _hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
    }
};

template<class Host>
MemFunThread<Host>* new_MemFunThread(Host* host, void(Host::*mfun)())
{
    return new MemFunThread<Host>(host, mfun);
}

class ObjectLevelLocker
{
    CRITICAL_SECTION _cs;
public:
    ObjectLevelLocker()
    { ::InitializeCriticalSection(&_cs); }

    ~ObjectLevelLocker()
    { ::DeleteCriticalSection(&_cs); }

    void doLock()
    { ::EnterCriticalSection(&_cs); }

    void unLock()
    { ::LeaveCriticalSection(&_cs); }

#if (_WIN32_WINNT >= 0x0400)
    bool tryLock()
    { return ::TryEnterCriticalSection(&_cs) != FALSE; }
#endif 

    class Lock : NonCopyable
    {
        ObjectLevelLocker& _host;
    public:
        Lock(ObjectLevelLocker* host):_host(*host)
        { _host.doLock(); }

        ~Lock()
        { _host.unLock(); }
    };

#if (_WIN32_WINNT >= 0x0400)
    class TryLock : NonCopyable
    {
    public:
        TryLock(ObjectLevelLocker* host):_host(*host)
        { _bLock = _host.tryLock(); }
        
        ~TryLock()
        { if(_bLock) _host.unLock(); }

        bool IsLock()
        { return _bLock; }

    private:
        ObjectLevelLocker& _host;
        bool _bLock;
    };
#endif

    template<class T> class AutoLock
    {
    public:
        AutoLock(T* host):_host(host)
        { _host->doLock(); }

        ~AutoLock()
        { _host->unLock(); }

        T* operator->()
        { return _host; }

    private:
        AutoLock& operator=(const AutoLock&);
        T* _host;
    };
};

#endif // _THREADMODEL_H_
