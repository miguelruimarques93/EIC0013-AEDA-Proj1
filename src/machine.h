#ifndef MACHINE_H_
#define MACHINE_H_

#include <vector>
#include <string>

#include "job.h"

class Machine
{
public:
	
protected:

private:
	float _availableRam,
		  _availableDiskSpace;
    std::vector<Software> _availableSoftware;

    int _maxJobs;
    std::vector<Job> _currentJobs;

};

#endif // MACHINE_H_
