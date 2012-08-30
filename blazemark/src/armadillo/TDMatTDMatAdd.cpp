//=================================================================================================
/*!
//  \file src/armadillo/TDMatTDMatAdd.cpp
//  \brief Source file for the Armadillo transpose dense matrix/transpose dense matrix addition kernel
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iostream>
#include <armadillo>
#include <blaze/util/Timing.h>
#include <blazemark/armadillo/init/Mat.h>
#include <blazemark/armadillo/TDMatTDMatAdd.h>
#include <blazemark/system/Config.h>


namespace blazemark {

namespace armadillo {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Armadillo transpose dense matrix/transpose dense matrix addition kernel.
//
// \param N The number of rows and columns of the matrices.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the transpose dense matrix/transpose dense matrix addition
// by means of the Armadillo functionality.
*/
double tdmattdmatadd( size_t N, size_t steps )
{
   using ::blazemark::element_t;

   ::blaze::setSeed( seed );

   ::arma::Mat<element_t> A( N, N ), B( N, N ), C( N, N );
   ::blaze::timing::WcTimer timer;

   init( A );
   init( B );

   C = A + B;

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         C = A + B;
      }
      timer.end();

      if( C.n_rows != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " Armadillo kernel 'tdmattdmatadd': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace armadillo

} // namespace blazemark
