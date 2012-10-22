#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>
#include <cstdarg>

struct Writer
{
    virtual void Write(const std::string& val) = 0;
};

struct ConsoleWritePolicy : public Writer
{
    void Write(const std::string& val) { std::cout << val << std::endl; }
};

struct FileWritePolicy : public Writer
{
    FileWritePolicy() { _file.open("log.txt"); }

    ~FileWritePolicy() { _file.close(); }

    void Write(const std::string& val) { _file << val << std::endl; }

    std::ofstream _file;
};

template <class WritePolicy>
class Logger
{
public:
    static Logger* Instance()
    {
        if (!_instance)
            _instance = new Logger<WritePolicy>;
        return _instance;
    }

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
    static Logger* _instance;
    std::mutex _mutex;
    WritePolicy _writer;
};

template <class WritePolicy>
Logger<WritePolicy>* Logger<WritePolicy>::_instance = NULL;

#define Console ConsoleWritePolicy
#define FileWP FileWritePolicy
#define sLog(policy) Logger<policy>::Instance()

#endif // LOG_H_
