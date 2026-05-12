// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "../include/stringview.h"

namespace CcpParser
{

std::string ToString( const StringView& result )
{
	if( result.begin )
	{
		return std::string( result.begin, result.end );
	}
	return {};
}

bool operator==( const char* a, const StringView& b )
{
	return strncmp( a, b.begin, b.end - b.begin ) == 0 && a[b.end - b.begin] == 0;
}

bool operator==( const StringView& a, const char* b )
{
	return b == a;
}

bool operator==( const StringView& a, const StringView& b )
{
	return a.end - a.begin == b.end - b.begin && strncmp( a.begin, b.begin, a.end - a.begin ) == 0;
}
}