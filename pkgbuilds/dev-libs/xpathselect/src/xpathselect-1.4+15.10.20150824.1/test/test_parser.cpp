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

#include "parser.h"
#include "xpathquerypart.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/variant/variant.hpp>

#include <iostream>
#include <string>
#include <cstdlib>
#include <typeinfo>
#include <limits>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace parser = xpathselect::parser;


// utility function to test parsers:
template <typename P, typename T>
bool test_parser_attr(std::string input, P const& p, T& attr)
{
    using boost::spirit::qi::parse;

    std::string::iterator f = input.begin();
    std::string::iterator l = input.end();
    if (parse(f, l, p, attr) && (f == l))
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <typename P>
bool test_parser_attr(std::string input, P const& p)
{
    using boost::spirit::qi::parse;

    std::string::iterator f = input.begin();
    std::string::iterator l = input.end();
    if (parse(f, l, p) && (f == l))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// a boost static visitor that checks value equality and type equality.
template <typename T>
class variant_equality_assertion : public boost::static_visitor<>
{
public:
    variant_equality_assertion( T const& expected)
    : expected_(expected)
    {}

    void operator()( T & operand ) const
    {
        ASSERT_EQ(expected_, operand);
    }

    template <typename U> void operator()( U & operand ) const
    {
        FAIL() << "Variant contained incorrect type! Expected: '"
        << expected_
        << "' Actual: '"
        << operand
        << "' Actual type is: "
        << typeid(U).name();
    }
private:
    T expected_;
};

//////////////////////////////////////
// Tests for basic type support:
//////////////////////////////////////


// Test python representations for boolean values:
TEST(TestXPathParser, test_basic_type_boolean)
{
    bool result = false;
    parser::xpath_grammar<std::string::iterator> g;

    ASSERT_TRUE(test_parser_attr("True", g.bool_type, result));
    ASSERT_TRUE(result);

    ASSERT_TRUE(test_parser_attr("False", g.bool_type, result));
    ASSERT_FALSE(result);

    ASSERT_FALSE(test_parser_attr("true", g.bool_type, result));
    ASSERT_FALSE(test_parser_attr("false", g.bool_type, result));
    ASSERT_FALSE(test_parser_attr("1", g.bool_type, result));
    ASSERT_FALSE(test_parser_attr("0", g.bool_type, result));
}


// test character escape codes:
class TestXPathParserCharacterEscapeCodes : public ::testing::TestWithParam<std::pair<std::string, char> >
{
};


TEST_P(TestXPathParserCharacterEscapeCodes, test_character_escape_codes)
{
    auto p = GetParam();

    std::string input = p.first;
    char expected_result = p.second;

    char actual_result = 0;
    parser::xpath_grammar<std::string::iterator> g;

    ASSERT_TRUE(test_parser_attr(input, g.unesc_char, actual_result));
    ASSERT_EQ(expected_result, actual_result);
}

INSTANTIATE_TEST_CASE_P(BasicCharacterCodes,
                        TestXPathParserCharacterEscapeCodes,
                        ::testing::Values(
                            std::pair<std::string, char>("\\a", '\a'),
                            std::pair<std::string, char>("\\b", '\b'),
                            std::pair<std::string, char>("\\f", '\f'),
                            std::pair<std::string, char>("\\n", '\n'),
                            std::pair<std::string, char>("\\r", '\r'),
                            std::pair<std::string, char>("\\t", '\t'),
                            std::pair<std::string, char>("\\v", '\v'),
                            std::pair<std::string, char>("\\\\", '\\'),
                            std::pair<std::string, char>("\\\'", '\''),
                            std::pair<std::string, char>("\\\"", '\"')
                        ));


class QuotedStringTests : public ::testing::TestWithParam<std::tuple<const char*, const char*, bool> >
{
};

TEST_P(QuotedStringTests, quoted_string_parameter_test)
{
    std::string input = std::get<0>(GetParam());
    std::string expected_output = std::get<1>(GetParam());
    bool expected_pass = std::get<2>(GetParam());

    std::string actual_result;
    parser::xpath_grammar<std::string::iterator> g;

    ASSERT_EQ(expected_pass, test_parser_attr(input, g.unesc_str, actual_result));
    if (expected_pass)
        ASSERT_EQ(expected_output, actual_result);
}


INSTANTIATE_TEST_CASE_P(BasicStrings,
                        QuotedStringTests,
                        ::testing::Values(
                            std::make_tuple("\"Hello\"", "Hello", true),
                            std::make_tuple("\"Hello World\"", "Hello World", true),
                            std::make_tuple("\"a b  c   d\"", "a b  c   d", true),
                            std::make_tuple("\"\\x41\"", "A", true),
                            std::make_tuple("\"\\x08\"", "\b", true)
                            ));

INSTANTIATE_TEST_CASE_P(PunctuationStrings,
                        QuotedStringTests,
                        ::testing::Values(
                            std::make_tuple("\".\"", ".", true),
                            std::make_tuple("\",\"", ",", true),
                            std::make_tuple("\"<\"", "<", true),
                            std::make_tuple("\">\"", ">", true),
                            std::make_tuple("\"/\"", "/", true),
                            std::make_tuple("\"?\"", "?", true),
                            std::make_tuple("\":\"", ":", true),
                            std::make_tuple("\";\"", ";", true),
                            std::make_tuple("\"'\"", "'", true), // '"' tested below
                            std::make_tuple("\"[\"", "[", true),
                            std::make_tuple("\"]\"", "]", true),
                            std::make_tuple("\"{\"", "{", true),
                            std::make_tuple("\"}\"", "}", true),
                            std::make_tuple("\"\\\\\"", "\\", true),
                            std::make_tuple("\"|\"", "|", true),
                            std::make_tuple("\"~\"", "~", true),
                            std::make_tuple("\"`\"", "`", true),
                            std::make_tuple("\"!\"", "!", true),
                            std::make_tuple("\"@\"", "@", true),
                            std::make_tuple("\"#\"", "#", true),
                            std::make_tuple("\"$\"", "$", true),
                            std::make_tuple("\"%\"", "%", true),
                            std::make_tuple("\"^\"", "^", true),
                            std::make_tuple("\"&\"", "&", true),
                            std::make_tuple("\"*\"", "*", true),
                            std::make_tuple("\"(\"", "(", true),
                            std::make_tuple("\")\"", ")", true),
                            std::make_tuple("\"-\"", "-", true),
                            std::make_tuple("\"_\"", "_", true),
                            std::make_tuple("\"+\"", "+", true),
                            std::make_tuple("\"=\"", "=", true)
                            ));

INSTANTIATE_TEST_CASE_P(QuoteStrings,
                        QuotedStringTests,
                        ::testing::Values(
                            std::make_tuple("\"\\\"\"", "\"", true),
                            std::make_tuple("\"\\\'\"", "\'", true)
                            ));

INSTANTIATE_TEST_CASE_P(NumberStrings,
                        QuotedStringTests,
                        ::testing::Values(
                            std::make_tuple("\"0\"", "0", true),
                            std::make_tuple("\"1\"", "1", true),
                            std::make_tuple("\"2\"", "2", true),
                            std::make_tuple("\"3\"", "3", true),
                            std::make_tuple("\"4\"", "4", true),
                            std::make_tuple("\"5\"", "5", true),
                            std::make_tuple("\"6\"", "6", true),
                            std::make_tuple("\"7\"", "7", true),
                            std::make_tuple("\"8\"", "8", true),
                            std::make_tuple("\"9\"", "9", true)
                            ));


TEST(TestIntegerTypes, test_signed_integers)
{
    int result = 0;
    parser::xpath_grammar<std::string::iterator> g;

    ASSERT_TRUE(test_parser_attr("123", g.int_type, result));
    ASSERT_EQ(123, result);

    ASSERT_TRUE(test_parser_attr("+456", g.int_type, result));
    ASSERT_EQ(456, result);

    ASSERT_TRUE(test_parser_attr("-123", g.int_type, result));
    ASSERT_EQ(-123, result);
}


// This test fails due to a bug in boost::spirit: https://svn.boost.org/trac/boost/ticket/9007
// TEST(TestIntegerTypes, test_integer_overflow)
// {
//     int result;

//     // store range of int in a long, since we'll be extending them
//     long min_int = std::numeric_limits<int>::min();
//     long max_int = std::numeric_limits<int>::max();

//     qi::int_parser<int> r;

//     ASSERT_TRUE(test_parser_attr(std::to_string(min_int), r, result));
//     ASSERT_EQ(min_int, result);

//     ASSERT_TRUE(test_parser_attr(std::to_string(max_int), r, result));
//     ASSERT_EQ(max_int, result);

//     min_int -= 1;
//     max_int += 1;

//     // these last two assertions are failing. I expect the parsing to fail, but it's passing
//     // for some reason.
//     ASSERT_FALSE(test_parser_attr(std::to_string(min_int), r, result)) << min_int;
//     ASSERT_FALSE(test_parser_attr(std::to_string(max_int), r, result)) << max_int;
// }


////////////////////////////////////
// more complicated grammar tests
////////////////////////////////////

/// Tests for parameter names:
class TestXPathParserParamNames : public ::testing::TestWithParam<std::pair<std::string, bool> >
{
};

TEST_P(TestXPathParserParamNames, test_param_name)
{
    auto p = GetParam();

    std::string input = p.first;
    bool expect_pass = p.second;

    parser::xpath_grammar<std::string::iterator> g;

    std::string result;
    ASSERT_EQ( expect_pass,  test_parser_attr(input, g.param_name, result) );
    if (expect_pass)
        ASSERT_EQ(input, result);
}

INSTANTIATE_TEST_CASE_P(BasicNodeNames,
                        TestXPathParserParamNames,
                        ::testing::Values(
                            std::pair<std::string, bool>("a b", false),
                            std::pair<std::string, bool>("a*", false),
                            std::pair<std::string, bool>("HelloWorld", true),
                            std::pair<std::string, bool>("H", true),
                            std::pair<std::string, bool>("h", true),
                            std::pair<std::string, bool>("1", true),
                            std::pair<std::string, bool>("node-name", true),
                            std::pair<std::string, bool>("node_name", true),
                            std::pair<std::string, bool>("node\\name", true),
                            std::pair<std::string, bool>("node::name", false),
                            std::pair<std::string, bool>("node.name", false),
                            std::pair<std::string, bool>("node name", false),
                            std::pair<std::string, bool>("..", false)
                         ));

/// Tests for parameter values. This test is much larger than it should be, since it seems to be
// impossible to parameterise tests for both type and value. The solution I use here is to have
// the actual test in a base class template method, and have several derive classes use different
// value parameters. Ugly, but probably the best we can do with google test.

class TestParamValues : public ::testing::Test
{
public:
    template <typename PairType>
    void test_param_value(PairType const& input_pair) const
    {
        RecordProperty("FirstType", typeid(typename PairType::first_type).name());
        RecordProperty("SecondType", typeid(typename PairType::second_type).name());
        std::string input = input_pair.first;
        typename PairType::second_type expected_result = input_pair.second;

        parser::xpath_grammar<std::string::iterator> g;

        xpathselect::XPathQueryParam::ParamValueType result;
        ASSERT_TRUE( test_parser_attr(input, g.param_value, result) );
        boost::apply_visitor(
            variant_equality_assertion<typename PairType::second_type>(expected_result),
            result
            );
    }
};

// test string parameter values:
class TestStringParamValues
 : public ::testing::WithParamInterface<std::pair<std::string, std::string> >
 , public TestParamValues
{
};

TEST_P(TestStringParamValues, test_param_value_str)
{
    auto p = GetParam();
    test_param_value(p);
}

INSTANTIATE_TEST_CASE_P(StringParams,
                        TestStringParamValues,
                        ::testing::Values(
                            std::pair<std::string, std::string>("\"a b\"", "a b" ),
                            std::pair<std::string, std::string>("\"a.b,c/\\d^\"", "a.b,c/\\d^" )
                         ));

// test boolean parameter values:
class TestBoolParamValues
 : public ::testing::WithParamInterface<std::pair<std::string, bool> >
 , public TestParamValues
{
};

TEST_P(TestBoolParamValues, test_param_value_bool)
{
    auto p = GetParam();
    test_param_value(p);
}

INSTANTIATE_TEST_CASE_P(StringParams,
                        TestBoolParamValues,
                        ::testing::Values(
                            std::pair<std::string, bool>("True", true ),
                            std::pair<std::string, bool>("False", false )
                         ));

// test integer parameter values:
class TestIntParamValues
 : public ::testing::WithParamInterface<std::pair<std::string, int> >
 , public TestParamValues
{
};

TEST_P(TestIntParamValues, test_param_value_bool)
{
    auto p = GetParam();
    test_param_value(p);
}

INSTANTIATE_TEST_CASE_P(IntegerParams,
                        TestIntParamValues,
                        ::testing::Values(
                            std::pair<std::string, int>("123", 123 ),
                            std::pair<std::string, int>("0", 0 ),
                            std::pair<std::string, int>("-123", -123 )
                         ));


/// Tests for the node names:
class TestXPathParserNodeNames : public ::testing::TestWithParam<std::pair<std::string, bool> >
{
};


TEST_P(TestXPathParserNodeNames, test_spec_node_name)
{
    auto p = GetParam();

    std::string input = p.first;
    bool expect_pass = p.second;

    parser::xpath_grammar<std::string::iterator> g;

    std::string result;
    ASSERT_EQ( expect_pass,  test_parser_attr(input, g.spec_node_name, result) );
    if (expect_pass)
        ASSERT_EQ(input, result);
}

INSTANTIATE_TEST_CASE_P(BasicNodeNames,
                        TestXPathParserNodeNames,
                        ::testing::Values(
                            std::pair<std::string, bool>("a b", true),
                            std::pair<std::string, bool>("a ", false),
                            std::pair<std::string, bool>(" ", false),
                            std::pair<std::string, bool>(" b", false),
                            std::pair<std::string, bool>("a    b", true),
                            std::pair<std::string, bool>("a b b a", true),
                            std::pair<std::string, bool>("a*", false),
                            std::pair<std::string, bool>("HelloWorld", true),
                            std::pair<std::string, bool>("H", true),
                            std::pair<std::string, bool>("h", true),
                            std::pair<std::string, bool>("1", true),
                            std::pair<std::string, bool>("node-name", true),
                            std::pair<std::string, bool>("node_name", true),
                            std::pair<std::string, bool>("node\\name", true),
                            std::pair<std::string, bool>("node::name", true),
                            std::pair<std::string, bool>("node::name::extra", true)
                         ));


class TestXPathParserWildcardNodeName : public ::testing::TestWithParam<std::pair<std::string, bool> >
{
};

TEST_P(TestXPathParserWildcardNodeName, test_wildcard_node_name_rejects_everything_else)
{
    auto p = GetParam();

    std::string input = p.first;
    bool expect_pass = p.second;

    parser::xpath_grammar<std::string::iterator> g;

    std::string result;
    ASSERT_EQ( expect_pass,  test_parser_attr(input, g.wildcard_node_name, result) );
    if (expect_pass)
        ASSERT_EQ(input, result);
}

INSTANTIATE_TEST_CASE_P(BasicNodeNames,
                        TestXPathParserWildcardNodeName,
                        ::testing::Values(
                            std::pair<std::string, bool>("", false),
                            std::pair<std::string, bool>("* ", false),
                            std::pair<std::string, bool>("**", false),
                            std::pair<std::string, bool>(" ", false),
                            std::pair<std::string, bool>("8", false),
                            std::pair<std::string, bool>("\t", false),
                            std::pair<std::string, bool>("node-name", false),
                            std::pair<std::string, bool>("node_name", false),
                            std::pair<std::string, bool>("*", true)
                         ));


TEST(TestXPathParser, test_param_parser_string_value_works)
{
    std::string input("name=\"value\"");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryParam result;
    ASSERT_EQ( true,  test_parser_attr(input, g.param, result) );
    ASSERT_EQ("name", result.param_name);
    boost::apply_visitor(
        variant_equality_assertion<std::string>("value"),
        result.param_value
        );
}

TEST(TestXPathParser, test_param_parser_bool_value_works)
{
    std::string input("name=True");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryParam result;
    ASSERT_EQ( true,  test_parser_attr(input, g.param, result) );
    ASSERT_EQ("name", result.param_name);
    boost::apply_visitor(
        variant_equality_assertion<bool>(true),
        result.param_value
        );
}

TEST(TestXPathParser, test_param_parser_string_int_works)
{
    std::string input("name=123456");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryParam result;
    ASSERT_EQ( true,  test_parser_attr(input, g.param, result) );
    ASSERT_EQ("name", result.param_name);
    boost::apply_visitor(
        variant_equality_assertion<int>(123456),
        result.param_value
        );
}

TEST(TestXPathParser, test_param_parser_fails)
{
    std::string input("name=");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryParam result;
    ASSERT_FALSE( test_parser_attr(input, g.param, result) );
}

TEST(TestXPathParser, test_param_list_single_value)
{
    std::string input("[name=123]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::ParamList result;
    ASSERT_EQ( true,  test_parser_attr(input, g.param_list, result) );
    ASSERT_EQ(1, result.size());
    ASSERT_EQ("name", result.at(0).param_name);
    boost::apply_visitor(
        variant_equality_assertion<int>(123),
        result.at(0).param_value
        );
}

TEST(TestXPathParser, test_param_list_two_values)
{
    std::string input("[name=\"value\",visible=True]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::ParamList result;
    ASSERT_EQ( true,  test_parser_attr(input, g.param_list, result) );
    ASSERT_EQ(2, result.size());
    ASSERT_EQ("name", result.at(0).param_name);
    boost::apply_visitor(
        variant_equality_assertion<std::string>("value"),
        result.at(0).param_value
        );
    ASSERT_EQ("visible", result.at(1).param_name);
    boost::apply_visitor(
        variant_equality_assertion<bool>(true),
        result.at(1).param_value
        );
}

TEST(TestXPathParser, test_spec_node_with_parameter)
{
    std::string input("node_name[param_name=123]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.spec_node, result) );
    ASSERT_EQ("node_name", result.node_name_);
    ASSERT_FALSE(result.parameter.empty());
    ASSERT_EQ("param_name", result.parameter.at(0).param_name);
    boost::apply_visitor(
        variant_equality_assertion<int>(123),
        result.parameter.at(0).param_value
        );
}

TEST(TestXPathParser, test_spec_node_without_parameter)
{
    std::string input("node_name");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.spec_node, result) );
    ASSERT_EQ("node_name", result.node_name_);
    ASSERT_TRUE(result.parameter.empty());
}

TEST(TestXPathParser, test_wildcard_node)
{
    std::string input("*");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.wildcard_node, result) );
    ASSERT_EQ("*", result.node_name_);
    ASSERT_TRUE(result.parameter.empty());
}

TEST(TestXPathParser, test_wildcard_node_rejects_parameters)
{
    std::string input("*[foo=bar]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_FALSE( test_parser_attr(input, g.wildcard_node, result) );
}

TEST(TestXPathParser, test_wildcard_node_with_params)
{
    std::string input("*[param_name=123]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.wildcard_node_with_params, result) );
    ASSERT_EQ("*", result.node_name_);
    ASSERT_FALSE(result.parameter.empty());
    ASSERT_EQ("param_name", result.parameter.at(0).param_name);
    boost::apply_visitor(
        variant_equality_assertion<int>(123),
        result.parameter.at(0).param_value
        );
}


TEST(TestXPathParser, test_node_can_be_a_wildcard_node_with_params)
{
    std::string input("*[name=\"value\"]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.node, result) );
    ASSERT_EQ( "*", result.node_name_ );
    ASSERT_EQ( xpathselect::XPathQueryPart::QueryPartType::Normal, result.Type() );
    ASSERT_EQ( 1, result.parameter.size() );
    ASSERT_EQ( "name", result.parameter.at(0).param_name );
    boost::apply_visitor(
        variant_equality_assertion<std::string>("value"),
        result.parameter.at(0).param_value
        );
}

TEST(TestXPathParser, test_node_can_be_a_wildcard_node_without_params)
{
    std::string input("*");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.node, result) );
    ASSERT_EQ( "*", result.node_name_ );
    ASSERT_EQ( xpathselect::XPathQueryPart::QueryPartType::Normal, result.Type() );
}

TEST(TestXPathParser, test_node_can_be_a_spec_node_with_params)
{
    std::string input("foo[name=\"value\"]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.node, result) );
    ASSERT_EQ( "foo", result.node_name_ );
    ASSERT_EQ( xpathselect::XPathQueryPart::QueryPartType::Normal, result.Type() );
    ASSERT_EQ( 1, result.parameter.size() );
    ASSERT_EQ( "name", result.parameter.at(0).param_name );
    boost::apply_visitor(
        variant_equality_assertion<std::string>("value"),
        result.parameter.at(0).param_value
        );
}

TEST(TestXPathParser, test_node_can_be_a_spec_node_without_params)
{
    std::string input("foo");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.node, result) );
    ASSERT_EQ( "foo", result.node_name_ );
    ASSERT_EQ( xpathselect::XPathQueryPart::QueryPartType::Normal, result.Type() );
}

TEST(TestXPathParser, test_node_can_be_a_parent_node)
{
    std::string input("..");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_EQ( true,  test_parser_attr(input, g.node, result) );
    ASSERT_EQ( "..", result.node_name_ );
    ASSERT_EQ( xpathselect::XPathQueryPart::QueryPartType::Parent, result.Type() );
}

TEST(TestXPathParser, test_search_node_followed_by_normal_node)
{
    // the search_node grammar fails if it's at the end of the line, so we need
    // to give it some more data, even though we're not actually matching it.
    std::string input("//node_name");
    parser::xpath_grammar<std::string::iterator> g;
    xpathselect::XPathQueryPart result;

    // however, this means we can't use the test_parser_attr function, since it
    // returns false on a partial match. Use the parse(...) function directly:
    ASSERT_TRUE( parse(input.begin(), input.end(),g.search_node, result) );
    ASSERT_TRUE( result.Type() == xpathselect::XPathQueryPart::QueryPartType::Search );
}

TEST(TestXPathParser, test_search_node_followed_by_wildcard_node_with_parameters)
{
    // the search_node grammar fails if it's at the end of the line, so we need
    // to give it some more data, even though we're not actually matching it.
    std::string input("//*[foo=\"bar\"]");
    parser::xpath_grammar<std::string::iterator> g;
    xpathselect::XPathQueryPart result;

    // however, this means we can't use the test_parser_attr function, since it
    // returns false on a partial match. Use the parse(...) function directly:
    ASSERT_TRUE( parse(input.begin(), input.end(),g.search_node, result) );
    ASSERT_TRUE( result.Type() == xpathselect::XPathQueryPart::QueryPartType::Search );
}

TEST(TestXPathParser, test_search_node_cannot_have_parameters)
{
    std::string input("//[param_name=value]");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_FALSE( test_parser_attr(input, g.search_node, result) );
}

TEST(TestXPathParser, test_parent_node)
{
    std::string input("..");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::XPathQueryPart result;
    ASSERT_TRUE( test_parser_attr(input, g.parent_node, result) );
    ASSERT_TRUE( result.Type() == xpathselect::XPathQueryPart::QueryPartType::Parent );
}

TEST(TestXPathParser, test_normal_sep_works)
{
    std::string input("/");

    parser::xpath_grammar<std::string::iterator> g;

    ASSERT_EQ( true,  test_parser_attr(input, g.normal_sep) );
}

TEST(TestXPathParser, test_normal_sep_does_not_match_search_node)
{
    std::string input("//");

    parser::xpath_grammar<std::string::iterator> g;

    ASSERT_FALSE( test_parser_attr(input, g.normal_sep) );
}

TEST(TestXPathParser, test_can_extract_query_list)
{
    std::string input("/node1/node2");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::QueryList result;
    ASSERT_TRUE(test_parser_attr(input, g.node_sequence, result));
    ASSERT_EQ(2, result.size());
    ASSERT_EQ("node1", result.at(0).node_name_);
    ASSERT_TRUE(result.at(0).parameter.empty());
    ASSERT_EQ("node2", result.at(1).node_name_);
    ASSERT_TRUE(result.at(1).parameter.empty());
}

TEST(TestXPathParser, test_can_extract_query_list_with_search)
{
    std::string input("//node1");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::QueryList result;
    ASSERT_TRUE(test_parser_attr(input, g.node_sequence, result));
    ASSERT_EQ(2, result.size());
    ASSERT_TRUE(result.at(0).Type() == xpathselect::XPathQueryPart::QueryPartType::Search );
    ASSERT_TRUE(result.at(1).Type() == xpathselect::XPathQueryPart::QueryPartType::Normal );
    ASSERT_TRUE(result.at(0).parameter.empty());
    ASSERT_EQ("node1", result.at(1).node_name_);
    ASSERT_TRUE(result.at(1).parameter.empty());
}

TEST(TestXPathParser, test_mix_search_and_normal)
{
    std::string input("/node1//node2");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::QueryList result;
    ASSERT_TRUE(test_parser_attr(input, g.node_sequence, result));

    ASSERT_EQ(3, result.size());

    ASSERT_EQ("node1", result.at(0).node_name_);
    ASSERT_TRUE(result.at(0).Type() == xpathselect::XPathQueryPart::QueryPartType::Normal );
    ASSERT_TRUE(result.at(0).parameter.empty());

    ASSERT_TRUE(result.at(1).Type() == xpathselect::XPathQueryPart::QueryPartType::Search );
    ASSERT_TRUE(result.at(1).parameter.empty());

    ASSERT_EQ("node2", result.at(2).node_name_);
    ASSERT_TRUE(result.at(2).Type() == xpathselect::XPathQueryPart::QueryPartType::Normal );
    ASSERT_TRUE(result.at(2).parameter.empty());
}

TEST(TestXPathParser, test_mix_search_and_long_normal)
{
    std::string input("/node1//node2[name=\"val\"]/node3");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::QueryList result;
    ASSERT_TRUE(test_parser_attr(input, g.node_sequence, result));

    ASSERT_EQ(4, result.size());

    ASSERT_EQ("node1", result.at(0).node_name_);
    ASSERT_TRUE(result.at(0).parameter.empty());

    ASSERT_TRUE(result.at(1).Type() == xpathselect::XPathQueryPart::QueryPartType::Search );
    ASSERT_TRUE(result.at(1).parameter.empty());

    ASSERT_EQ("node2", result.at(2).node_name_);
    ASSERT_EQ(1, result.at(2).parameter.size());
    ASSERT_EQ("name", result.at(2).parameter.at(0).param_name);
    boost::apply_visitor(
        variant_equality_assertion<std::string>("val"),
        result.at(2).parameter.at(0).param_value
        );
    ASSERT_EQ("node3", result.at(3).node_name_);
    ASSERT_TRUE(result.at(3).parameter.empty());
}

TEST(TestXPathParser, test_mix_normal_and_parent)
{
    std::string input("/node1/..");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::QueryList result;
    ASSERT_TRUE(test_parser_attr(input, g.node_sequence, result));

    ASSERT_EQ(2, result.size());

    ASSERT_EQ("node1", result.at(0).node_name_);
    ASSERT_TRUE(result.at(0).parameter.empty());

    ASSERT_TRUE(result.at(1).Type() == xpathselect::XPathQueryPart::QueryPartType::Parent );
    ASSERT_TRUE(result.at(1).parameter.empty());
}

TEST(TestXPathParser, test_mix_normal_and_parent_and_wildcard)
{
    std::string input("/node1/../*");

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::QueryList result;
    ASSERT_TRUE(test_parser_attr(input, g.node_sequence, result));

    ASSERT_EQ(3, result.size());

    ASSERT_EQ("node1", result.at(0).node_name_);
    ASSERT_TRUE(result.at(0).parameter.empty());

    ASSERT_TRUE(result.at(1).Type() == xpathselect::XPathQueryPart::QueryPartType::Parent );
    ASSERT_TRUE(result.at(1).parameter.empty());

    ASSERT_TRUE(result.at(2).node_name_ == "*" );
    ASSERT_TRUE(result.at(2).parameter.empty());
}

class TestXPathParserQueryStrings : public ::testing::TestWithParam<std::pair<std::string, bool> >
{};

TEST_P(TestXPathParserQueryStrings, test_query_acceptance)
{
    auto p = GetParam();

    std::string input = p.first;
    bool expect_pass = p.second;

    parser::xpath_grammar<std::string::iterator> g;

    xpathselect::QueryList result;
    ASSERT_EQ( expect_pass,  test_parser_attr(input, g, result) );
}

INSTANTIATE_TEST_CASE_P(BasicNodeNames,
                        TestXPathParserQueryStrings,
                        ::testing::Values(
                            // queries that must all parse correctly:
                            std::pair<std::string, bool>("//root", true),
                            std::pair<std::string, bool>("/root", true),
                            std::pair<std::string, bool>("/root/node1", true),
                            std::pair<std::string, bool>("/root//node1", true),
                            std::pair<std::string, bool>("//root", true),
                            std::pair<std::string, bool>("/root//node1/node2", true),
                            std::pair<std::string, bool>("/root[p=1]//node1[p=\"2\"]/node3", true),
                            std::pair<std::string, bool>("/root[p=True,n=2,d=\"e3\"]", true),
                            std::pair<std::string, bool>("//root[p=1,n=2,d=\"e3\"]", true),
                            std::pair<std::string, bool>("/Root//*[p=1]", true),
                            std::pair<std::string, bool>("/Root//*[p=1,v=\"sj\",c=False]", true),
                            // queries that must not parse correctly:
                            std::pair<std::string, bool>("//", false),
                            std::pair<std::string, bool>("/root//", false),
                            std::pair<std::string, bool>("/root///", false),
                            std::pair<std::string, bool>("/ /", false),
                            std::pair<std::string, bool>("", false),
                            std::pair<std::string, bool>(" ", false),
                            std::pair<std::string, bool>("//*", false),
                            std::pair<std::string, bool>("/Root///Leaf", false),
                            std::pair<std::string, bool>("/Root////", false),
                            std::pair<std::string, bool>("/Root/..*", false),
                            std::pair<std::string, bool>("/Root/../Child//..", false)
                         ));
