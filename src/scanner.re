// Copyright © 2022 CCP ehf.

#include "src/pch.h"
#include "src/parserstate.h"

/*!re2c
    re2c:yyfill:enable = 0;
    re2c:define:YYCTYPE = char;
*/
namespace CcpParser
{

bool UnescapeString(const StringView &str, std::vector<uint8_t>& buffer)
{
    const char* YYCURSOR = str.begin + 1;
    const char* YYLIMIT = str.end - 1;
    for (char u;; buffer.push_back(u)) 
    {
        /*!re2c
            "\\a"                { u = '\a'; continue; }
            "\\b"                { u = '\b'; continue; }
            "\\f"                { u = '\f'; continue; }
            "\\n"                { u = '\n'; continue; }
            "\\r"                { u = '\r'; continue; }
            "\\t"                { u = '\t'; continue; }
            "\\v"                { u = '\v'; continue; }
            "\\\\"               { u = '\\'; continue; }
            "\\'"                { u = '\''; continue; }
            "\\\""               { 
                u = '"';  continue; 
            }
            "\\?"                { u = '?';  continue; }
            [^\n\\]              { 
                u = YYCURSOR[-1]; 
                if (u == '\"') 
                break; 
                continue; 
            }
            *                    { 
                return false; 
            }
        */
    }
    return true;
}

bool CompareUnescapedString(const StringView &str, const char* escapedString, const char* escapedStringEnd)
{
    const char* YYCURSOR = str.begin + 1;
    const char* YYLIMIT = str.end - 1;

    for (char u;;) 
    {
        /*!re2c
            "\\a"                { u = '\a'; goto compare; }
            "\\b"                { u = '\b'; goto compare; }
            "\\f"                { u = '\f'; goto compare; }
            "\\n"                { u = '\n'; goto compare; }
            "\\r"                { u = '\r'; goto compare; }
            "\\t"                { u = '\t'; goto compare; }
            "\\v"                { u = '\v'; goto compare; }
            "\\\\"               { u = '\\'; goto compare; }
            "\\'"                { u = '\''; goto compare; }
            "\\\""               { 
                u = '"';  goto compare; 
            }
            "\\?"                { u = '?';  goto compare; }
            [^\n\\]              { 
                u = YYCURSOR[-1]; 
                if (u == '\"') 
                break; 
                goto compare; 
            }
            *                    { 
                return false; 
            }
        */
compare:
        if( u != *escapedString )
        {
            return false;
        }
        ++escapedString;
        if( escapedString >= escapedStringEnd )
        {
            return false;
        }
    }
    return *escapedString == 0;
}

const char* ScanString(const char *s)
{
    const char* YYCURSOR = s;
    for (char u;;) 
    {
        /*!re2c
            "\\a"                { u = '\a'; continue; }
            "\\b"                { u = '\b'; continue; }
            "\\f"                { u = '\f'; continue; }
            "\\n"                { u = '\n'; continue; }
            "\\r"                { u = '\r'; continue; }
            "\\t"                { u = '\t'; continue; }
            "\\v"                { u = '\v'; continue; }
            "\\\\"               { u = '\\'; continue; }
            "\\'"                { u = '\''; continue; }
            "\\\""               { 
                u = '"';  continue; 
            }
            "\\?"                { u = '?';  continue; }
            [^\n\\\000]              { 
                u = YYCURSOR[-1]; 
                if (u == '\"') 
                break; 
                continue; 
            }
            *                    { 
                return nullptr; 
            }
        */
    }
    return YYCURSOR;
}


float ScanFloat(const char *s)
{
    double d = 0;
    double x = 1;
    int e = 0;
    const char *YYCURSOR = s;
mant_int:
    /*!re2c
        "."   { goto mant_frac; }
        [eE]  { goto exp_sign; }
        [0-9] { d = (d * 10) + (YYCURSOR[-1] - '0'); goto mant_int; }
        ""    { goto end; }
    */
mant_frac:
    /*!re2c
        ""    { goto end; }
        [eE]  { goto exp_sign; }
        [0-9] { d += (x /= 10) * (YYCURSOR[-1] - '0'); goto mant_frac; }
    */
exp_sign:
    /*!re2c
        "+"?  { x = 1e+1; goto exp; }
        "-"   { x = 1e-1; goto exp; }
    */
exp:
    /*!re2c
        ""    { for (; e > 0; --e) d *= x;    goto end; }
        [0-9] { e = (e * 10) + (YYCURSOR[-1] - '0'); goto exp; }
    */
end:
    return float(d);
}


Token ScanToken( const char *s ) 
{
    const char *YYCURSOR = s;
    const char *YYMARKER = s;
    while (true)
    {
		auto start = YYCURSOR;
    /*!re2c
		ANY = [\001-\377]; 
		LETTER = [a-zA-Z_]; 
		DECIMAL = [0-9]; 
		OCTAL = [0-7]; 
		HEX = [a-fA-F0-9]; 
		ESC	= [\\] ([abfnrtv?'"\\] | "x" HEX+ | OCTAL+); 
		EXPONENT	= [Ee] [+-]? DECIMAL+; 
		SPACE = [ \t\v\f\r\n]+;
		ID = LETTER(LETTER|DECIMAL)*;
		FLOAT_CONST = ((DECIMAL+) | (DECIMAL+ EXPONENT) | (DECIMAL* "." DECIMAL+ EXPONENT?) | (DECIMAL+ "." DECIMAL* EXPONENT?));
        STRING_CONST = ["][^"]*[^\\]["];

        SPACE { continue; }
        [\000] { return {OP_EOF}; }
		FLOAT_CONST { 
#ifndef __APPLE__
            float value = 0;
            auto converted = std::from_chars( start, YYCURSOR, value );
            if( converted.ec == std::errc() )
            {
                return { OP_FLOAT_CONST, start, YYCURSOR, value }; 
            }
            else
            {
                return { OP_ERROR, start, YYCURSOR + 1 };
            }
#else
        	static auto cLocale = newlocale( LC_ALL_MASK, NULL, NULL );
            return { OP_FLOAT_CONST, start, YYCURSOR, float( atof_l( start, cLocale ) ) };
#endif
        }
		["] { 
            YYCURSOR = ScanString(YYCURSOR);
            if (YYCURSOR)
            {
                return { OP_STRING_CONST, start, YYCURSOR }; 
            }
            else
            {
                return { OP_ERROR, start, start + 1 };
            }
        }
		ID { return { OP_ID, start, YYCURSOR }; }
		"&&" { return { OP_AND, start, YYCURSOR }; }
		"||" { return { OP_OR, start, YYCURSOR }; }
		"==" { return { OP_EQ, start, YYCURSOR }; }
		"!=" { return { OP_NE, start, YYCURSOR }; }
		">=" { return { OP_GE, start, YYCURSOR }; }
		"<=" { return { OP_LE, start, YYCURSOR }; }
		">" { return { OP_GT, start, YYCURSOR }; }
		"<" { return { OP_LT, start, YYCURSOR }; }
		"+" { return { OP_PLUS, start, YYCURSOR }; }
		"-" { return { OP_MINUS, start, YYCURSOR }; }
		"*" { return { OP_TIMES, start, YYCURSOR }; }
		"/" { return { OP_DIVIDE, start, YYCURSOR }; }
		"%" { return { OP_MOD, start, YYCURSOR }; }
		"^" { return { OP_EXP, start, YYCURSOR }; }
		"!" { return { OP_NOT, start, YYCURSOR }; }
		"(" { return { OP_LEFT_PAR, start, YYCURSOR }; }
		")" { return { OP_RIGHT_PAR, start, YYCURSOR }; }
		"," { return { OP_COMA, start, YYCURSOR }; }
		"?" { return { OP_QUESTION, start, YYCURSOR }; }
		":" { return { OP_COLON, start, YYCURSOR }; }
        * { return { OP_ERROR, YYCURSOR, YYCURSOR + 1 }; }
    */
    }
}

}