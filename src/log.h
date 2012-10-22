#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdarg>

class Logger
{
public:
	virtual ~Logger() { }

	virtual void Log(const char * format, ...) = 0;
};

struct Writer
{
	void Write(const std::string& val)
	{
		std::cout << val << std::endl;
	}
};

struct Formatter 
{
	std::string Format(const char* format, va_list args)
	{
		char res[1000];
		vsprintf(res, format, args);
		std::ostringstream out;
		out << time(NULL) << ": " << res;
		return out.str();
	}
};

template <class FormatPolicy, class WritePolicy>
class LoggerImpl : public Logger, public WritePolicy
{
public:
	virtual void Log(const char * format, ...) 
	{ 
		va_list args;
		va_start(args, format);
		WritePolicy::Write(_formatter.Format(format, args)); 
		va_end(args);
	}


	FormatPolicy _formatter;
};

#endif // LOG_H_
