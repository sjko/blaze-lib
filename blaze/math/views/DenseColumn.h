//=================================================================================================
/*!
//  \file blaze/math/views/DenseColumn.h
//  \brief Header file for the DenseColumn class template
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_VIEWS_DENSECOLUMN_H_
#define _BLAZE_MATH_VIEWS_DENSECOLUMN_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iterator>
#include <stdexcept>
#include <blaze/math/constraints/Computation.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/DenseVector.h>
#include <blaze/math/constraints/RequiresEvaluation.h>
#include <blaze/math/constraints/Restricted.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/constraints/Symmetric.h>
#include <blaze/math/constraints/TransExpr.h>
#include <blaze/math/constraints/TransposeFlag.h>
#include <blaze/math/expressions/Column.h>
#include <blaze/math/expressions/DenseVector.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/shims/Clear.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/traits/ColumnTrait.h>
#include <blaze/math/traits/DerestrictTrait.h>
#include <blaze/math/traits/DivTrait.h>
#include <blaze/math/traits/SubvectorTrait.h>
#include <blaze/math/typetraits/IsColumnMajorMatrix.h>
#include <blaze/math/typetraits/IsDiagonal.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsLower.h>
#include <blaze/math/typetraits/IsRestricted.h>
#include <blaze/math/typetraits/IsSparseVector.h>
#include <blaze/math/typetraits/IsSymmetric.h>
#include <blaze/math/typetraits/IsUpper.h>
#include <blaze/math/typetraits/RequiresEvaluation.h>
#include <blaze/system/CacheSize.h>
#include <blaze/system/Inline.h>
#include <blaze/system/Streaming.h>
#include <blaze/system/Thresholds.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Pointer.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/constraints/Vectorizable.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/logging/FunctionTrace.h>
#include <blaze/util/mpl/And.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/mpl/Or.h>
#include <blaze/util/Null.h>
#include <blaze/util/StaticAssert.h>
#include <blaze/util/Template.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsConst.h>
#include <blaze/util/typetraits/IsFloatingPoint.h>
#include <blaze/util/typetraits/IsNumeric.h>
#include <blaze/util/typetraits/IsReference.h>
#include <blaze/util/typetraits/IsSame.h>
#include <blaze/util/typetraits/RemoveReference.h>
#include <blaze/util/Unused.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup dense_column DenseColumn
// \ingroup views
*/
/*!\brief Reference to a specific column of a dense matrix.
// \ingroup dense_column
//
// The DenseColumn template represents a reference to a specific column of a dense matrix
// primitive. The type of the dense matrix is specified via the first template parameter:

   \code
   template< typename MT, bool SO >
   class DenseColumn;
   \endcode

//  - MT: specifies the type of the dense matrix primitive. DenseColumn can be used with every
//        dense matrix primitive, but does not work with any matrix expression type.
//  - SO: specifies the storage order (blaze::rowMajor, blaze::columnMajor) of the dense matrix.
//        This template parameter doesn't have to be explicitly defined, but is automatically
//        derived from the first template parameter.
//  - SF: specifies whether the given matrix is a symmetric matrix or not. Also this parameter
//        doesn't have to be explicitly defined, but is automatically derived from the first
//        template parameter.
//
//
// \n \section dense_column_setup Setup of Dense Columns
//
// A reference to a dense column can be created very conveniently via the \c column() function.
// This reference can be treated as any other column vector, i.e. it can be assigned to, it can
// be copied from, and it can be used in arithmetic operations. The reference can also be used
// on both sides of an assignment: The column can either be used as an alias to grant write access
// to a specific column of a matrix primitive on the left-hand side of an assignment or to grant
// read-access to a specific column of a matrix primitive or expression on the right-hand side of
// an assignment. The following example demonstrates this in detail:

   \code
   typedef blaze::DynamicVector<double,blaze::columnVector>     DenseVectorType;
   typedef blaze::CompressedVector<double,blaze::columnVector>  SparseVectorType;
   typedef blaze::DynamicMatrix<double,blaze::columnMajor>      DenseMatrixType;

   DenseVectorType  x;
   SparseVectorType y;
   DenseMatrixType  A, B;
   // ... Resizing and initialization

   // Setting the 2nd column of matrix A to x
   blaze::DenseColumn<DenseMatrixType> col2 = column( A, 2UL );
   col2 = x;

   // Setting the 3rd column of matrix B to y
   column( B, 3UL ) = y;

   // Setting x to the 1st column of matrix B
   x = column( B, 1UL );

   // Setting y to the 4th column of the result of the matrix multiplication
   y = column( A * B, 4UL );
   \endcode

// \n \section dense_column_element_access Element access
//
// A dense column can be used like any other column vector. For instance, the elements of the
// dense column can be directly accessed with the subscript operator.

   \code
   typedef blaze::DynamicMatrix<double,blaze::columnMajor>  MatrixType;
   MatrixType A;
   // ... Resizing and initialization

   // Creating a view on the 4th column of matrix A
   blaze::DenseColumn<MatrixType> col4 = column( A, 4UL );

   // Setting the 1st element of the dense column, which corresponds
   // to the 1st element in the 4th column of matrix A
   col4[1] = 2.0;
   \endcode

// The numbering of the column elements is

                             \f[\left(\begin{array}{*{5}{c}}
                             0 & 1 & 2 & \cdots & N-1 \\
                             \end{array}\right),\f]

// where N is the number of rows of the referenced matrix. Alternatively, the elements of a
// column can be traversed via iterators. Just as with vectors, in case of non-const rows,
// \c begin() and \c end() return an Iterator, which allows a manipulation of the non-zero
// values, in case of constant columns a ConstIterator is returned:

   \code
   typedef blaze::DynamicMatrix<int,blaze::columnMajor>  MatrixType;
   typedef blaze::DenseColumn<MatrixType>                ColumnType;

   MatrixType A( 128UL, 256UL );
   // ... Resizing and initialization

   // Creating a reference to the 31st column of matrix A
   ColumnType col31 = column( A, 31UL );

   for( ColumnType::Iterator it=col31.begin(); it!=col31.end(); ++it ) {
      *it = ...;  // OK; Write access to the dense column value
      ... = *it;  // OK: Read access to the dense column value.
   }

   for( ColumnType::ConstIterator it=col31.begin(); it!=col31.end(); ++it ) {
      *it = ...;  // Compilation error: Assignment to the value via a ConstIterator is invalid.
      ... = *it;  // OK: Read access to the dense column value.
   }
   \endcode

// \n \section dense_column_common_operations Common Operations
//
// The current number of column elements can be obtained via the \c size() function, the current
// capacity via the \c capacity() function, and the number of non-zero elements via the
// \c nonZeros() function. However, since columns are references to specific columns of a matrix,
// several operations are not possible on views, such as resizing and swapping:

   \code
   typedef blaze::DynamicMatrix<int,blaze::columnMajor>  MatrixType;
   typedef blaze::DenseColumn<MatrixType>                ColumnType;

   MatrixType A( 42UL, 42UL );
   // ... Resizing and initialization

   // Creating a reference to the 2nd column of matrix A
   ColumnType col2 = column( A, 2UL );

   col2.size();          // Returns the number of elements in the column
   col2.capacity();      // Returns the capacity of the column
   col2.nonZeros();      // Returns the number of non-zero elements contained in the column

   col2.resize( 84UL );  // Compilation error: Cannot resize a single column of a matrix

   ColumnType col3 = column( A, 3UL );
   swap( col2, col3 );   // Compilation error: Swap operation not allowed
   \endcode

// \n \section dense_column_arithmetic_operations Arithmetic Operations
//
// The following example gives an impression of the use of DenseColumn within arithmetic operations.
// All operations (addition, subtraction, multiplication, scaling, ...) can be performed on all
// possible combinations of dense and sparse vectors with fitting element types:

   \code
   blaze::DynamicVector<double,blaze::columnVector> a( 2UL, 2.0 ), b;
   blaze::CompressedVector<double,blaze::columnVector> c( 2UL );
   c[1] = 3.0;

   typedef blaze::DynamicMatrix<double,blaze::columnMajor>  MatrixType;
   MatrixType A( 2UL, 4UL );  // Non-initialized 2x4 matrix

   typedef blaze::DenseColumn<DenseMatrix>  RowType;
   RowType col0( column( A, 0UL ) );  // Reference to the 0th column of A

   col0[0] = 0.0;           // Manual initialization of the 0th column of A
   col0[1] = 0.0;
   column( A, 1UL ) = 1.0;  // Homogeneous initialization of the 1st column of A
   column( A, 2UL ) = a;    // Dense vector initialization of the 2nd column of A
   column( A, 3UL ) = c;    // Sparse vector initialization of the 3rd column of A

   b = col0 + a;                 // Dense vector/dense vector addition
   b = c + column( A, 1UL );     // Sparse vector/dense vector addition
   b = col0 * column( A, 2UL );  // Component-wise vector multiplication

   column( A, 1UL ) *= 2.0;     // In-place scaling of the 1st column
   b = column( A, 1UL ) * 2.0;  // Scaling of the 1st column
   b = 2.0 * column( A, 1UL );  // Scaling of the 1st column

   column( A, 2UL ) += a;                 // Addition assignment
   column( A, 2UL ) -= c;                 // Subtraction assignment
   column( A, 2UL ) *= column( A, 0UL );  // Multiplication assignment

   double scalar = trans( c ) * column( A, 1UL );  // Scalar/dot/inner product between two vectors

   A = column( A, 1UL ) * trans( c );  // Outer product between two vectors
   \endcode

// \n \section dense_column_on_row_major_matrix Dense Column on a Row-Major Matrix
//
// It is especially noteworthy that column views can be created for both row-major and column-major
// matrices. Whereas the interface of a row-major matrix only allows to traverse a row directly
// and the interface of a column-major matrix only allows to traverse a column, via views it is
// also possible to traverse a column of a row-major matrix. For instance:

   \code
   typedef blaze::DynamicMatrix<int,blaze::rowMajor>  MatrixType;
   typedef blaze::DenseColumn<MatrixType>             ColumnType;

   MatrixType A( 64UL, 32UL );
   // ... Resizing and initialization

   // Creating a reference to the 1st column of a column-major matrix A
   ColumnType col1 = column( A, 1UL );

   for( ColumnType::Iterator it=col1.begin(); it!=col1.end(); ++it ) {
      // ...
   }
   \endcode

// However, please note that creating a column view on a matrix stored in a row-major fashion
// can result in a considerable performance decrease in comparison to a column view on a matrix
// with column-major storage format. This is due to the non-contiguous storage of the matrix
// elements. Therefore care has to be taken in the choice of the most suitable storage order:

   \code
   // Setup of two row-major matrices
   blaze::DynamicMatrix<double,blaze::rowMajor> A( 128UL, 128UL );
   blaze::DynamicMatrix<double,blaze::rowMajor> B( 128UL, 128UL );
   // ... Resizing and initialization

   // The computation of the 15th column of the multiplication between A and B ...
   blaze::DynamicVector<double,blaze::columnVector> x = column( A * B, 15UL );

   // ... is essentially the same as the following computation, which multiplies
   // A with the 15th column of the row-major matrix B.
   blaze::DynamicVector<double,blaze::rowVector> x = A * column( B, 15UL );
   \endcode

// Although Blaze performs the resulting matrix/vector multiplication as efficiently as possible
// using a column-major storage order for matrix A would result in a more efficient evaluation.
*/
template< typename MT                               // Type of the dense matrix
        , bool SO = IsColumnMajorMatrix<MT>::value  // Storage order
        , bool SF = IsSymmetric<MT>::value >        // Symmetry flag
