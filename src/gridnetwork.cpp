#include "gridnetwork.h"
#include "grid.h"
#include "bytebuffer.h"
#include "loader.h"
#include "log.h"
#include <memory>
#include <algorithm>

GridNetwork* GridNetwork::Load(ByteBuffer& bb)
{
    GridNetwork* gn = new GridNetwork();

    uint32 gridCount = bb.ReadUInt32();

    for (uint32 i = 0; i < gridCount; ++i)
    {
        std::string name = bb.ReadString();
        gn->AddGrid(name, Grid::Load(bb));
    }

    return gn;
}

bool GridNetwork::Save(ByteBuffer& bb) const
{
    bb.WriteUInt32(_bst.size());

    for (auto pair : _bst)
    {
        bb.WriteString(pair.first);
        if (!pair.second->Save(bb))
            return false;
    }

    return true;
}

void GridNetwork::NewGrid(const std::string& name, const std::string& subject)
{
    if (_bst.find(name) != _bst.end())
        throw ExistingGrid(name);

    _bst[name] = new Grid(name, subject);
}

void GridNetwork::ChangeGridName(const std::string& oldName, const std::string& newName)
{
    auto itr = _bst.find(oldName);
    if (itr == _bst.end())
        throw UnknownGrid(oldName);

    // save and remove old value
    Grid* temp = itr->second;
    _bst.erase(itr);

    // change name
    temp->ChangeName(newName);

    // add it back with new name (name is BST's key)
    _bst[newName] = temp;
}

void GridNetwork::ChangeGridTopic(const std::string& name, const std::string& newTopic)
{
    auto itr = _bst.find(name);
    if (itr == _bst.end())
        throw UnknownGrid(name);

    itr->second->ChangeTopic(newTopic);
}

void GridNetwork::RemoveGrid(const std::string& name)
{
    auto itr = _bst.find(name);
    if (itr == _bst.end())
        throw UnknownGrid(name);

    delete itr->second;
    _bst.erase(itr);
}

Grid* GridNetwork::GetGrid(const std::string& name)
{
    auto itr = _bst.find(name);
    if (itr == _bst.end())
        throw UnknownGrid(name);

    return itr->second;
}

GridNetwork::~GridNetwork()
{
    std::for_each(_bst.begin(), _bst.end(), [](std::pair<std::string, Grid*> pair) { delete pair.second; });
}

void GridNetwork::AddGrid(const std::string& name, Grid* grid)
{
    _bst[name] = grid;
}

void GridNetwork::Run()
{
    _realCurrTime = GetCurrentTime();

    uint saveDiff = 0;

    while (!_stop)
    {
        _realPrevTime = _realCurrTime;
        _realCurrTime = GetCurrentTime();

        uint32 diff = GetTimeDiff(_realCurrTime, _realPrevTime);

        /*sLog(Console)->Log("Diff: %u", diff);*/

        Update(diff);

        saveDiff += diff;
        if (saveDiff >= 5000) // save every 5 seconds
        {
            saveDiff = 0;

            if (!Saver<GridNetwork>(GRID_SAVE_FILE).Save(this))
                sLog(Console)->Log("GridNetwork save failed in Update()");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // sleep for half a second
    }
}

void GridNetwork::Update(uint32 diff)
{
    for (auto pair : _bst)
        pair.second->Update(diff);
}

std::vector<const Grid*> GridNetwork::ApplyPredicate(std::function<bool(const Grid*)> predicate) const
{
    std::vector<const Grid*> result;

    for (auto g : _bst)
        if (predicate(g.second))
            result.push_back(g.second);

    return result;
}
