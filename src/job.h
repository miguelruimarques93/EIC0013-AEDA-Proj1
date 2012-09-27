#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <vector>

#include "machine.h"

struct Software
{
	std::string Name;
	std::string Version;
};

class Job
{
public:

protected:

private:
	std::string _name;
	int _priority;
	float _requiredRam,
          _requiredDiskSpace;
	std::vector<Software> _requiredSoftware;

    Machine* _machine;
};

#endif // JOB_H_
