#include "grid.h"
#include "machine.h"
#include <algorithm>
#include <numeric>

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

    return std::accumulate(diskSpaces.begin(), diskSpaces.end(), 0.0) / 1024.0;
}
