/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMathDetail.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkMathDetail_h
#define __itkMathDetail_h

#include "vnl/vnl_math.h"


// Figure out when the fast implementations can be used
//
// Turn on 32-bit sse2 impl if asked for
#if VNL_CONFIG_ENABLE_SSE2_ROUNDING && defined(__SSE2__) && (!defined(__GCCXML__))
# define USE_SSE2_32IMPL 1
#else
# define USE_SSE2_32IMPL 0
#endif
// Turn on 64-bit sse2 impl on 64-bit architectures if asked for
// NOTE: I have concerns that this expression is correct. Perhaps we
// need parentheses? BCL
#if VNL_CONFIG_ENABLE_SSE2_ROUNDING && defined(__SSE2__) && (defined(__x86_64) || defined(__x86_64__) || defined(_M_X64)) && (!defined(__GCCXML__))
# define USE_SSE2_64IMPL 1
#else
# define USE_SSE2_64IMPL 0
#endif

// Turn on 32-bit and 64-bit asm impl when using GCC on x86 platform with the following exception:
//   GCCXML
#if defined(__GNUC__) && (!defined(__GCCXML__)) &&  (defined(__i386__) || defined(__i386) || defined(__x86_64__) || defined(__x86_64))
# define GCC_USE_ASM_32IMPL 1
# define GCC_USE_ASM_64IMPL 1
#else
# define GCC_USE_ASM_32IMPL 0
# define GCC_USE_ASM_64IMPL 0
#endif
// Turn on 32-bit and 64-bit asm impl when using msvc on 32 bits windows
#if defined(VCL_VC) && (!defined(__GCCXML__)) && !defined(_WIN64)
# define VC_USE_ASM_32IMPL 1
# define VC_USE_ASM_64IMPL 1
#else
# define VC_USE_ASM_32IMPL 0
# define VC_USE_ASM_64IMPL 0
#endif


