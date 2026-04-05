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

#include "xpathselect.h"
#include "node.h"
#include "dummynode.h"

#include <memory>

class TestTreeFixture: public ::testing::Test
{
public:
    void SetUp() override
    {
        root_ = std::make_shared<DummyNode>("Root");
        child_l1_ = std::make_shared<DummyNode>("ChildLeft1");
        child_r1_ = std::make_shared<DummyNode>("ChildRight1");
        leaf_1_ = std::make_shared<DummyNode>("Leaf");
        leaf_2_ = std::make_shared<DummyNode>("Leaf");
        root_->AddChild(child_l1_);
        root_->AddChild(child_r1_);
        child_l1_->AddChild(leaf_1_);
        child_l1_->AddChild(leaf_2_);
    }
    typedef std::shared_ptr<DummyNode> NodePtr;

    NodePtr root_;
    NodePtr child_l1_;
    NodePtr child_r1_;
    NodePtr leaf_1_;
    NodePtr leaf_2_;
};

TEST_F(TestTreeFixture, test_simple)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/");

    ASSERT_EQ(1, result.size());
}

TEST_F(TestTreeFixture, test_simple_absolute)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/ChildLeft1");

    ASSERT_EQ(1, result.size());
    auto expected = child_l1_;
    auto actual = result.front();
    ASSERT_EQ(expected, actual);
}

TEST_F(TestTreeFixture, test_simple_relative)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "//ChildRight1");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_r1_, result.front());
}

TEST_F(TestTreeFixture, test_complex_relative)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "//Root/ChildRight1");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_r1_, result.front());
}

TEST_F(TestTreeFixture, test_relative_multiple_return)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "//Leaf");

    ASSERT_EQ(2, result.size());

    for(xpathselect::Node::Ptr n : result)
    {
        ASSERT_TRUE(n == leaf_1_ || n == leaf_2_ );
    }
}

TEST_F(TestTreeFixture, test_relative_wildcard)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "//ChildLeft1/*");

    ASSERT_EQ(2, result.size());

    for(xpathselect::Node::Ptr n : result)
    {
        ASSERT_TRUE(n == leaf_1_ || n == leaf_2_ );
    }
}

TEST_F(TestTreeFixture, test_absolute_wildcard)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/ChildLeft1/*");

    ASSERT_EQ(2, result.size());

    for(xpathselect::Node::Ptr n : result)
    {
        ASSERT_TRUE(n == leaf_1_ || n == leaf_2_ );
    }
}

TEST_F(TestTreeFixture, test_simple_absolute_property_match)
{
    child_l1_->AddProperty("visible", "True");
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/ChildLeft1[visible=True]");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_l1_, result.front());
}

TEST_F(TestTreeFixture, test_simple_relative_property_match)
{
    child_l1_->AddProperty("visible", true);
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "//ChildLeft1[visible=True]");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_l1_, result.front());
}

TEST_F(TestTreeFixture, test_absolute_multiple_property_match)
{
    root_->AddProperty("number", 45);
    child_l1_->AddProperty("visible", true);
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root[number=45]/ChildLeft1[visible=True]");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_l1_, result.front());
}

TEST_F(TestTreeFixture, test_mixed_query_simple)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root//Leaf");
    ASSERT_EQ(2, result.size());
    for(auto n : result)
    {
        ASSERT_TRUE(n == leaf_1_ || n == leaf_2_ );
    }
}

TEST_F(TestTreeFixture, test_mixed_query_property_match)
{
    leaf_1_->AddProperty("visible", true);
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root//Leaf[visible=True]");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(leaf_1_, result.front());
}

TEST_F(TestTreeFixture, test_search_node_with_wildcard_and_property)
{
    child_l1_->AddProperty("visible", true);
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root//*[visible=True]");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_l1_, result.front());
}

TEST_F(TestTreeFixture, test_wildcard)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/*");
    ASSERT_EQ(2, result.size());
    for(auto n : result)
    {
        ASSERT_TRUE(n == child_l1_ || n == child_r1_ );
    }
}

TEST_F(TestTreeFixture, test_parent)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/ChildLeft1/..");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(root_, result.front());
}

TEST_F(TestTreeFixture, test_parent_on_root)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/..");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(root_, result.front());
}

TEST_F(TestTreeFixture, test_parent_on_leaf)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/ChildLeft1/Leaf/..");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_l1_, result.front());
}

TEST_F(TestTreeFixture, test_double_parent_on_leaf)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/ChildLeft1/Leaf/../..");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(root_, result.front());
}

TEST_F(TestTreeFixture, test_parent_and_child)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root/ChildLeft1/../ChildLeft1/../ChildLeft1");

    ASSERT_EQ(1, result.size());
    ASSERT_EQ(child_l1_, result.front());
}

TEST_F(TestTreeFixture, test_invalid_query_search)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root///Leaf");
    ASSERT_EQ(0, result.size());
}

TEST_F(TestTreeFixture, test_invalid_query_multiple_searches)
{
    xpathselect::NodeVector result = xpathselect::SelectNodes(root_, "/Root////");
    ASSERT_EQ(0, result.size());
}
