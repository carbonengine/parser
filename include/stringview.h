////////////////////////////////////////////////////////////
//
//    Creator:   Filipp Pavlov
//    Created:   August 2022
//    Copyright: CCP 2022
//

#pragma once

#include <string>

namespace CcpParser
{

struct StringView
{
	const char* begin;
	const char* end;
};

std::string ToString( const StringView& result );
bool operator==( const char* a, const StringView& b );
bool operator==( const StringView& a, const char* b );
bool operator==( const StringView& a, const StringView& b );

}