#include "gridnetwork.h"
#include "grid.h"
#include "bytebuffer.h"
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
        pair.second->Save(bb);
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

    // add it back with new name
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
