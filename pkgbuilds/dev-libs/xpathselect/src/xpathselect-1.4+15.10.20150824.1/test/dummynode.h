/*
* Copyright (C) 2012 Canonical Ltd
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 3 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef _DUMMYNODE_H
#define _DUMMYNODE_H

#include "node.h"

#include <map>

// simple implementation of the node interface for testing purposes.
class DummyNode: public xpathselect::Node, public std::enable_shared_from_this<DummyNode>
{
public:
    typedef std::shared_ptr<DummyNode> Ptr;

    DummyNode(std::string name="DummyNode")
    : name_(name)
    {
        static int32_t id = 1;
        id_ = id++;
    }

    std::string GetName() const override
    {
        return name_;
    }

    std::string GetPath() const override
    {
        return std::string();
    }

    int32_t GetId() const override
    {
        return id_;
    }

    void SetName(std::string const& name)
    {
        name_ = name;
    }

    bool MatchStringProperty(const std::string& name, const std::string& value) const override
    {
        auto it = string_properties_.find(name);
        if (it == string_properties_.end() || it->second != value)
            return false;
        return true;
    }

    bool MatchBooleanProperty(const std::string& name, bool value) const override
    {
        auto it = bool_properties_.find(name);
        if (it == bool_properties_.end() || it->second != value)
            return false;
        return true;
    }

    bool MatchIntegerProperty(const std::string& name, int value) const override
    {
        auto it = int_properties_.find(name);
        if (it == int_properties_.end() || it->second != value)
            return false;
        return true;
    }

    xpathselect::NodeVector Children() const override
    {
        return children_;
    }

    virtual Node::Ptr GetParent() const override
    {
        return parent_;
    }

    void AddChild(Ptr const& child)
    {
        child->parent_ = shared_from_this();
        children_.push_back(child);
    }

    void AddProperty(std::string const& name, std::string const& value)
    {
        string_properties_[name] = value;
    }

    void AddProperty(std::string const& name, bool value)
    {
        bool_properties_[name] = value;
    }

    void AddProperty(std::string const& name, int value)
    {
        int_properties_[name] = value;
    }

private:
    int32_t id_;
    std::string name_;
    xpathselect::Node::Ptr parent_;
    xpathselect::NodeVector children_;
    std::map<std::string, std::string> string_properties_;
    std::map<std::string, int> int_properties_;
    std::map<std::string, bool> bool_properties_;
};


#endif
