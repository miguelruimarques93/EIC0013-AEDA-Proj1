#include "grid.h"
#include "machine.h"
#include "gridmanager.h"
#include <algorithm>
#include <numeric>

uint Grid::_maxNameLength = 4; // std::string("Name").size();
uint Grid::_maxTopicLength = 5; // std::string("Topic").size();

Grid::Grid(const std::string& name, const std::string& topic) : _name(name), _topic(topic)
{
    _gm = new GridManager();

    if (_name.length() > _maxNameLength)
        _maxNameLength = _name.length();

    if (_topic.length() > _maxTopicLength)
        _maxTopicLength = _topic.length();
}

Grid::Grid(const std::string& name, const std::string& topic, GridManager* gm) : _name(name), _topic(topic), _gm(gm)
{
    if (_name.length() > _maxNameLength)
        _maxNameLength = _name.length();

    if (_topic.length() > _maxTopicLength)
        _maxTopicLength = _topic.length();
}

Grid::~Grid()
{
    delete _gm;
}

Grid* Grid::Load(ByteBuffer& bb)
{
    std::string name = bb.ReadString();
    std::string topic = bb.ReadString();

    GridManager* gm = GridManager::Load(bb);

    Grid* g = new Grid(name, topic, gm);

    return g;
}

bool Grid::Save(ByteBuffer& bb) const
{
    bb.WriteString(GetName());
    bb.WriteString(GetTopic());

    _gm->Save(bb);

    return true;
}

double Grid::HighestMachineRAM() const
{
    std::vector<double> rams = _gm->ApplySelector<Machine, double>([](const Machine* m) { return m->GetTotalRAM(); });

    if (rams.empty())
        return 0.0;

    std::sort(rams.begin(), rams.end());

    return rams.back() / 1024.0; // in GB
}

double Grid::TotalDiskSpace() const
{
    std::vector<double> diskSpaces = _gm->ApplySelector<Machine, double>([](const Machine* m) { return m->GetTotalDiskSpace(); });

    return std::accumulate(diskSpaces.begin(), diskSpaces.end(), 0.0) / 1024.0; // in GB
}

uint Grid::GetAvailableMachineCount() const
{
    return _gm->GetNumberOfMachines();
}

void Grid::Update(uint32 diff)
{
    _gm->Update(diff);
}

void Grid::Print(std::ostream& os /*= std::cout*/) const
{
    os << "| " << std::setw(_maxNameLength) << _name << " | " << std::setw(_maxTopicLength) << _topic << " | " << std::setw(16) << HighestMachineRAM() << " | " << std::setw(15) << TotalDiskSpace() << " | " << std::setw(10) << GetAvailableMachineCount() << " |\n"
       << "--" << std::string(_maxNameLength, '-')   << "---" << std::string(_maxTopicLength, '-')    << "----------------------------------------------------\n";
}

void Grid::PrintHeader(std::ostream& os /*= std::cout*/)
{
    os << "--" << std::string(_maxNameLength, '-')    << "---" << std::string(_maxTopicLength, '-')     << "----------------------------------------------------\n"
       << "| " << std::setw(_maxNameLength) << "Name" << " | " << std::setw(_maxTopicLength) << "Topic" << " | Highest RAM (GB) | Disk space (GB) | # Machines |\n"
       << "--" << std::string(_maxNameLength, '-')    << "---" << std::string(_maxTopicLength, '-')     << "----------------------------------------------------\n";
}

void Grid::ChangeTopic(const std::string& topic)
{
    _topic = topic;

    if (_topic.length() > _maxTopicLength)
        _maxTopicLength = _topic.length();
}

void Grid::ChangeName(const std::string& name)
{
    _name = name;

    if (_name.length() > _maxNameLength)
        _maxNameLength = _name.length();
}
