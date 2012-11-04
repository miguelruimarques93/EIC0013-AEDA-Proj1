#ifndef RUNNABLE_H_
#define RUNNABLE_H_

#include <atomic>
#include <thread>

//! Runnable abstract Class
/*!
    Represents a class that can be used in a multithreaded way
*/
class Runnable
{
public:
    Runnable() : _stop(), _thread() { } ///< Constructor.
    virtual ~Runnable() { Stop(); } ///< Destructor. Stops thread.

    void Stop() { _stop = true; _thread.join(); } ///< Stops thread.
    void Start() { _thread = std::thread(&Runnable::Run, this); } ///< Starts thread.

protected:
    virtual void Run() = 0; ///< Method to execute when thread is started.
    std::atomic<bool> _stop; ///< True if the thread should be stopped.

private:
    std::thread _thread; ///< The thread.

private: // no copying
    Runnable(Runnable const&);
    Runnable& operator =(Runnable const&);
};

#endif // RUNNABLE_H_