class DenseColumn : public DenseVector< DenseColumn<MT,SO,SF>, false >
                  , private Column
{
 private:
   //**Type definitions****************************************************************************
   //! Composite data type of the dense matrix expression.
   typedef typename If< IsExpression<MT>, MT, MT& >::Type  Operand;

   //! Intrinsic trait for the column element type.
   typedef IntrinsicTrait<typename MT::ElementType>  IT;
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the non-const reference and iterator types.
   /*! The \a useConst compile time constant expression represents a compilation switch for
       the non-const reference and iterator types. In case the given dense matrix of type
       \a MT is const qualified, \a useConst will be set to 1 and the dense column will
       return references and iterators to const. Otherwise \a useConst will be set to 0
       and the dense column will offer write access to the dense matrix elements both via
       the subscript operator and iterators. */
   enum { useConst = IsConst<MT>::value };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DenseColumn<MT,SO,SF>               This;           //!< Type of this DenseColumn instance.
   typedef typename ColumnTrait<MT>::Type      ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType            ElementType;    //!< Type of the column elements.
   typedef typename IT::Type                   IntrinsicType;  //!< Intrinsic type of the column elements.
   typedef typename MT::ReturnType             ReturnType;     //!< Return type for expression template evaluations
   typedef const DenseColumn&                  CompositeType;  //!< Data type for composite expression templates.

   //! Reference to a constant column value.
   typedef typename MT::ConstReference  ConstReference;

   //! Reference to a non-constant column value.
   typedef typename IfTrue< useConst, ConstReference, typename MT::Reference >::Type  Reference;

   //! Pointer to a constant row value.
   typedef const ElementType*  ConstPointer;

   //! Pointer to a non-constant row value.
   typedef typename IfTrue< useConst, ConstPointer, ElementType* >::Type  Pointer;

   //! Iterator over constant elements.
   typedef typename MT::ConstIterator  ConstIterator;

   //! Iterator over non-constant elements.
   typedef typename IfTrue< useConst, ConstIterator, typename MT::Iterator >::Type  Iterator;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = MT::vectorizable };

   //! Compilation switch for the expression template assignment strategy.
   enum { smpAssignable = MT::smpAssignable };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline DenseColumn( MT& matrix, size_t index );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator[]( size_t index );
   inline ConstReference operator[]( size_t index ) const;
   inline Pointer        data  ();
   inline ConstPointer   data  () const;
   inline Iterator       begin ();
   inline ConstIterator  begin () const;
   inline ConstIterator  cbegin() const;
   inline Iterator       end   ();
   inline ConstIterator  end   () const;
   inline ConstIterator  cend  () const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
   inline DenseColumn& operator=( const ElementType& rhs );
   inline DenseColumn& operator=( const DenseColumn& rhs );

   template< typename VT > inline DenseColumn& operator= ( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator+=( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator-=( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator*=( const Vector<VT,false>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseColumn >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseColumn >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t       size() const;
                              inline size_t       capacity() const;
                              inline size_t       nonZeros() const;
                              inline void         reset();
   template< typename Other > inline DenseColumn& scale( const Other& scalar );
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAddAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value &&
                     IntrinsicTrait<ElementType>::addition };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedSubAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value &&
                     IntrinsicTrait<ElementType>::subtraction };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedMultAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value &&
                     IntrinsicTrait<ElementType>::multiplication };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other >
   inline bool canAlias( const Other* alias ) const;

   template< typename MT2, bool SO2, bool SF2 >
   inline bool canAlias( const DenseColumn<MT2,SO2,SF2>* alias ) const;

   template< typename Other >
   inline bool isAliased( const Other* alias ) const;

   template< typename MT2, bool SO2, bool SF2 >
   inline bool isAliased( const DenseColumn<MT2,SO2,SF2>* alias ) const;

   inline bool isAligned   () const;
   inline bool canSMPAssign() const;

   BLAZE_ALWAYS_INLINE IntrinsicType load ( size_t index ) const;
   BLAZE_ALWAYS_INLINE IntrinsicType loadu( size_t index ) const;

   BLAZE_ALWAYS_INLINE void store ( size_t index, const IntrinsicType& value );
   BLAZE_ALWAYS_INLINE void storeu( size_t index, const IntrinsicType& value );
   BLAZE_ALWAYS_INLINE void stream( size_t index, const IntrinsicType& value );

   template< typename VT >
   inline typename DisableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void assign( const SparseVector<VT,false>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void addAssign( const SparseVector<VT,false>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void subAssign( const SparseVector<VT,false>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void multAssign( const SparseVector<VT,false>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< Not< IsRestricted<MT2> >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const Vector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Operand      matrix_;  //!< The dense matrix containing the column.
   const size_t col_;     //!< The index of the column in the matrix.
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename MT2, bool SO2, bool SF2 > friend class DenseColumn;

   template< typename MT2, bool SO2, bool SF2 >
   friend bool isSame( const DenseColumn<MT2,SO2,SF2>& a, const DenseColumn<MT2,SO2,SF2>& b );

   template< typename MT2, bool SO2, bool SF2 >
   friend typename DerestrictTrait< DenseColumn<MT2,SO2,SF2> >::Type
      derestrict( DenseColumn<MT2,SO2,SF2>& dm );
   /*! \endcond */
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE       ( MT );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE    ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_TRANSEXPR_TYPE      ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE        ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE      ( MT );
   BLAZE_STATIC_ASSERT( !IsRestricted<MT>::value || IsLower<MT>::value || IsUpper<MT>::value );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The constructor for DenseColumn.
//
// \param matrix The matrix containing the column.
// \param index The index of the column.
// \exception std::invalid_argument Invalid column access index.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline DenseColumn<MT,SO,SF>::DenseColumn( MT& matrix, size_t index )
   : matrix_( matrix )  // The dense matrix containing the column
   , col_   ( index  )  // The index of the column in the matrix
{
   if( matrix_.columns() <= index )
      throw std::invalid_argument( "Invalid column access index" );
}
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Subscript operator for the direct access to the column elements.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return Reference to the accessed value.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::Reference DenseColumn<MT,SO,SF>::operator[]( size_t index )
{
   BLAZE_USER_ASSERT( index < size(), "Invalid column access index" );
   return matrix_(index,col_);
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subscript operator for the direct access to the column elements.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return Reference to the accessed value.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::ConstReference
   DenseColumn<MT,SO,SF>::operator[]( size_t index ) const
{
   BLAZE_USER_ASSERT( index < size(), "Invalid column access index" );
   return matrix_(index,col_);
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the column elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::Pointer DenseColumn<MT,SO,SF>::data()
{
   return matrix_.data( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the column elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::ConstPointer DenseColumn<MT,SO,SF>::data() const
{
   return matrix_.data( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::Iterator DenseColumn<MT,SO,SF>::begin()
{
   return matrix_.begin( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::ConstIterator DenseColumn<MT,SO,SF>::begin() const
{
   return matrix_.cbegin( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::ConstIterator DenseColumn<MT,SO,SF>::cbegin() const
{
   return matrix_.cbegin( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::Iterator DenseColumn<MT,SO,SF>::end()
{
   return matrix_.end( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::ConstIterator DenseColumn<MT,SO,SF>::end() const
{
   return matrix_.cend( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DenseColumn<MT,SO,SF>::ConstIterator DenseColumn<MT,SO,SF>::cend() const
{
   return matrix_.cend( col_ );
}
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Homogenous assignment to all column elements.
//
// \param rhs Scalar value to be assigned to all column elements.
// \return Reference to the assigned column.
//
// This function homogeneously assigns the given value to all elements of the column. Note that in
// case the underlying dense matrix is a lower/upper matrix only lower/upper and diagonal elements
// of the underlying matrix are modified.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline DenseColumn<MT,SO,SF>& DenseColumn<MT,SO,SF>::operator=( const ElementType& rhs )
{
   const size_t ibegin( ( IsLower<MT>::value )?( col_ ):( 0UL ) );
   const size_t iend  ( ( IsUpper<MT>::value )?( col_+1UL ):( size() ) );

   for( size_t i=ibegin; i<iend; ++i )
      matrix_(i,col_) = rhs;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Copy assignment operator for DenseColumn.
//
// \param rhs Dense column to be copied.
// \return Reference to the assigned column.
// \exception std::invalid_argument Column sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two columns don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline DenseColumn<MT,SO,SF>& DenseColumn<MT,SO,SF>::operator=( const DenseColumn& rhs )
{
   if( &rhs == this ) return *this;

   if( size() != rhs.size() )
      throw std::invalid_argument( "Column sizes do not match" );

   if( !preservesInvariant( matrix_, rhs ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   smpAssign( left, rhs );

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different vectors.
//
// \param rhs Vector to be assigned.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,SO,SF>& DenseColumn<MT,SO,SF>::operator=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   if( IsSparseVector<VT>::value )
      reset();

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpAssign( left, tmp );
   }
   else {
      smpAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Addition assignment operator for the addition of a vector (\f$ \vec{a}+=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be added to the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,SO,SF>& DenseColumn<MT,SO,SF>::operator+=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpAddAssign( left, tmp );
   }
   else {
      smpAddAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subtraction assignment operator for the subtraction of a vector (\f$ \vec{a}-=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be subtracted from the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,SO,SF>& DenseColumn<MT,SO,SF>::operator-=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpSubAssign( left, tmp );
   }
   else {
      smpSubAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication of a vector
//        (\f$ \vec{a}*=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be multiplied with the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,SO,SF>& DenseColumn<MT,SO,SF>::operator*=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( (~rhs).canAlias( &matrix_ ) || IsSparseVector<VT>::value ) {
      const ResultType tmp( *this * (~rhs) );
      smpAssign( left, tmp );
   }
   else {
      smpMultAssign( left, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication between a vector and
//        a scalar value (\f$ \vec{a}*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the vector.
*/
template< typename MT       // Type of the dense matrix
        , bool SO           // Storage order
        , bool SF >         // Symmetry flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseColumn<MT,SO,SF> >::Type&
   DenseColumn<MT,SO,SF>::operator*=( Other rhs )
{
   return operator=( (*this) * rhs );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a vector by a scalar value
//        (\f$ \vec{a}/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the vector.
//
// \b Note: A division by zero is only checked by an user assert.
*/
template< typename MT       // Type of the dense matrix
        , bool SO           // Storage order
        , bool SF >         // Symmetry flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseColumn<MT,SO,SF> >::Type&
   DenseColumn<MT,SO,SF>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   return operator=( (*this) / rhs );
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns the current size/dimension of the column.
//
// \return The size of the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline size_t DenseColumn<MT,SO,SF>::size() const
{
   return matrix_.rows();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the maximum capacity of the dense column.
//
// \return The capacity of the dense column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline size_t DenseColumn<MT,SO,SF>::capacity() const
{
   return matrix_.capacity( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the column.
//
// \return The number of non-zero elements in the column.
//
// Note that the number of non-zero elements is always less than or equal to the current number
// of rows of the matrix containing the column.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline size_t DenseColumn<MT,SO,SF>::nonZeros() const
{
   return matrix_.nonZeros( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline void DenseColumn<MT,SO,SF>::reset()
{
   matrix_.reset( col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the column by the scalar value \a scalar (\f$ \vec{a}=\vec{b}*s \f$).
//
// \param scalar The scalar value for the column scaling.
// \return Reference to the dense column.
*/
template< typename MT       // Type of the dense matrix
        , bool SO           // Storage order
        , bool SF >         // Symmetry flag
template< typename Other >  // Data type of the scalar value
inline DenseColumn<MT,SO,SF>& DenseColumn<MT,SO,SF>::scale( const Other& scalar )
{
   for( size_t i=0UL; i<size(); ++i ) {
      matrix_(i,col_) *= scalar;
   }
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking for possible invariant violations of the underlying matrix.
//
// \param lhs The matrix to be assigned to.
// \param rhs The vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying matrix of type \a MT would be
// violated by an assignment of the given vector \a rhs. In case the matrix would be preserved,
// the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side vector
inline typename EnableIf< Not< IsRestricted<MT2> >, bool >::Type
   DenseColumn<MT,SO,SF>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const Vector<VT,false>& rhs )
{
   UNUSED_PARAMETER( lhs, rhs );

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking for possible invariant violations of the underlying lower triangular matrix.
//
// \param lhs The lower matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying lower triangular matrix of type
// \a MT would be violated by an assignment of the given dense vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,SO,SF>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=0UL; i<col_; ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking for possible invariant violations of the underlying lower triangular matrix.
//
// \param lhs The lower matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying lower triangular matrix of type
// \a MT would be violated by an assignment of the given sparse vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,SO,SF>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).begin(); element!=(~rhs).lowerBound( col_ ); ++element ) {
      if( !isDefault( element->value() ) )
         return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking for possible invariant violations of the underlying upper triangular matrix.
//
// \param lhs The upper matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying upper triangular matrix of type
// \a MT would be violated by an assignment of the given dense vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,SO,SF>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=col_+1UL; i<size(); ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking for possible invariant violations of the underlying upper triangular matrix.
//
// \param lhs The upper matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying upper triangular matrix of type
// \a MT would be violated by an assignment of the given sparse vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,SO,SF>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).lowerBound( col_+1UL ); element!=(~rhs).end(); ++element ) {
      if( !isDefault( element->value() ) )
         return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking for possible invariant violations of the underlying diagonal matrix.
//
// \param lhs The diagonal matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying diagonal matrix of type \a MT would
// be violated by an assignment of the given dense vector \a rhs. In case the matrix would be
// preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
   DenseColumn<MT,SO,SF>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=0UL; i<col_; ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   for( size_t i=col_+1UL; i<size(); ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking for possible invariant violations of the underlying diagonal matrix.
//
// \param lhs The diagonal matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying diagonal matrix of type \a MT would
// be violated by an assignment of the given sparse vector \a rhs. In case the matrix would be
// preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
   DenseColumn<MT,SO,SF>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element ) {
      if( element->index() != col_ && !isDefault( element->value() ) )
         return false;
   }

   return true;
}
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether the dense column can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address can alias with the dense column. In
// contrast to the isAliased() function this function is allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT       // Type of the dense matrix
        , bool SO           // Storage order
        , bool SF >         // Symmetry flag
template< typename Other >  // Data type of the foreign expression
inline bool DenseColumn<MT,SO,SF>::canAlias( const Other* alias ) const
{
   return matrix_.isAliased( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the dense column can alias with the given dense column \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address can alias with the dense column. In
// contrast to the isAliased() function this function is allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT   // Type of the dense matrix
        , bool SO       // Storage order
        , bool SF >     // Symmetry flag
template< typename MT2  // Data type of the foreign dense column
        , bool SO2      // Storage order of the foreign dense column
        , bool SF2 >    // Symmetry flag of the foreign dense column
inline bool DenseColumn<MT,SO,SF>::canAlias( const DenseColumn<MT2,SO2,SF2>* alias ) const
{
   return matrix_.isAliased( alias->matrix_ ) && ( col_ == alias->col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the dense column is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address is aliased with the dense column. In
// contrast to the canAlias() function this function is not allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT       // Type of the dense matrix
        , bool SO           // Storage order
        , bool SF >         // Symmetry flag
template< typename Other >  // Data type of the foreign expression
inline bool DenseColumn<MT,SO,SF>::isAliased( const Other* alias ) const
{
   return matrix_.isAliased( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the dense column is aliased with the given dense column \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address is aliased with the dense column. In
// contrast to the canAlias() function this function is not allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT   // Type of the dense matrix
        , bool SO       // Storage order
        , bool SF >     // Symmetry flag
template< typename MT2  // Data type of the foreign dense column
        , bool SO2      // Storage order of the foreign dense column
        , bool SF2 >    // Symmetry flag of the foreign dense column
inline bool DenseColumn<MT,SO,SF>::isAliased( const DenseColumn<MT2,SO2,SF2>* alias ) const
{
   return matrix_.isAliased( &alias->matrix_ ) && ( col_ == alias->col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the dense column is properly aligned in memory.
//
// \return \a true in case the dense column is aligned, \a false if not.
//
// This function returns whether the dense column is guaranteed to be properly aligned in memory,
// i.e. whether the beginning and the end of the dense column are guaranteed to conform to the
// alignment restrictions of the element type \a Type.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline bool DenseColumn<MT,SO,SF>::isAligned() const
{
   return matrix_.isAligned();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the dense column can be used in SMP assignments.
//
// \return \a true in case the dense column can be used in SMP assignments, \a false if not.
//
// This function returns whether the dense column can be used in SMP assignments. In contrast
// to the \a smpAssignable member enumeration, which is based solely on compile time information,
// this function additionally provides runtime information (as for instance the current size of
// the dense column).
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline bool DenseColumn<MT,SO,SF>::canSMPAssign() const
{
   return ( size() > SMP_DVECASSIGN_THRESHOLD );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned load of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return The loaded intrinsic element.
//
// This function performs an aligned load of a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
BLAZE_ALWAYS_INLINE typename DenseColumn<MT,SO,SF>::IntrinsicType
   DenseColumn<MT,SO,SF>::load( size_t index ) const
{
   return matrix_.load( index, col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned load of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return The loaded intrinsic element.
//
// This function performs an unaligned load of a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
BLAZE_ALWAYS_INLINE typename DenseColumn<MT,SO,SF>::IntrinsicType
   DenseColumn<MT,SO,SF>::loadu( size_t index ) const
{
   return matrix_.loadu( index, col_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned store of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned store a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be
// called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
BLAZE_ALWAYS_INLINE void DenseColumn<MT,SO,SF>::store( size_t index, const IntrinsicType& value )
{
   matrix_.store( index, col_, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned store of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an unaligned store a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be
// called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
BLAZE_ALWAYS_INLINE void DenseColumn<MT,SO,SF>::storeu( size_t index, const IntrinsicType& value )
{
   matrix_.storeu( index, col_, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned, non-temporal store a specific intrinsic element of the
// dense column. The index must be smaller than the number of matrix rows. This function must
// \b NOT be called explicitly! It is used internally for the performance optimized evaluation
// of expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
BLAZE_ALWAYS_INLINE void DenseColumn<MT,SO,SF>::stream( size_t index, const IntrinsicType& value )
{
   matrix_.stream( index, col_, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::assign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) = (~rhs)[i    ];
      matrix_(i+1UL,col_) = (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) = (~rhs)[ipos];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::assign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t rows( size() );

   if( useStreaming && rows > ( cacheSize/( sizeof(ElementType) * 3UL ) ) && !(~rhs).isAliased( this ) )
   {
      for( size_t i=0UL; i<rows; i+=IT::size ) {
         matrix_.stream( i, col_, (~rhs).load(i) );
      }
   }
   else
   {
      const size_t ipos( rows & size_t(-IT::size*4) );
      BLAZE_INTERNAL_ASSERT( ( rows - ( rows % (IT::size*4UL) ) ) == ipos, "Invalid end calculation" );

      typename VT::ConstIterator it( (~rhs).begin() );
      for( size_t i=0UL; i<ipos; i+=IT::size*4UL ) {
         matrix_.store( i             , col_, it.load() ); it+=IT::size;
         matrix_.store( i+IT::size    , col_, it.load() ); it+=IT::size;
         matrix_.store( i+IT::size*2UL, col_, it.load() ); it+=IT::size;
         matrix_.store( i+IT::size*3UL, col_, it.load() ); it+=IT::size;
      }
      for( size_t i=ipos; i<rows; i+=IT::size, it+=IT::size ) {
         matrix_.store( i, col_, it.load() );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,SO,SF>::assign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::addAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) += (~rhs)[i    ];
      matrix_(i+1UL,col_) += (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) += (~rhs)[ipos];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::addAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t rows( size() );

   const size_t ipos( rows & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( rows - ( rows % (IT::size*4UL) ) ) == ipos, "Invalid end calculation" );

   typename VT::ConstIterator it( (~rhs).begin() );
   for( size_t i=0UL; i<ipos; i+=IT::size*4UL ) {
      matrix_.store( i             , col_, matrix_.load(i             ,col_) + it.load() ); it += IT::size;
      matrix_.store( i+IT::size    , col_, matrix_.load(i+IT::size    ,col_) + it.load() ); it += IT::size;
      matrix_.store( i+IT::size*2UL, col_, matrix_.load(i+IT::size*2UL,col_) + it.load() ); it += IT::size;
      matrix_.store( i+IT::size*3UL, col_, matrix_.load(i+IT::size*3UL,col_) + it.load() ); it += IT::size;
   }
   for( size_t i=ipos; i<rows; i+=IT::size, it+=IT::size ) {
      matrix_.store( i, col_, matrix_.load(i,col_) + it.load() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,SO,SF>::addAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::subAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) -= (~rhs)[i    ];
      matrix_(i+1UL,col_) -= (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) -= (~rhs)[ipos];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::subAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t rows( size() );

   const size_t ipos( rows & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( rows - ( rows % (IT::size*4UL) ) ) == ipos, "Invalid end calculation" );

   typename VT::ConstIterator it( (~rhs).begin() );
   for( size_t i=0UL; i<ipos; i+=IT::size*4UL ) {
      matrix_.store( i             , col_, matrix_.load(i             ,col_) - it.load() ); it += IT::size;
      matrix_.store( i+IT::size    , col_, matrix_.load(i+IT::size    ,col_) - it.load() ); it += IT::size;
      matrix_.store( i+IT::size*2UL, col_, matrix_.load(i+IT::size*2UL,col_) - it.load() ); it += IT::size;
      matrix_.store( i+IT::size*3UL, col_, matrix_.load(i+IT::size*3UL,col_) - it.load() ); it += IT::size;
   }
   for( size_t i=ipos; i<rows; i+=IT::size, it+=IT::size ) {
      matrix_.store( i, col_, matrix_.load(i,col_) - it.load() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,SO,SF>::subAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) -= element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::multAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) *= (~rhs)[i    ];
      matrix_(i+1UL,col_) *= (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) *= (~rhs)[ipos];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,SO,SF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DenseColumn<MT,SO,SF>::multAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t rows( size() );

   const size_t ipos( rows & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( rows - ( rows % (IT::size*4UL) ) ) == ipos, "Invalid end calculation" );

   typename VT::ConstIterator it( (~rhs).begin() );
   for( size_t i=0UL; i<ipos; i+=IT::size*4UL ) {
      matrix_.store( i             , col_, matrix_.load(i             ,col_) * it.load() ); it += IT::size;
      matrix_.store( i+IT::size    , col_, matrix_.load(i+IT::size    ,col_) * it.load() ); it += IT::size;
      matrix_.store( i+IT::size*2UL, col_, matrix_.load(i+IT::size*2UL,col_) * it.load() ); it += IT::size;
      matrix_.store( i+IT::size*3UL, col_, matrix_.load(i+IT::size*3UL,col_) * it.load() ); it += IT::size;
   }
   for( size_t i=ipos; i<rows; i+=IT::size, it+=IT::size ) {
      matrix_.store( i, col_, matrix_.load(i,col_) * it.load() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the multiplication assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , bool SF >      // Symmetry flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,SO,SF>::multAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const ResultType tmp( serial( *this ) );

   reset();

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) = tmp[element->index()] * element->value();
}
//*************************************************************************************************








//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR GENERAL ROW-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of DenseColumn for general row-major matrices.
// \ingroup views
//
// This specialization of DenseColumn adapts the class template to the requirements of general
// row-major matrices.
*/
template< typename MT >  // Type of the dense matrix
class DenseColumn<MT,false,false> : public DenseVector< DenseColumn<MT,false,false>, false >
                                  , private Column
{
 private:
   //**Type definitions****************************************************************************
   //! Composite data type of the dense matrix expression.
   typedef typename If< IsExpression<MT>, MT, MT& >::Type  Operand;
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the non-const reference and iterator types.
   /*! The \a useConst compile time constant expression represents a compilation switch for
       the non-const reference and iterator types. In case the given dense matrix of type
       \a MT is const qualified, \a useConst will be set to 1 and the dense column will
       return references and iterators to const. Otherwise \a useConst will be set to 0
       and the dense column will offer write access to the dense matrix elements both via
       the subscript operator and iterators. */
   enum { useConst = IsConst<MT>::value };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DenseColumn<MT,false,false>         This;            //!< Type of this DenseColumn instance.
   typedef typename ColumnTrait<MT>::Type      ResultType;      //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType  TransposeType;   //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType            ElementType;     //!< Type of the column elements.
   typedef typename MT::ReturnType             ReturnType;      //!< Return type for expression template evaluations
   typedef const DenseColumn&                  CompositeType;   //!< Data type for composite expression templates.

   //! Reference to a constant column value.
   typedef typename MT::ConstReference  ConstReference;

   //! Reference to a non-constant column value.
   typedef typename IfTrue< useConst, ConstReference, typename MT::Reference >::Type  Reference;
   //**********************************************************************************************

   //**ColumnIterator class definition*************************************************************
   /*!\brief Iterator over the elements of the dense column.
   */
   template< typename MatrixType >  // Type of the dense matrix
   class ColumnIterator
   {
    private:
      //*******************************************************************************************
      //! Compilation switch for the return type of the value member function.
      /*! The \a returnConst compile time constant expression represents a compilation switch for
          the return type of the member access operators. In case the given matrix type \a MatrixType
          is const qualified, \a returnConst will be set to 1 and the member access operators will
          return a reference to const. Otherwise \a returnConst will be set to 0 and the member
          access operators will offer write access to the dense matrix elements. */
      enum { returnConst = IsConst<MatrixType>::value };
      //*******************************************************************************************

    public:
      //**Type definitions*************************************************************************
      //! Return type for the access to the value of a dense element.
      typedef typename IfTrue< returnConst
                             , typename MatrixType::ConstReference
                             , typename MatrixType::Reference >::Type  Reference;

      typedef std::random_access_iterator_tag  IteratorCategory;  //!< The iterator category.
      typedef RemoveReference<Reference>       ValueType;         //!< Type of the underlying elements.
      typedef ValueType*                       PointerType;       //!< Pointer return type.
      typedef Reference                        ReferenceType;     //!< Reference return type.
      typedef ptrdiff_t                        DifferenceType;    //!< Difference between two iterators.

      // STL iterator requirements
      typedef IteratorCategory  iterator_category;  //!< The iterator category.
      typedef ValueType         value_type;         //!< Type of the underlying elements.
      typedef PointerType       pointer;            //!< Pointer return type.
      typedef ReferenceType     reference;          //!< Reference return type.
      typedef DifferenceType    difference_type;    //!< Difference between two iterators.
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Default constructor of the ColumnIterator class.
      */
      inline ColumnIterator()
         : matrix_( NULL )  // The dense matrix containing the column.
         , row_   ( 0UL  )  // The current row index.
         , column_( 0UL  )  // The current column index.
      {}
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Constructor of the ColumnIterator class.
      //
      // \param matrix The matrix containing the column.
      // \param row The row index.
      // \param column The column index.
      */
      inline ColumnIterator( MatrixType& matrix, size_t row, size_t column )
         : matrix_( &matrix )  // The dense matrix containing the column.
         , row_   ( row     )  // The current row index.
         , column_( column  )  // The current column index.
      {}
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Conversion constructor from different ColumnIterator instances.
      //
      // \param it The column iterator to be copied.
      */
      template< typename MatrixType2 >
      inline ColumnIterator( const ColumnIterator<MatrixType2>& it )
         : matrix_( it.matrix_ )  // The dense matrix containing the column.
         , row_   ( it.row_    )  // The current row index.
         , column_( it.column_ )  // The current column index.
      {}
      //*******************************************************************************************

      //**Addition assignment operator*************************************************************
      /*!\brief Addition assignment operator.
      //
      // \param inc The increment of the iterator.
      // \return The incremented iterator.
      */
      inline ColumnIterator& operator+=( size_t inc ) {
         row_ += inc;
         return *this;
      }
      //*******************************************************************************************

      //**Subtraction assignment operator**********************************************************
      /*!\brief Subtraction assignment operator.
      //
      // \param dec The decrement of the iterator.
      // \return The decremented iterator.
      */
      inline ColumnIterator& operator-=( size_t dec ) {
         row_ -= dec;
         return *this;
      }
      //*******************************************************************************************

      //**Prefix increment operator****************************************************************
      /*!\brief Pre-increment operator.
      //
      // \return Reference to the incremented iterator.
      */
      inline ColumnIterator& operator++() {
         ++row_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix increment operator***************************************************************
      /*!\brief Post-increment operator.
      //
      // \return The previous position of the iterator.
      */
      inline const ColumnIterator operator++( int ) {
         const ColumnIterator tmp( *this );
         ++(*this);
         return tmp;
      }
      //*******************************************************************************************

      //**Prefix decrement operator****************************************************************
      /*!\brief Pre-decrement operator.
      //
      // \return Reference to the decremented iterator.
      */
      inline ColumnIterator& operator--() {
         --row_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix decrement operator***************************************************************
      /*!\brief Post-decrement operator.
      //
      // \return The previous position of the iterator.
      */
      inline const ColumnIterator operator--( int ) {
         const ColumnIterator tmp( *this );
         --(*this);
         return tmp;
      }
      //*******************************************************************************************

      //**Subscript operator***********************************************************************
      /*!\brief Direct access to the dense column elements.
      //
      // \param index Access index.
      // \return Reference to the accessed value.
      */
      inline ReferenceType operator[]( size_t index ) const {
         return (*matrix_)(row_+index,column_);
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the dense vector element at the current iterator position.
      //
      // \return The current value of the dense element.
      */
      inline ReferenceType operator*() const {
         return (*matrix_)(row_,column_);
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the dense vector element at the current iterator position.
      //
      // \return Reference to the dense vector element at the current iterator position.
      */
      inline PointerType operator->() const {
         return &(*matrix_)(row_,column_);
      }
      //*******************************************************************************************

      //**Equality operator************************************************************************
      /*!\brief Equality comparison between two ColumnIterator objects.
      //
      // \param rhs The right-hand side column iterator.
      // \return \a true if the iterators refer to the same element, \a false if not.
      */
      template< typename MatrixType2 >
      inline bool operator==( const ColumnIterator<MatrixType2>& rhs ) const {
         return ( matrix_ == rhs.matrix_ ) && ( row_ == rhs.row_ ) && ( column_ == rhs.column_ );
      }
      //*******************************************************************************************

      //**Inequality operator**********************************************************************
      /*!\brief Inequality comparison between two ColumnIterator objects.
      //
      // \param rhs The right-hand side column iterator.
      // \return \a true if the iterators don't refer to the same element, \a false if they do.
      */
      template< typename MatrixType2 >
      inline bool operator!=( const ColumnIterator<MatrixType2>& rhs ) const {
         return !( *this == rhs );
      }
      //*******************************************************************************************

      //**Less-than operator***********************************************************************
      /*!\brief Less-than comparison between two ColumnIterator objects.
      //
      // \param rhs The right-hand side column iterator.
      // \return \a true if the left-hand side iterator is smaller, \a false if not.
      */
      template< typename MatrixType2 >
      inline bool operator<( const ColumnIterator<MatrixType2>& rhs ) const {
         return ( matrix_ == rhs.matrix_ ) && ( row_ < rhs.row_ ) && ( column_ == rhs.column_ );
      }
      //*******************************************************************************************

      //**Greater-than operator********************************************************************
      /*!\brief Greater-than comparison between two ColumnIterator objects.
      //
      // \param rhs The right-hand side column iterator.
      // \return \a true if the left-hand side iterator is greater, \a false if not.
      */
      template< typename MatrixType2 >
      inline bool operator>( const ColumnIterator<MatrixType2>& rhs ) const {
         return ( matrix_ == rhs.matrix_ ) && ( row_ > rhs.row_ ) && ( column_ == rhs.column_ );
      }
      //*******************************************************************************************

      //**Less-or-equal-than operator**************************************************************
      /*!\brief Less-than comparison between two ColumnIterator objects.
      //
      // \param rhs The right-hand side column iterator.
      // \return \a true if the left-hand side iterator is smaller or equal, \a false if not.
      */
      template< typename MatrixType2 >
      inline bool operator<=( const ColumnIterator<MatrixType2>& rhs ) const {
         return ( matrix_ == rhs.matrix_ ) && ( row_ <= rhs.row_ ) && ( column_ == rhs.column_ );
      }
      //*******************************************************************************************

      //**Greater-or-equal-than operator***********************************************************
      /*!\brief Greater-than comparison between two ColumnIterator objects.
      //
      // \param rhs The right-hand side column iterator.
      // \return \a true if the left-hand side iterator is greater or equal, \a false if not.
      */
      template< typename MatrixType2 >
      inline bool operator>=( const ColumnIterator<MatrixType2>& rhs ) const {
         return ( matrix_ == rhs.matrix_ ) && ( row_ >= rhs.row_ ) && ( column_ == rhs.column_ );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Calculating the number of elements between two column iterators.
      //
      // \param rhs The right-hand side column iterator.
      // \return The number of elements between the two column iterators.
      */
      inline DifferenceType operator-( const ColumnIterator& rhs ) const {
         return row_ - rhs.row_;
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between a ColumnIterator and an integral value.
      //
      // \param it The iterator to be incremented.
      // \param inc The number of elements the iterator is incremented.
      // \return The incremented iterator.
      */
      friend inline const ColumnIterator operator+( const ColumnIterator& it, size_t inc ) {
         return ColumnIterator( *it.matrix_, it.row_+inc, it.column_ );
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between an integral value and a ColumnIterator.
      //
      // \param inc The number of elements the iterator is incremented.
      // \param it The iterator to be incremented.
      // \return The incremented iterator.
      */
      friend inline const ColumnIterator operator+( size_t inc, const ColumnIterator& it ) {
         return ColumnIterator( *it.matrix_, it.row_+inc, it.column_ );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Subtraction between a ColumnIterator and an integral value.
      //
      // \param it The iterator to be decremented.
      // \param inc The number of elements the iterator is decremented.
      // \return The decremented iterator.
      */
      friend inline const ColumnIterator operator-( const ColumnIterator& it, size_t dec ) {
         return ColumnIterator( *it.matrix_, it.row_-dec, it.column_ );
      }
      //*******************************************************************************************

    private:
      //**Member variables*************************************************************************
      MatrixType* matrix_;  //!< The dense matrix containing the column.
      size_t      row_;     //!< The current row index.
      size_t      column_;  //!< The current column index.
      //*******************************************************************************************

      //**Friend declarations**********************************************************************
      template< typename MatrixType2 > friend class ColumnIterator;
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   //! Iterator over constant elements.
   typedef ColumnIterator<const MT>  ConstIterator;

   //! Iterator over non-constant elements.
   typedef typename IfTrue< useConst, ConstIterator, ColumnIterator<MT> >::Type  Iterator;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = 0 };

   //! Compilation switch for the expression template assignment strategy.
   enum { smpAssignable = MT::smpAssignable };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline DenseColumn( MT& matrix, size_t index );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator[]( size_t index );
   inline ConstReference operator[]( size_t index ) const;
   inline Iterator       begin ();
   inline ConstIterator  begin () const;
   inline ConstIterator  cbegin() const;
   inline Iterator       end   ();
   inline ConstIterator  end   () const;
   inline ConstIterator  cend  () const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
   inline DenseColumn& operator= ( const ElementType& rhs );
   inline DenseColumn& operator= ( const DenseColumn& rhs );

   template< typename VT > inline DenseColumn& operator= ( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator+=( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator-=( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator*=( const Vector<VT,false>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseColumn >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseColumn >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t       size() const;
                              inline size_t       capacity() const;
                              inline size_t       nonZeros() const;
                              inline void         reset();
   template< typename Other > inline DenseColumn& scale( const Other& scalar );
   //@}
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other >
   inline bool canAlias ( const Other* alias ) const;

   template< typename MT2, bool SO2, bool SF2 >
   inline bool canAlias ( const DenseColumn<MT2,SO2,SF2>* alias ) const;

   template< typename Other >
   inline bool isAliased( const Other* alias ) const;

   template< typename MT2, bool SO2, bool SF2 >
   inline bool isAliased( const DenseColumn<MT2,SO2,SF2>* alias ) const;

   inline bool isAligned   () const;
   inline bool canSMPAssign() const;

   template< typename VT > inline void assign    ( const DenseVector <VT,false>& rhs );
   template< typename VT > inline void assign    ( const SparseVector<VT,false>& rhs );
   template< typename VT > inline void addAssign ( const DenseVector <VT,false>& rhs );
   template< typename VT > inline void addAssign ( const SparseVector<VT,false>& rhs );
   template< typename VT > inline void subAssign ( const DenseVector <VT,false>& rhs );
   template< typename VT > inline void subAssign ( const SparseVector<VT,false>& rhs );
   template< typename VT > inline void multAssign( const DenseVector <VT,false>& rhs );
   template< typename VT > inline void multAssign( const SparseVector<VT,false>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< Not< IsRestricted<MT2> >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const Vector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Operand      matrix_;  //!< The dense matrix containing the column.
   const size_t col_;     //!< The index of the column in the matrix.
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   template< typename MT2, bool SO2, bool SF2 > friend class DenseColumn;

   template< typename MT2, bool SO2, bool SF2 >
   friend bool isSame( const DenseColumn<MT2,SO2,SF2>& a, const DenseColumn<MT2,SO2,SF2>& b );

   template< typename MT2, bool SO2, bool SF2 >
   friend typename DerestrictTrait< DenseColumn<MT2,SO2,SF2> >::Type
      derestrict( DenseColumn<MT2,SO2,SF2>& dm );
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE        ( MT );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE    ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE     ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_TRANSEXPR_TYPE       ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE         ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE       ( MT );
   BLAZE_STATIC_ASSERT( !IsRestricted<MT>::value || IsLower<MT>::value || IsUpper<MT>::value );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The constructor for DenseColumn.
//
// \param matrix The matrix containing the column.
// \param index The index of the column.
// \exception std::invalid_argument Invalid column access index.
*/
template< typename MT >  // Type of the dense matrix
inline DenseColumn<MT,false,false>::DenseColumn( MT& matrix, size_t index )
   : matrix_( matrix )  // The dense matrix containing the column
   , col_   ( index  )  // The index of the column in the matrix
{
   if( matrix_.columns() <= index )
      throw std::invalid_argument( "Invalid column access index" );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subscript operator for the direct access to the column elements.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return Reference to the accessed value.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::Reference
   DenseColumn<MT,false,false>::operator[]( size_t index )
{
   BLAZE_USER_ASSERT( index < size(), "Invalid column access index" );
   return matrix_(index,col_);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subscript operator for the direct access to the column elements.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return Reference to the accessed value.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::ConstReference
   DenseColumn<MT,false,false>::operator[]( size_t index ) const
{
   BLAZE_USER_ASSERT( index < size(), "Invalid column access index" );
   return matrix_(index,col_);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::Iterator DenseColumn<MT,false,false>::begin()
{
   return Iterator( matrix_, 0UL, col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::ConstIterator
   DenseColumn<MT,false,false>::begin() const
{
   return ConstIterator( matrix_, 0UL, col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::ConstIterator
   DenseColumn<MT,false,false>::cbegin() const
{
   return ConstIterator( matrix_, 0UL, col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::Iterator DenseColumn<MT,false,false>::end()
{
   return Iterator( matrix_, size(), col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::ConstIterator
   DenseColumn<MT,false,false>::end() const
{
   return ConstIterator( matrix_, size(), col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,false>::ConstIterator
   DenseColumn<MT,false,false>::cend() const
{
   return ConstIterator( matrix_, size(), col_ );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Homogenous assignment to all column elements.
//
// \param rhs Scalar value to be assigned to all column elements.
// \return Reference to the assigned column.
//
// This function homogeneously assigns the given value to all elements of the column. Note that in
// case the underlying dense matrix is a lower/upper matrix only lower/upper and diagonal elements
// of the underlying matrix are modified.
*/
template< typename MT >  // Type of the dense matrix
inline DenseColumn<MT,false,false>&
   DenseColumn<MT,false,false>::operator=( const ElementType& rhs )
{
   const size_t ibegin( ( IsLower<MT>::value )?( col_ ):( 0UL ) );
   const size_t iend  ( ( IsUpper<MT>::value )?( col_+1UL ):( size() ) );

   for( size_t i=ibegin; i<iend; ++i )
      matrix_(i,col_) = rhs;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Copy assignment operator for DenseColumn.
//
// \param rhs Dense column to be copied.
// \return Reference to the assigned column.
// \exception std::invalid_argument Column sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two columns don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
inline DenseColumn<MT,false,false>&
   DenseColumn<MT,false,false>::operator=( const DenseColumn& rhs )
{
   if( &rhs == this ) return *this;

   if( size() != rhs.size() )
      throw std::invalid_argument( "Column sizes do not match" );

   if( !preservesInvariant( matrix_, rhs ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   smpAssign( left, rhs );

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different vectors.
//
// \param rhs Vector to be assigned.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,false>&
   DenseColumn<MT,false,false>::operator=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE  ( ResultType );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const ResultType tmp( right );
      smpAssign( left, tmp );
   }
   else {
      if( IsSparseVector<VT>::value )
         reset();
      smpAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Addition assignment operator for the addition of a vector (\f$ \vec{a}+=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be added to the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,false>&
   DenseColumn<MT,false,false>::operator+=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpAddAssign( left, tmp );
   }
   else {
      smpAddAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subtraction assignment operator for the subtraction of a vector (\f$ \vec{a}-=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be subtracted from the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,false>&
   DenseColumn<MT,false,false>::operator-=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpSubAssign( left, tmp );
   }
   else {
      smpSubAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication of a vector
//        (\f$ \vec{a}*=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be multiplied with the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,false>&
   DenseColumn<MT,false,false>::operator*=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( (~rhs).canAlias( &matrix_ ) || IsSparseVector<VT>::value ) {
      const ResultType tmp( *this * (~rhs) );
      smpAssign( left, tmp );
   }
   else {
      smpMultAssign( left, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication between a vector and
//        a scalar value (\f$ \vec{a}*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the vector.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseColumn<MT,false,false> >::Type&
   DenseColumn<MT,false,false>::operator*=( Other rhs )
{
   for( size_t i=0UL; i<size(); ++i )
      matrix_(i,col_) *= rhs;
   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Division assignment operator for the division of a vector by a scalar value
//        (\f$ \vec{a}/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the vector.
//
// \b Note: A division by zero is only checked by an user assert.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseColumn<MT,false,false> >::Type&
   DenseColumn<MT,false,false>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   typedef typename DivTrait<ElementType,Other>::Type  DT;
   typedef typename If< IsNumeric<DT>, DT, Other >::Type  Tmp;

   // Depending on the two involved data types, an integer division is applied or a
   // floating point division is selected.
   if( IsNumeric<DT>::value && IsFloatingPoint<DT>::value ) {
      const Tmp tmp( Tmp(1)/static_cast<Tmp>( rhs ) );
      for( size_t i=0UL; i<size(); ++i )
         matrix_(i,col_) *= tmp;
   }
   else {
      for( size_t i=0UL; i<size(); ++i )
         matrix_(i,col_) /= rhs;
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current size/dimension of the column.
//
// \return The size of the column.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseColumn<MT,false,false>::size() const
{
   return matrix_.rows();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum capacity of the dense column.
//
// \return The capacity of the dense column.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseColumn<MT,false,false>::capacity() const
{
   return matrix_.rows();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the column.
//
// \return The number of non-zero elements in the column.
//
// Note that the number of non-zero elements is always less than or equal to the current number
// of columns of the matrix containing the column.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseColumn<MT,false,false>::nonZeros() const
{
   const size_t rows( size() );
   size_t nonzeros( 0UL );

   for( size_t i=0UL; i<rows; ++i )
      if( !isDefault( matrix_(i,col_) ) )
         ++nonzeros;

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename MT >  // Type of the dense matrix
inline void DenseColumn<MT,false,false>::reset()
{
   using blaze::clear;
   const size_t rows( size() );
   for( size_t i=0UL; i<rows; ++i )
      clear( matrix_(i,col_) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Scaling of the column by the scalar value \a scalar (\f$ \vec{a}=\vec{b}*s \f$).
//
// \param scalar The scalar value for the column scaling.
// \return Reference to the dense column.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the scalar value
inline DenseColumn<MT,false,false>& DenseColumn<MT,false,false>::scale( const Other& scalar )
{
   for( size_t i=0UL; i<size(); ++i ) {
      matrix_(i,col_) *= scalar;
   }
   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying matrix.
//
// \param lhs The matrix to be assigned to.
// \param rhs The vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying matrix of type \a MT would be
// violated by an assignment of the given vector \a rhs. In case the matrix would be preserved,
// the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side vector
inline typename EnableIf< Not< IsRestricted<MT2> >, bool >::Type
   DenseColumn<MT,false,false>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const Vector<VT,false>& rhs )
{
   UNUSED_PARAMETER( lhs, rhs );

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying lower triangular matrix.
//
// \param lhs The lower matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying lower triangular matrix of type
// \a MT would be violated by an assignment of the given dense vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,false>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=0UL; i<col_; ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying lower triangular matrix.
//
// \param lhs The lower matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying lower triangular matrix of type
// \a MT would be violated by an assignment of the given sparse vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,false>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).begin(); element!=(~rhs).lowerBound( col_ ); ++element ) {
      if( !isDefault( element->value() ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying upper triangular matrix.
//
// \param lhs The upper matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying upper triangular matrix of type
// \a MT would be violated by an assignment of the given dense vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,false>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=col_+1UL; i<size(); ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying upper triangular matrix.
//
// \param lhs The upper matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying upper triangular matrix of type
// \a MT would be violated by an assignment of the given sparse vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,false>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).lowerBound( col_+1UL ); element!=(~rhs).end(); ++element ) {
      if( !isDefault( element->value() ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying diagonal matrix.
//
// \param lhs The diagonal matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying diagonal matrix of type \a MT would
// be violated by an assignment of the given dense vector \a rhs. In case the matrix would be
// preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
   DenseColumn<MT,false,false>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=0UL; i<col_; ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   for( size_t i=col_+1UL; i<size(); ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying diagonal matrix.
//
// \param lhs The diagonal matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying diagonal matrix of type \a MT would
// be violated by an assignment of the given sparse vector \a rhs. In case the matrix would be
// preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
   DenseColumn<MT,false,false>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element ) {
      if( element->index() != col_ && !isDefault( element->value() ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address can alias with the dense column. In contrast
// to the isAliased() function this function is allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the foreign expression
inline bool DenseColumn<MT,false,false>::canAlias( const Other* alias ) const
{
   return matrix_.isAliased( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column can alias with the given dense column \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address can alias with the dense column. In contrast
// to the isAliased() function this function is allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Data type of the foreign dense column
        , bool SO2       // Storage order of the foreign dense column
        , bool SF2 >     // Symmetry flag of the foreign dense column
inline bool DenseColumn<MT,false,false>::canAlias( const DenseColumn<MT2,SO2,SF2>* alias ) const
{
   return matrix_.isAliased( &alias->matrix_ ) && ( col_ == alias->col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address is aliased with the dense column. In contrast
// to the canAlias() function this function is not allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the foreign expression
inline bool DenseColumn<MT,false,false>::isAliased( const Other* alias ) const
{
   return matrix_.isAliased( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column is aliased with the given dense column \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address is aliased with the dense column. In contrast
// to the canAlias() function this function is not allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Data type of the foreign dense column
        , bool SO2       // Storage order of the foreign dense column
        , bool SF2 >     // Symmetry flag of the foreign dense column
inline bool DenseColumn<MT,false,false>::isAliased( const DenseColumn<MT2,SO2,SF2>* alias ) const
{
   return matrix_.isAliased( &alias->matrix_ ) && ( col_ == alias->col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column is properly aligned in memory.
//
// \return \a true in case the dense column is aligned, \a false if not.
//
// This function returns whether the dense column is guaranteed to be properly aligned in memory,
// i.e. whether the beginning and the end of the dense column are guaranteed to conform to the
// alignment restrictions of the element type \a Type.
*/
template< typename MT >  // Type of the dense matrix
inline bool DenseColumn<MT,false,false>::isAligned() const
{
   return false;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column can be used in SMP assignments.
//
// \return \a true in case the dense column can be used in SMP assignments, \a false if not.
//
// This function returns whether the dense column can be used in SMP assignments. In contrast
// to the \a smpAssignable member enumeration, which is based solely on compile time information,
// this function additionally provides runtime information (as for instance the current size of
// the dense column).
*/
template< typename MT >  // Type of the dense matrix
inline bool DenseColumn<MT,false,false>::canSMPAssign() const
{
   return ( size() > SMP_DVECASSIGN_THRESHOLD );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline void DenseColumn<MT,false,false>::assign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) = (~rhs)[i    ];
      matrix_(i+1UL,col_) = (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) = (~rhs)[ipos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,false>::assign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) = element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline void DenseColumn<MT,false,false>::addAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) += (~rhs)[i    ];
      matrix_(i+1UL,col_) += (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) += (~rhs)[ipos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,false>::addAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) += element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline void DenseColumn<MT,false,false>::subAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) -= (~rhs)[i    ];
      matrix_(i+1UL,col_) -= (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) -= (~rhs)[ipos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,false>::subAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) -= element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline void DenseColumn<MT,false,false>::multAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t ipos( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<ipos; i+=2UL ) {
      matrix_(i    ,col_) *= (~rhs)[i    ];
      matrix_(i+1UL,col_) *= (~rhs)[i+1UL];
   }
   if( ipos < (~rhs).size() )
      matrix_(ipos,col_) *= (~rhs)[ipos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the multiplication assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,false>::multAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const ResultType tmp( serial( *this ) );

   reset();

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(element->index(),col_) = tmp[element->index()] * element->value();
}
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR SYMMETRIC ROW-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of DenseColumn for symmetric row-major matrices.
// \ingroup views
//
// This specialization of DenseColumn adapts the class template to the requirements of symmetric
// row-major matrices.
*/
template< typename MT >  // Type of the dense matrix
class DenseColumn<MT,false,true> : public DenseVector< DenseColumn<MT,false,true>, false >
                                 , private Column
{
 private:
   //**Type definitions****************************************************************************
   //! Composite data type of the dense matrix expression.
   typedef typename If< IsExpression<MT>, MT, MT& >::Type  Operand;

   //! Intrinsic trait for the column element type.
   typedef IntrinsicTrait<typename MT::ElementType>  IT;
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the non-const reference and iterator types.
   /*! The \a useConst compile time constant expression represents a compilation switch for
       the non-const reference and iterator types. In case the given dense matrix of type
       \a MT is const qualified, \a useConst will be set to 1 and the dense column will
       return references and iterators to const. Otherwise \a useConst will be set to 0
       and the dense column will offer write access to the dense matrix elements both via
       the subscript operator and iterators. */
   enum { useConst = IsConst<MT>::value };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DenseColumn<MT,false,true>          This;           //!< Type of this DenseColumn instance.
   typedef typename ColumnTrait<MT>::Type      ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::TransposeType  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType            ElementType;    //!< Type of the column elements.
   typedef typename IT::Type                   IntrinsicType;  //!< Intrinsic type of the column elements.
   typedef typename MT::ReturnType             ReturnType;     //!< Return type for expression template evaluations
   typedef const DenseColumn&                  CompositeType;  //!< Data type for composite expression templates.

   //! Reference to a constant column value.
   typedef typename MT::ConstReference  ConstReference;

   //! Reference to a non-constant column value.
   typedef typename IfTrue< useConst, ConstReference, typename MT::Reference >::Type  Reference;

   //! Pointer to a constant row value.
   typedef const ElementType*  ConstPointer;

   //! Pointer to a non-constant row value.
   typedef typename IfTrue< useConst, ConstPointer, ElementType* >::Type  Pointer;

   //! Iterator over constant elements.
   typedef typename MT::ConstIterator  ConstIterator;

   //! Iterator over non-constant elements.
   typedef typename IfTrue< useConst, ConstIterator, typename MT::Iterator >::Type  Iterator;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = MT::vectorizable };

   //! Compilation switch for the expression template assignment strategy.
   enum { smpAssignable = MT::smpAssignable };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline DenseColumn( MT& matrix, size_t index );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator[]( size_t index );
   inline ConstReference operator[]( size_t index ) const;
   inline Pointer        data  ();
   inline ConstPointer   data  () const;
   inline Iterator       begin ();
   inline ConstIterator  begin () const;
   inline ConstIterator  cbegin() const;
   inline Iterator       end   ();
   inline ConstIterator  end   () const;
   inline ConstIterator  cend  () const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
   inline DenseColumn& operator=( const ElementType& rhs );
   inline DenseColumn& operator=( const DenseColumn& rhs );

   template< typename VT > inline DenseColumn& operator= ( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator+=( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator-=( const Vector<VT,false>& rhs );
   template< typename VT > inline DenseColumn& operator*=( const Vector<VT,false>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseColumn >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseColumn >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t       size() const;
                              inline size_t       capacity() const;
                              inline size_t       nonZeros() const;
                              inline void         reset();
   template< typename Other > inline DenseColumn& scale( const Other& scalar );
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAddAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value &&
                     IntrinsicTrait<ElementType>::addition };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedSubAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value &&
                     IntrinsicTrait<ElementType>::subtraction };
   };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedMultAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<ElementType,typename VT::ElementType>::value &&
                     IntrinsicTrait<ElementType>::multiplication };
   };
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other >
   inline bool canAlias( const Other* alias ) const;

   template< typename MT2, bool SO2, bool SF2 >
   inline bool canAlias( const DenseColumn<MT2,SO2,SF2>* alias ) const;

   template< typename Other >
   inline bool isAliased( const Other* alias ) const;

   template< typename MT2, bool SO2, bool SF2 >
   inline bool isAliased( const DenseColumn<MT2,SO2,SF2>* alias ) const;

   inline bool isAligned   () const;
   inline bool canSMPAssign() const;

   BLAZE_ALWAYS_INLINE IntrinsicType load ( size_t index ) const;
   BLAZE_ALWAYS_INLINE IntrinsicType loadu( size_t index ) const;

   BLAZE_ALWAYS_INLINE void store ( size_t index, const IntrinsicType& value );
   BLAZE_ALWAYS_INLINE void storeu( size_t index, const IntrinsicType& value );
   BLAZE_ALWAYS_INLINE void stream( size_t index, const IntrinsicType& value );

   template< typename VT >
   inline typename DisableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void assign( const SparseVector<VT,false>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void addAssign( const SparseVector<VT,false>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void subAssign( const SparseVector<VT,false>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,false>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,false>& rhs );

   template< typename VT > inline void multAssign( const SparseVector<VT,false>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< Not< IsRestricted<MT2> >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const Vector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs );

   template< typename MT2, bool SO2, typename VT >
   inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
      preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs );
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Operand      matrix_;  //!< The dense matrix containing the column.
   const size_t col_;     //!< The index of the column in the matrix.
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   template< typename MT2, bool SO2, bool SF2 > friend class DenseColumn;

   template< typename MT2, bool SO2, bool SF2 >
   friend bool isSame( const DenseColumn<MT2,SO2,SF2>& a, const DenseColumn<MT2,SO2,SF2>& b );

   template< typename MT2, bool SO2, bool SF2 >
   friend typename DerestrictTrait< DenseColumn<MT2,SO2,SF2> >::Type
      derestrict( DenseColumn<MT2,SO2,SF2>& dm );
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE    ( MT );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_SYMMETRIC_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_TRANSEXPR_TYPE   ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE     ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE   ( MT );
   BLAZE_STATIC_ASSERT( !IsRestricted<MT>::value || IsLower<MT>::value || IsUpper<MT>::value );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The constructor for DenseColumn.
//
// \param matrix The matrix containing the column.
// \param index The index of the column.
// \exception std::invalid_argument Invalid column access index.
*/
template< typename MT >  // Type of the dense matrix
inline DenseColumn<MT,false,true>::DenseColumn( MT& matrix, size_t index )
   : matrix_( matrix )  // The dense matrix containing the column
   , col_   ( index  )  // The index of the column in the matrix
{
   if( matrix_.columns() <= index )
      throw std::invalid_argument( "Invalid column access index" );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subscript operator for the direct access to the column elements.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return Reference to the accessed value.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::Reference
   DenseColumn<MT,false,true>::operator[]( size_t index )
{
   BLAZE_USER_ASSERT( index < size(), "Invalid column access index" );
   return matrix_(col_,index);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subscript operator for the direct access to the column elements.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return Reference to the accessed value.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::ConstReference
   DenseColumn<MT,false,true>::operator[]( size_t index ) const
{
   BLAZE_USER_ASSERT( index < size(), "Invalid column access index" );
   return matrix_(col_,index);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the column elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::Pointer DenseColumn<MT,false,true>::data()
{
   return matrix_.data( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the column elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::ConstPointer DenseColumn<MT,false,true>::data() const
{
   return matrix_.data( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::Iterator DenseColumn<MT,false,true>::begin()
{
   return matrix_.begin( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::ConstIterator DenseColumn<MT,false,true>::begin() const
{
   return matrix_.cbegin( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first element of the column.
//
// \return Iterator to the first element of the column.
//
// This function returns an iterator to the first element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::ConstIterator DenseColumn<MT,false,true>::cbegin() const
{
   return matrix_.cbegin( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::Iterator DenseColumn<MT,false,true>::end()
{
   return matrix_.end( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::ConstIterator DenseColumn<MT,false,true>::end() const
{
   return matrix_.cend( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last element of the column.
//
// \return Iterator just past the last element of the column.
//
// This function returns an iterator just past the last element of the column.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseColumn<MT,false,true>::ConstIterator DenseColumn<MT,false,true>::cend() const
{
   return matrix_.cend( col_ );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Homogenous assignment to all column elements.
//
// \param rhs Scalar value to be assigned to all column elements.
// \return Reference to the assigned column.
*/
template< typename MT >  // Type of the dense matrix
inline DenseColumn<MT,false,true>& DenseColumn<MT,false,true>::operator=( const ElementType& rhs )
{
   const size_t jbegin( ( IsUpper<MT>::value )?( col_ ):( 0UL ) );
   const size_t jend  ( ( IsLower<MT>::value )?( col_+1UL ):( size() ) );

   for( size_t j=jbegin; j<jend; ++j )
      matrix_(col_,j) = rhs;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Copy assignment operator for DenseColumn.
//
// \param rhs Dense column to be copied.
// \return Reference to the assigned column.
// \exception std::invalid_argument Column sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two columns don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
inline DenseColumn<MT,false,true>& DenseColumn<MT,false,true>::operator=( const DenseColumn& rhs )
{
   if( &rhs == this ) return *this;

   if( size() != rhs.size() )
      throw std::invalid_argument( "Column sizes do not match" );

   if( !preservesInvariant( matrix_, rhs ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   smpAssign( left, rhs );

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different vectors.
//
// \param rhs Vector to be assigned.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,true>&
   DenseColumn<MT,false,true>::operator=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   if( IsSparseVector<VT>::value )
      reset();

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpAssign( left, tmp );
   }
   else {
      smpAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Addition assignment operator for the addition of a vector (\f$ \vec{a}+=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be added to the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,true>&
   DenseColumn<MT,false,true>::operator+=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpAddAssign( left, tmp );
   }
   else {
      smpAddAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subtraction assignment operator for the subtraction of a vector (\f$ \vec{a}-=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be subtracted from the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
// \exception std::invalid_argument Invalid assignment to restricted matrix.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown. Also, if the underlying matrix \a MT is a lower or upper triangular matrix and the
// assignment would violate its lower or upper property, respectively, a \a std::invalid_argument
// exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,true>&
   DenseColumn<MT,false,true>::operator-=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typedef typename If< IsRestricted<MT>, typename VT::CompositeType, const VT& >::Type  Right;
   Right right( ~rhs );

   if( !preservesInvariant( matrix_, right ) )
      throw std::invalid_argument( "Invalid assignment to restricted matrix" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( IsReference<Right>::value && right.canAlias( &matrix_ ) ) {
      const typename VT::ResultType tmp( right );
      smpSubAssign( left, tmp );
   }
   else {
      smpSubAssign( left, right );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication of a vector
//        (\f$ \vec{a}*=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be multiplied with the dense column.
// \return Reference to the assigned column.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side vector
inline DenseColumn<MT,false,true>&
   DenseColumn<MT,false,true>::operator*=( const Vector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_VECTOR_TYPE ( typename VT::ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename VT::ResultType );

   if( size() != (~rhs).size() )
      throw std::invalid_argument( "Vector sizes do not match" );

   typename DerestrictTrait<This>::Type left( derestrict( *this ) );

   if( (~rhs).canAlias( &matrix_ ) || IsSparseVector<VT>::value ) {
      const ResultType tmp( *this * (~rhs) );
      smpAssign( left, tmp );
   }
   else {
      smpMultAssign( left, ~rhs );
   }

   BLAZE_INTERNAL_ASSERT( !IsLower<MT>::value || isLower( derestrict( matrix_ ) ), "Lower violation detected" );
   BLAZE_INTERNAL_ASSERT( !IsUpper<MT>::value || isUpper( derestrict( matrix_ ) ), "Upper violation detected" );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication between a vector and
//        a scalar value (\f$ \vec{a}*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the vector.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseColumn<MT,false,true> >::Type&
   DenseColumn<MT,false,true>::operator*=( Other rhs )
{
   return operator=( (*this) * rhs );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Division assignment operator for the division of a vector by a scalar value
//        (\f$ \vec{a}/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the vector.
//
// \b Note: A division by zero is only checked by an user assert.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseColumn<MT,false,true> >::Type&
   DenseColumn<MT,false,true>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   return operator=( (*this) / rhs );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current size/dimension of the column.
//
// \return The size of the column.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseColumn<MT,false,true>::size() const
{
   return matrix_.rows();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum capacity of the dense column.
//
// \return The capacity of the dense column.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseColumn<MT,false,true>::capacity() const
{
   return matrix_.capacity( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the column.
//
// \return The number of non-zero elements in the column.
//
// Note that the number of non-zero elements is always less than or equal to the current number
// of rows of the matrix containing the column.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseColumn<MT,false,true>::nonZeros() const
{
   return matrix_.nonZeros( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename MT >  // Type of the dense matrix
inline void DenseColumn<MT,false,true>::reset()
{
   matrix_.reset( col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Scaling of the column by the scalar value \a scalar (\f$ \vec{a}=\vec{b}*s \f$).
//
// \param scalar The scalar value for the column scaling.
// \return Reference to the dense column.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the scalar value
inline DenseColumn<MT,false,true>& DenseColumn<MT,false,true>::scale( const Other& scalar )
{
   for( size_t j=0UL; j<size(); ++j ) {
      matrix_(col_,j) *= scalar;
   }
   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying matrix.
//
// \param lhs The matrix to be assigned to.
// \param rhs The vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying matrix of type \a MT would be
// violated by an assignment of the given vector \a rhs. In case the matrix would be preserved,
// the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side vector
inline typename EnableIf< Not< IsRestricted<MT2> >, bool >::Type
   DenseColumn<MT,false,true>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const Vector<VT,false>& rhs )
{
   UNUSED_PARAMETER( lhs, rhs );

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying lower triangular matrix.
//
// \param lhs The lower matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying lower triangular matrix of type
// \a MT would be violated by an assignment of the given dense vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,true>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=0UL; i<col_; ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying lower triangular matrix.
//
// \param lhs The lower matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying lower triangular matrix of type
// \a MT would be violated by an assignment of the given sparse vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< And< IsLower<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,true>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).begin(); element!=(~rhs).lowerBound( col_ ); ++element ) {
      if( !isDefault( element->value() ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying upper triangular matrix.
//
// \param lhs The upper matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying upper triangular matrix of type
// \a MT would be violated by an assignment of the given dense vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,true>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=col_+1UL; i<size(); ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying upper triangular matrix.
//
// \param lhs The upper matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying upper triangular matrix of type
// \a MT would be violated by an assignment of the given sparse vector \a rhs. In case the
// matrix would be preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< And< IsUpper<MT2>, Not< IsDiagonal<MT2> > >, bool >::Type
   DenseColumn<MT,false,true>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).lowerBound( col_+1UL ); element!=(~rhs).end(); ++element ) {
      if( !isDefault( element->value() ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying diagonal matrix.
//
// \param lhs The diagonal matrix to be assigned to.
// \param rhs The dense vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying diagonal matrix of type \a MT would
// be violated by an assignment of the given dense vector \a rhs. In case the matrix would be
// preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
   DenseColumn<MT,false,true>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   for( size_t i=0UL; i<col_; ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   for( size_t i=col_+1UL; i<size(); ++i ) {
      if( !isDefault( (~rhs)[i] ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Checking for possible invariant violations of the underlying diagonal matrix.
//
// \param lhs The diagonal matrix to be assigned to.
// \param rhs The sparse vector to be checked.
// \return \a true in case the invariants of the matrix are preserved, \a false if not.
//
// This function checks if the invariants of the underlying diagonal matrix of type \a MT would
// be violated by an assignment of the given sparse vector \a rhs. In case the matrix would be
// preserved, the function returns \a true. Otherwise it returns \a false.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the left-hand side dense matrix
        , bool SO2       // Storage order of the left-hand side dense matrix
        , typename VT >  // Type of the right-hand side sparse vector
inline typename EnableIf< IsDiagonal<MT2>, bool >::Type
   DenseColumn<MT,false,true>::preservesInvariant( const DenseMatrix<MT2,SO2>& lhs, const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( VT );

   UNUSED_PARAMETER( lhs );

   typedef typename VT::ConstIterator  RhsIterator;

   for( RhsIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element ) {
      if( element->index() != col_ && !isDefault( element->value() ) )
         return false;
   }

   return true;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address can alias with the dense column. In
// contrast to the isAliased() function this function is allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the foreign expression
inline bool DenseColumn<MT,false,true>::canAlias( const Other* alias ) const
{
   return matrix_.isAliased( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column can alias with the given dense column \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address can alias with the dense column. In
// contrast to the isAliased() function this function is allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Data type of the foreign dense column
        , bool SO2       // Storage order of the foreign dense column
        , bool SF2 >     // Symmetry flag of the foreign dense column
inline bool DenseColumn<MT,false,true>::canAlias( const DenseColumn<MT2,SO2,SF2>* alias ) const
{
   return matrix_.isAliased( alias->matrix_ ) && ( col_ == alias->col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address is aliased with the dense column. In
// contrast to the canAlias() function this function is not allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the foreign expression
inline bool DenseColumn<MT,false,true>::isAliased( const Other* alias ) const
{
   return matrix_.isAliased( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column is aliased with the given dense column \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this dense column, \a false if not.
//
// This function returns whether the given address is aliased with the dense column. In
// contrast to the canAlias() function this function is not allowed to use compile time
// expressions to optimize the evaluation.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Data type of the foreign dense column
        , bool SO2       // Storage order of the foreign dense column
        , bool SF2 >     // Symmetry flag of the foreign dense column
inline bool DenseColumn<MT,false,true>::isAliased( const DenseColumn<MT2,SO2,SF2>* alias ) const
{
   return matrix_.isAliased( &alias->matrix_ ) && ( col_ == alias->col_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column is properly aligned in memory.
//
// \return \a true in case the dense column is aligned, \a false if not.
//
// This function returns whether the dense column is guaranteed to be properly aligned in memory,
// i.e. whether the beginning and the end of the dense column are guaranteed to conform to the
// alignment restrictions of the element type \a Type.
*/
template< typename MT >  // Type of the dense matrix
inline bool DenseColumn<MT,false,true>::isAligned() const
{
   return matrix_.isAligned();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the dense column can be used in SMP assignments.
//
// \return \a true in case the dense column can be used in SMP assignments, \a false if not.
//
// This function returns whether the dense column can be used in SMP assignments. In contrast
// to the \a smpAssignable member enumeration, which is based solely on compile time information,
// this function additionally provides runtime information (as for instance the current size of
// the dense column).
*/
template< typename MT >  // Type of the dense matrix
inline bool DenseColumn<MT,false,true>::canSMPAssign() const
{
   return ( size() > SMP_DVECASSIGN_THRESHOLD );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned load of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return The loaded intrinsic element.
//
// This function performs an aligned load of a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT >  // Type of the dense matrix
BLAZE_ALWAYS_INLINE typename DenseColumn<MT,false,true>::IntrinsicType
   DenseColumn<MT,false,true>::load( size_t index ) const
{
   return matrix_.load( col_, index );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Unaligned load of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \return The loaded intrinsic element.
//
// This function performs an unaligned load of a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT >  // Type of the dense matrix
BLAZE_ALWAYS_INLINE typename DenseColumn<MT,false,true>::IntrinsicType
   DenseColumn<MT,false,true>::loadu( size_t index ) const
{
   return matrix_.loadu( col_, index );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned store of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned store a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be
// called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename MT >  // Type of the dense matrix
BLAZE_ALWAYS_INLINE void DenseColumn<MT,false,true>::store( size_t index, const IntrinsicType& value )
{
   matrix_.store( col_, index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Unaligned store of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an unaligned store a specific intrinsic element of the dense column.
// The index must be smaller than the number of matrix rows. This function must \b NOT be
// called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename MT >  // Type of the dense matrix
BLAZE_ALWAYS_INLINE void DenseColumn<MT,false,true>::storeu( size_t index, const IntrinsicType& value )
{
   matrix_.storeu( col_, index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned, non-temporal store of an intrinsic element of the dense column.
//
// \param index Access index. The index must be smaller than the number of matrix rows.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned, non-temporal store a specific intrinsic element of the
// dense column. The index must be smaller than the number of matrix rows. This function must
// \b NOT be called explicitly! It is used internally for the performance optimized evaluation
// of expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename MT >  // Type of the dense matrix
BLAZE_ALWAYS_INLINE void DenseColumn<MT,false,true>::stream( size_t index, const IntrinsicType& value )
{
   matrix_.stream( col_, index, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DenseColumn<MT,false,true>::assign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t jpos( (~rhs).size() & size_t(-2) );
   for( size_t j=0UL; j<jpos; j+=2UL ) {
      matrix_(col_,j    ) = (~rhs)[j    ];
      matrix_(col_,j+1UL) = (~rhs)[j+1UL];
   }
   if( jpos < (~rhs).size() )
      matrix_(col_,jpos) = (~rhs)[jpos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DenseColumn<MT,false,true>::assign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t columns( size() );

   if( useStreaming && columns > ( cacheSize/( sizeof(ElementType) * 3UL ) ) && !(~rhs).isAliased( this ) )
   {
      for( size_t j=0UL; j<columns; j+=IT::size ) {
         matrix_.stream( col_, j, (~rhs).load(j) );
      }
   }
   else
   {
      const size_t jpos( columns & size_t(-IT::size*4) );
      BLAZE_INTERNAL_ASSERT( ( columns - ( columns % (IT::size*4UL) ) ) == jpos, "Invalid end calculation" );

      typename VT::ConstIterator it( (~rhs).begin() );
      for( size_t j=0UL; j<jpos; j+=IT::size*4UL ) {
         matrix_.store( col_, j             , it.load() ); it+=IT::size;
         matrix_.store( col_, j+IT::size    , it.load() ); it+=IT::size;
         matrix_.store( col_, j+IT::size*2UL, it.load() ); it+=IT::size;
         matrix_.store( col_, j+IT::size*3UL, it.load() ); it+=IT::size;
      }
      for( size_t j=jpos; j<columns; j+=IT::size, it+=IT::size ) {
         matrix_.store( col_, j, it.load() );
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,true>::assign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(col_,element->index()) = element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DenseColumn<MT,false,true>::addAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t jpos( (~rhs).size() & size_t(-2) );
   for( size_t j=0UL; j<jpos; j+=2UL ) {
      matrix_(col_,j    ) += (~rhs)[j    ];
      matrix_(col_,j+1UL) += (~rhs)[j+1UL];
   }
   if( jpos < (~rhs).size() )
      matrix_(col_,jpos) += (~rhs)[jpos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DenseColumn<MT,false,true>::addAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t columns( size() );

   const size_t jpos( columns & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( columns - ( columns % (IT::size*4UL) ) ) == jpos, "Invalid end calculation" );

   typename VT::ConstIterator it( (~rhs).begin() );
   for( size_t j=0UL; j<jpos; j+=IT::size*4UL ) {
      matrix_.store( col_, j             , matrix_.load(col_,j             ) + it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size    , matrix_.load(col_,j+IT::size    ) + it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size*2UL, matrix_.load(col_,j+IT::size*2UL) + it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size*3UL, matrix_.load(col_,j+IT::size*3UL) + it.load() ); it += IT::size;
   }
   for( size_t j=jpos; j<columns; j+=IT::size, it+=IT::size ) {
      matrix_.store( col_, j, matrix_.load(col_,j) + it.load() );
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,true>::addAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(col_,element->index()) += element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DenseColumn<MT,false,true>::subAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t jpos( (~rhs).size() & size_t(-2) );
   for( size_t j=0UL; j<jpos; j+=2UL ) {
      matrix_(col_,j    ) -= (~rhs)[j    ];
      matrix_(col_,j+1UL) -= (~rhs)[j+1UL];
   }
   if( jpos < (~rhs).size() )
      matrix_(col_,jpos) -= (~rhs)[jpos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DenseColumn<MT,false,true>::subAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t columns( size() );

   const size_t jpos( columns & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( columns - ( columns % (IT::size*4UL) ) ) == jpos, "Invalid end calculation" );

   typename VT::ConstIterator it( (~rhs).begin() );
   for( size_t j=0UL; j<jpos; j+=IT::size*4UL ) {
      matrix_.store( col_, j             , matrix_.load(col_,j             ) - it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size    , matrix_.load(col_,j+IT::size    ) - it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size*2UL, matrix_.load(col_,j+IT::size*2UL) - it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size*3UL, matrix_.load(col_,j+IT::size*3UL) - it.load() ); it += IT::size;
   }
   for( size_t j=jpos; j<columns; j+=IT::size, it+=IT::size ) {
      matrix_.store( col_, j, matrix_.load(col_,j) - it.load() );
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,true>::subAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(col_,element->index()) -= element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DenseColumn<MT,false,true>::multAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t jpos( (~rhs).size() & size_t(-2) );
   for( size_t j=0UL; j<jpos; j+=2UL ) {
      matrix_(col_,j    ) *= (~rhs)[j    ];
      matrix_(col_,j+1UL) *= (~rhs)[j+1UL];
   }
   if( jpos < (~rhs).size() )
      matrix_(col_,jpos) *= (~rhs)[jpos];
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DenseColumn<MT,false,true>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DenseColumn<MT,false,true>::multAssign( const DenseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const size_t columns( size() );

   const size_t jpos( columns & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( columns - ( columns % (IT::size*4UL) ) ) == jpos, "Invalid end calculation" );

   typename VT::ConstIterator it( (~rhs).begin() );
   for( size_t j=0UL; j<jpos; j+=IT::size*4UL ) {
      matrix_.store( col_, j             , matrix_.load(col_,j             ) * it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size    , matrix_.load(col_,j+IT::size    ) * it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size*2UL, matrix_.load(col_,j+IT::size*2UL) * it.load() ); it += IT::size;
      matrix_.store( col_, j+IT::size*3UL, matrix_.load(col_,j+IT::size*3UL) * it.load() ); it += IT::size;
   }
   for( size_t j=jpos; j<columns; j+=IT::size, it+=IT::size ) {
      matrix_.store( col_, j, matrix_.load(col_,j) * it.load() );
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the multiplication assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >  // Type of the dense matrix
template< typename VT >  // Type of the right-hand side sparse vector
inline void DenseColumn<MT,false,true>::multAssign( const SparseVector<VT,false>& rhs )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_RESTRICTED( MT );

   BLAZE_INTERNAL_ASSERT( size() == (~rhs).size(), "Invalid vector sizes" );

   const ResultType tmp( serial( *this ) );

   reset();

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      matrix_(col_,element->index()) = tmp[element->index()] * element->value();
}
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  DENSECOLUMN OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name DenseColumn operators */
//@{
template< typename MT, bool SO, bool SF >
inline void reset( DenseColumn<MT,SO,SF>& column );

template< typename MT, bool SO, bool SF >
inline void clear( DenseColumn<MT,SO,SF>& column );

template< typename MT, bool SO, bool SF >
inline bool isDefault( const DenseColumn<MT,SO,SF>& column );

template< typename MT, bool SO, bool SF >
inline bool isSame( const DenseColumn<MT,SO,SF>& a, const DenseColumn<MT,SO,SF>& b );
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given dense column.
// \ingroup dense_column
//
// \param column The dense column to be resetted.
// \return void
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline void reset( DenseColumn<MT,SO,SF>& column )
{
   column.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given dense column.
// \ingroup dense_column
//
// \param column The dense column to be cleared.
// \return void
//
// Clearing a dense column is equivalent to resetting it via the reset() function.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline void clear( DenseColumn<MT,SO,SF>& column )
{
   column.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given dense column is in default state.
// \ingroup dense_column
//
// \param column The dense column to be tested for its default state.
// \return \a true in case the given dense column is component-wise zero, \a false otherwise.
//
// This function checks whether the dense column is in default state. For instance, in case the
// column is instantiated for a built-in integral or floating point data type, the function
// returns \a true in case all column elements are 0 and \a false in case any column element
// is not 0. The following example demonstrates the use of the \a isDefault function:

   \code
   blaze::DynamicMatrix<int,columnMajor> A;
   // ... Resizing and initialization
   if( isDefault( column( A, 0UL ) ) ) { ... }
   \endcode
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline bool isDefault( const DenseColumn<MT,SO,SF>& column )
{
   for( size_t i=0UL; i<column.size(); ++i )
      if( !isDefault( column[i] ) ) return false;
   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the two given dense columns represent the same observable state.
// \ingroup dense_column
//
// \param a The first dense column to be tested for its state.
// \param b The second dense column to be tested for its state.
// \return \a true in case the two columns share a state, \a false otherwise.
//
// This overload of the isSame function tests if the two given dense columns refer to exactly the
// same range of the same dense matrix. In case both columns represent the same observable state,
// the function returns \a true, otherwise it returns \a false.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline bool isSame( const DenseColumn<MT,SO,SF>& a, const DenseColumn<MT,SO,SF>& b )
{
   return ( isSame( a.matrix_, b.matrix_ ) && ( a.col_ == b.col_ ) );
}
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Removal of all restrictions on the data access to the given dense column.
// \ingroup dense_column
//
// \param column The dense column to be derestricted.
// \return Dense column without access restrictions.
//
// This function removes all restrictions on the data access to the given dense column. It returns
// a column object that does provide the same interface but does not have any restrictions on the
// data access.\n
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in the violation of invariants, erroneous results and/or in compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO      // Storage order
        , bool SF >    // Symmetry flag
inline typename DerestrictTrait< DenseColumn<MT,SO,SF> >::Type
   derestrict( DenseColumn<MT,SO,SF>& column )
{
   typedef typename DerestrictTrait< DenseColumn<MT,SO,SF> >::Type  ReturnType;
   return ReturnType( derestrict( column.matrix_ ), column.col_ );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISRESTRICTED SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool SO, bool SF >
struct IsRestricted< DenseColumn<MT,SO,SF> > : public If< IsRestricted<MT>, TrueType, FalseType >::Type
{
   enum { value = IsRestricted<MT>::value };
   typedef typename If< IsRestricted<MT>, TrueType, FalseType >::Type  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DERESTRICTTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool SO, bool SF >
struct DerestrictTrait< DenseColumn<MT,SO,SF> >
{
   typedef DenseColumn< typename RemoveReference< typename DerestrictTrait<MT>::Type >::Type >  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SUBVECTORTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool SO, bool SF >
struct SubvectorTrait< DenseColumn<MT,SO,SF> >
{
   typedef typename SubvectorTrait< typename DenseColumn<MT,SO,SF>::ResultType >::Type  Type;
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
