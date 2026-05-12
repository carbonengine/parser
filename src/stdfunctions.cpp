// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "stdfunctions.h"

using namespace CcpParser;

namespace
{
float sign( float x )
{
	return x > 0 ? 1.f : ( x < 0 ? -1.f : 0.f );
}

template <typename... Args>
float minv( float arg0, Args... args )
{
	float r;
	( ( r = std::min( arg0, args ) ), ... );
	return r;
}

template <typename... Args>
float maxv( float arg0, Args... args )
{
	float r;
	( ( r = std::max( arg0, args ) ), ... );
	return r;
}

template <typename... Args>
float sumv( float arg0, Args... args )
{
	float r = arg0;
	( ( r += args ), ... );
	return r;
}

template <typename... Args>
float avgv( float arg0, Args... args )
{
	float r = arg0;
	( ( r += args ), ... );
	return r / float( 1 + sizeof...( Args ) );
}

Function s_stdFunctions[] = {
	Function( "sin", &sinf, FunctionFlags::PURE_FUNC ),
	Function( "cos", &cosf, FunctionFlags::PURE_FUNC ),
	Function( "tan", &tanf, FunctionFlags::PURE_FUNC ),
	Function( "asin", &asinf, FunctionFlags::PURE_FUNC ),
	Function( "acos", &acosf, FunctionFlags::PURE_FUNC ),
	Function( "atan", &atanf, FunctionFlags::PURE_FUNC ),
	Function( "sinh", &sinhf, FunctionFlags::PURE_FUNC ),
	Function( "cosh", &coshf, FunctionFlags::PURE_FUNC ),
	Function( "tanh", &tanhf, FunctionFlags::PURE_FUNC ),
	Function( "asinh", &asinhf, FunctionFlags::PURE_FUNC ),
	Function( "acosh", &acoshf, FunctionFlags::PURE_FUNC ),
	Function( "atanh", &atanhf, FunctionFlags::PURE_FUNC ),
	Function( "log2", &log2f, FunctionFlags::PURE_FUNC ),
	Function( "log10", &log10f, FunctionFlags::PURE_FUNC ),
	Function( "log", &logf, FunctionFlags::PURE_FUNC ),
	Function( "ln", &logf, FunctionFlags::PURE_FUNC ),
	Function( "exp", &expf, FunctionFlags::PURE_FUNC ),
	Function( "sqrt", &sqrtf, FunctionFlags::PURE_FUNC ),
	Function( "sign", &sign, FunctionFlags::PURE_FUNC ),
	Function( "rint", &rintf, FunctionFlags::PURE_FUNC ),
	Function( "abs", &fabsf, FunctionFlags::PURE_FUNC ),
	Function( "min", &minv<float>, FunctionFlags::PURE_FUNC ),
	Function( "min", &minv<float, float>, FunctionFlags::PURE_FUNC ),
	Function( "min", &minv<float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "min", &minv<float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "min", &minv<float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "min", &minv<float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "min", &minv<float, float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "max", &maxv<float>, FunctionFlags::PURE_FUNC ),
	Function( "max", &maxv<float, float>, FunctionFlags::PURE_FUNC ),
	Function( "max", &maxv<float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "max", &maxv<float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "max", &maxv<float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "max", &maxv<float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "max", &maxv<float, float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "sum", &sumv<float>, FunctionFlags::PURE_FUNC ),
	Function( "sum", &sumv<float, float>, FunctionFlags::PURE_FUNC ),
	Function( "sum", &sumv<float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "sum", &sumv<float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "sum", &sumv<float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "sum", &sumv<float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "sum", &sumv<float, float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "avg", &avgv<float>, FunctionFlags::PURE_FUNC ),
	Function( "avg", &avgv<float, float>, FunctionFlags::PURE_FUNC ),
	Function( "avg", &avgv<float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "avg", &avgv<float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "avg", &avgv<float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "avg", &avgv<float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
	Function( "avg", &avgv<float, float, float, float, float, float, float>, FunctionFlags::PURE_FUNC ),
};

Constant s_stdConstants[] = {
	{ "_pi", float( 3.141592653589793238462643 ) },
	{ "_e", float( 2.718281828459045235360287 ) },
};
}
namespace CcpParser
{
const FunctionView g_stdFunctions = s_stdFunctions;
const ConstantView g_stdConstants = s_stdConstants;
}