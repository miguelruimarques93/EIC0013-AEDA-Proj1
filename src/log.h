#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>
#include <cstdarg>
#include <sstream>

/// Base class for Writers
struct Writer
{
    virtual void Write(const std::string& val) = 0; ///> Writes a message to the logger
};

/// Writer for console
struct ConsoleWritePolicy : public Writer
{
    void Write(const std::string& val) { std::cout << '\r' << val << std::endl; }
};

/// Writer for file (log.txt)
struct FileWritePolicy : public Writer
{
    FileWritePolicy() { _file.open("log.txt"); }

    ~FileWritePolicy() { _file.close(); }

    void Write(const std::string& val) { _file << val << std::endl; }

    std::ofstream _file;
};

//! Logger singleton Class
/*!
    A class that enables to log messages to files, console and others
*/
template <class WritePolicy>
class Logger
{
public:
    /// Returns the singleton's instance of this class
    static Logger* Instance()
    {
        if (!_instance)
            _instance = new Logger<WritePolicy>;
        return _instance;
    }

    /// Writes a message (printf style). All messages will be prefixed with current time.
    void Log(const char* format, ...)
    {
        char buffer[512];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        std::ostringstream out;

        _mutex.lock();

        time_t now = time(0);
        tm* current = localtime(&now);

        out << '[' << current->tm_hour << ':' << current->tm_min << ':' << current->tm_sec << ']' << ": " << buffer;
        _writer.Write(out.str());

        _mutex.unlock();
    }

private:
    static Logger* _instance; ///> There's only one instance of this class
    std::mutex _mutex; ///> Mutex to make the class safe to multiple threads
    WritePolicy _writer; ///> A writer (console, file, etc.)
};

template <class WritePolicy>
Logger<WritePolicy>* Logger<WritePolicy>::_instance = NULL;

/// Alias for ConsoleWritePolicy
#define Console ConsoleWritePolicy

/// Alias for FileWritePolicy
#define FileWP FileWritePolicy

/// Short way to access the log. Example: sLog(Console)->Log("Message here.");
#define sLog(policy) Logger<policy>::Instance()

#endif // LOG_H_
