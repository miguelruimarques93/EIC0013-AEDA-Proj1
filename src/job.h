#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <vector>

struct Software;

class Job
{
public:

protected:

private:
	std::string _name;
	int _priority;
	float _requiredRam;
	float _requiredDiskSpace;
	std::vector<Software> _requiredSoftware;
};

#endif // JOB_H_
