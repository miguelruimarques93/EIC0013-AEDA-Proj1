#ifndef RUNNABLE_H_
#define RUNNABLE_H_

#include <atomic>
#include <thread>

class Runnable
{
public:
    Runnable() : _stop(), _thread() { }
    virtual ~Runnable() { Stop(); }

    void Stop() { _stop = true; _thread.join(); }
    void Start() { _thread = std::thread(&Runnable::Run, this); }

protected:
    virtual void Run() = 0;
    std::atomic<bool> _stop;

private:
    std::thread _thread;

private: // no copying
    Runnable(Runnable const&);
    Runnable& operator =(Runnable const&);
};

#endif // RUNNABLE_H_