namespace itk 
{
namespace Math 
{
namespace Detail
{
// The functions defined in this namespace are not meant to be used directly
// and thus do not adhere to the standard backward-compatibility
// policy of ITK, as any Detail namespace should be considered private.
// Please use the functions from the itk::Math namespace instead

////////////////////////////////////////
// Base versions

template <typename TReturn, typename TInput>
inline TReturn RoundHalfIntegerToEven_base(TInput x)
{
  if ( x >= static_cast<TInput>( 0 ) ) 
    {
    x += static_cast<TInput>( 0.5 );
    }
  else 
    {
    x -= static_cast<TInput>( 0.5 );
    }

  const TReturn r = static_cast<TReturn>( x );
  return ( x != static_cast<TInput>( r ) ) ? r : static_cast<TReturn>( 2*r/2 );
}

template <typename TReturn, typename TInput>
inline TReturn RoundHalfIntegerUp_base(TInput x)
{
  x += static_cast<TInput>( 0.5 );
  const TReturn r = static_cast<TReturn>( x );
  return ( r >= static_cast<TReturn>( 0 ) ) ?
    r :
    ( x == static_cast<TInput>( r ) ? r : r - static_cast<TReturn>( 1 ) );
}

template <typename TReturn, typename TInput>
inline TReturn Floor_base(TInput x)
{
  const TReturn r = static_cast<TReturn>( x );
  return ( x >=static_cast<TInput>( 0 ) ) ?
    r :
    ( x == static_cast<TInput>( r ) ? r : r - static_cast<TReturn>( 1 ) );
}

template <typename TReturn, typename TInput>
inline TReturn Ceil_base(TInput x)
{
  const TReturn r = static_cast<TReturn>( x );
  return ( x < static_cast<TInput>( 0 ) ) ?
    r :
    ( x == static_cast<TInput>( r ) ? r : r + static_cast<TReturn>( 1 ) );
}

////////////////////////////////////////
// 32 bits versions


#if USE_SSE2_32IMPL // sse2 implementation

inline vxl_int_32 RoundHalfIntegerToEven_32(double x) { return _mm_cvtsd_si32(_mm_set_sd(x)); }
inline vxl_int_32 RoundHalfIntegerToEven_32(float  x) { return _mm_cvtss_si32(_mm_set_ss(x)); }

#elif GCC_USE_ASM_32IMPL // gcc asm implementation

inline vxl_int_32 RoundHalfIntegerToEven_32(double x)
{
  vxl_int_32 r;
  __asm__ __volatile__( "fistpl %0" : "=m"(r) : "t"(x) : "st" );
  return r;
}

inline vxl_int_32 RoundHalfIntegerToEven_32(float x)
{
  vxl_int_32 r;
  __asm__ __volatile__( "fistpl %0" : "=m"(r) : "t"(x) : "st" );
  return r;
}

#elif VC_USE_ASM_32IMPL // msvc asm implementation

inline vxl_int_32 RoundHalfIntegerToEven_32(double x)
{
  vxl_int_32 r;
  __asm {
    fld x
    fistp r
  }
  return r;
}

inline vxl_int_32 RoundHalfIntegerToEven_32(float x)
{
  vxl_int_32 r;
  __asm {
    fld x
    fistp r
  }
  return r;
}

#else // Base implementation

inline vxl_int_32 RoundHalfIntegerToEven_32(double x) { return RoundHalfIntegerToEven_base<vxl_int_32,double>(x); }
inline vxl_int_32 RoundHalfIntegerToEven_32(float  x) { return RoundHalfIntegerToEven_base<vxl_int_32,float>(x); }

#endif


#if USE_SSE2_32IMPL || GCC_USE_ASM_32IMPL || VC_USE_ASM_32IMPL

inline vxl_int_32 RoundHalfIntegerUp_32(double x) { return RoundHalfIntegerToEven_32(2*x+0.5)>>1; }
inline vxl_int_32 RoundHalfIntegerUp_32(float  x) { return RoundHalfIntegerToEven_32(2*x+0.5f)>>1; }


inline vxl_int_32 Floor_32(double x) { return RoundHalfIntegerToEven_32(2*x-0.5)>>1; }
inline vxl_int_32 Floor_32(float  x) { return RoundHalfIntegerToEven_32(2*x-0.5f)>>1; }

inline vxl_int_32 Ceil_32(double x) { return -(RoundHalfIntegerToEven_32(-0.5-2*x)>>1); }
inline vxl_int_32 Ceil_32(float  x) { return -(RoundHalfIntegerToEven_32(-0.5f-2*x)>>1); }

#else // Base implementation

inline vxl_int_32 RoundHalfIntegerUp_32(double x) { return RoundHalfIntegerUp_base<vxl_int_32,double>(x); }
inline vxl_int_32 RoundHalfIntegerUp_32(float  x) { return RoundHalfIntegerUp_base<vxl_int_32,float>(x); }

inline vxl_int_32 Floor_32(double x) { return Floor_base<vxl_int_32,double>(x); }
inline vxl_int_32 Floor_32(float  x) { return Floor_base<vxl_int_32,float>(x); }

inline vxl_int_32 Ceil_32(double x) { return Ceil_base<vxl_int_32,double>(x); }
inline vxl_int_32 Ceil_32(float  x) { return Ceil_base<vxl_int_32,float>(x); }

#endif // USE_SSE2_32IMPL || GCC_USE_ASM_32IMPL || VC_USE_ASM_32IMPL


////////////////////////////////////////
// 64 bits versions
#if VXL_HAS_INT_64


// Figure out when the fast implementation can be used

#if USE_SSE2_64IMPL // sse2 implementation

inline vxl_int_64 RoundHalfIntegerToEven_64(double x) { return _mm_cvtsd_si64(_mm_set_sd(x)); }
inline vxl_int_64 RoundHalfIntegerToEven_64(float  x) { return _mm_cvtss_si64(_mm_set_ss(x)); }

#elif GCC_USE_ASM_64IMPL // gcc asm implementation

inline vxl_int_64 RoundHalfIntegerToEven_64(double x)
{
  vxl_int_64 r;
  __asm__ __volatile__( "fistpll %0" : "=m"(r) : "t"(x) : "st" );
  return r;
}

inline vxl_int_64 RoundHalfIntegerToEven_64(float x)
{
  vxl_int_64 r;
  __asm__ __volatile__( "fistpll %0" : "=m"(r) : "t"(x) : "st" );
  return r;
}

#elif VC_USE_ASM_64IMPL // msvc asm implementation

inline vxl_int_64 RoundHalfIntegerToEven_64(double x)
{
  vxl_int_64 r;
  __asm 
    {
    fld x
    fistp r
    }
  return r;
}

inline vxl_int_64 RoundHalfIntegerToEven_64(float x)
{
  vxl_int_64 r;
  __asm 
    {
    fld x
    fistp r
    }
  return r;
}

#else // Base implementation

inline vxl_int_64 RoundHalfIntegerToEven_64(double x) { return RoundHalfIntegerToEven_base<vxl_int_64,double>(x); }
inline vxl_int_64 RoundHalfIntegerToEven_64(float  x) { return RoundHalfIntegerToEven_base<vxl_int_64,float>(x); }

#endif


#if USE_SSE2_64IMPL || GCC_USE_ASM_64IMPL || VC_USE_ASM_64IMPL

inline vxl_int_64 RoundHalfIntegerUp_64(double x) { return RoundHalfIntegerToEven_64(2*x+0.5)>>1; }
inline vxl_int_64 RoundHalfIntegerUp_64(float  x) { return RoundHalfIntegerToEven_64(2*x+0.5f)>>1; }

inline vxl_int_64 Floor_64(double x) { return RoundHalfIntegerToEven_64(2*x-0.5)>>1; }
inline vxl_int_64 Floor_64(float  x) { return RoundHalfIntegerToEven_64(2*x-0.5f)>>1; }


inline vxl_int_64 Ceil_64(double x) { return -(RoundHalfIntegerToEven_64(-0.5-2*x)>>1); }
inline vxl_int_64 Ceil_64(float  x) { return -(RoundHalfIntegerToEven_64(-0.5f-2*x)>>1); }


#else // Base implementation

inline vxl_int_64 RoundHalfIntegerUp_64(double x) { return RoundHalfIntegerUp_base<vxl_int_64,double>(x); }
inline vxl_int_64 RoundHalfIntegerUp_64(float  x) { return RoundHalfIntegerUp_base<vxl_int_64,float>(x); }

inline vxl_int_64 Floor_64(double x) { return Floor_base<vxl_int_64,double>(x); }
inline vxl_int_64 Floor_64(float  x) { return Floor_base<vxl_int_64,float>(x); }

inline vxl_int_64 Ceil_64(double x) { return Ceil_base<vxl_int_64,double>(x); }
inline vxl_int_64 Ceil_64(float  x) { return Ceil_base<vxl_int_64,float>(x); }

#endif // USE_SSE2_64IMPL || GCC_USE_ASM_64IMPL || VC_USE_ASM_64IMPL

#endif // VXL_HAS_INT_64


} // end namespace Detail
} // end namespace Math
} // end namespace itk


#undef USE_SSE2_32IMPL
#undef GCC_USE_ASM_32IMPL
#undef VC_USE_ASM_32IMPL

#undef USE_SSE2_64IMPL
#undef GCC_USE_ASM_64IMPL
#undef VC_USE_ASM_64IMPL

#endif // end of itkMathDetail.h