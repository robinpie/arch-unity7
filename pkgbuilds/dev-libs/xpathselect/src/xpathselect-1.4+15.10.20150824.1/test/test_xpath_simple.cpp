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

#include "gtest/gtest.h"

#include "node.h"
#include "xpathselect.h"

#include "dummynode.h"

// empty query must select tree root.
TEST(TestXPath, test_select_empty_tree)
{
    xpathselect::Node::Ptr tree_root = std::make_shared<DummyNode>();
    xpathselect::NodeVector result = xpathselect::SelectNodes(tree_root, "");

    ASSERT_EQ(1, result.size());
}

// test explicitly selecting tree root without node name
TEST(TestXPath, test_select_tree_root)
{
    xpathselect::Node::Ptr tree_root = std::make_shared<DummyNode>("RootNode");
    xpathselect::NodeVector result = xpathselect::SelectNodes(tree_root, "/");

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.front(), tree_root);
}

// test explicitly selecting tree root without node name
TEST(TestXPath, test_select_tree_root_with_name)
{
    xpathselect::Node::Ptr tree_root = std::make_shared<DummyNode>("RootNode");
    xpathselect::NodeVector result = xpathselect::SelectNodes(tree_root, "/RootNode");

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.front(), tree_root);
}

// test explicitly selecting tree root with relative query
TEST(TestXPath, test_select_tree_root_with_relative_query)
{
    xpathselect::Node::Ptr tree_root = std::make_shared<DummyNode>("RootNode");
    xpathselect::NodeVector result = xpathselect::SelectNodes(tree_root, "//RootNode");

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.front(), tree_root);
}

// test explicitly selecting tree root with relative query
TEST(TestXPath, test_select_tree_root_with_empty_relative_query)
{
    xpathselect::Node::Ptr tree_root = std::make_shared<DummyNode>("RootNode");
    xpathselect::NodeVector result = xpathselect::SelectNodes(tree_root, "//");

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result.front(), tree_root);
}
