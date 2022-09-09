////////////////////////////////////////////////////////////
//
//    Creator:   Filipp Pavlov
//    Created:   August 2022
//    Copyright: CCP 2022
//

#include "pch.h"
#include "../include/ccpparser.h"
#include "helpers.h"

using namespace CcpParser;

TEST(Basic, CannotParseEmptyInput)
{
	ASSERT_FALSE(CanParse(""));
	ASSERT_FALSE(CanParse(" "));
}

TEST(Basic, CanParseInt)
{
	ASSERT_TRUE(CanParse("123"));
	ASSERT_TRUE(CanParse("-123"));
	ASSERT_TRUE(CanParse("+123"));
}

TEST(Basic, CanParseFloats)
{
	ASSERT_TRUE(CanParse("123."));
	ASSERT_TRUE(CanParse("123.234"));
	ASSERT_TRUE(CanParse("123.234e-1"));
}

TEST(Basic, CanParseSimpleOperations)
{
	ASSERT_TRUE(CanParse("1 + 2"));
}

TEST(Basic, HandlesSyntaxErrors)
{
	ASSERT_EQ(ParseResult::SCANNER_ERROR, CanParse("1..").type);
	ASSERT_EQ(ParseResult::SYNTAX_ERROR, CanParse("1 +").type);
}
