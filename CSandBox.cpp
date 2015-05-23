/** @addtogroup scheduler Distributed operations scheduler
 *  @{
 */
#include <iostream>

#include "CSandBox.hpp"

CSandBox::CSandBox( const CMatrix& A, const CMatrix& B, CMatrix& C, const std::vector<CComputationNode>& nodes )
   : mA( A )
   , mB( B )
   , mC( C )
   , mNodes( nodes )
   , mFinished( false )
   , mHasError( false )
{

}

CSandBox::~CSandBox( void )
{

}

bool CSandBox::exec( void )
{
   Callback terminateCallback = boost::bind( &CSandBox::terminate, this, _1 );

   boost::thread sandboxThread( &CSandBox::sandBoxMain,
                                boost::ref( mA ),
                                boost::ref( mB ),
                                boost::ref( mC ),
                                boost::ref( mNodes ),
                                terminateCallback );


   sandboxThread.detach();

   boost::unique_lock<boost::mutex> lock( mWaitGuard );
   mWaitCondition.wait( lock, boost::bind( &CSandBox::isFinished, this ) );

   return !mHasError;
}

bool CSandBox::isFinished( void ) const
{
   return mFinished;
}

bool CSandBox::hasError( void ) const
{
   return mHasError;
}

void CSandBox::terminate( bool hasError )
{
   mHasError = hasError;

   {
      boost::lock_guard<boost::mutex> lock( mWaitGuard );
      mFinished = true;
   }

   mWaitCondition.notify_all();
}

void CSandBox::sandBoxMain( const CMatrix& A,
                            const CMatrix& B,
                            CMatrix& C,
                            const std::vector<CComputationNode>& nodes,
                            CSandBox::Callback terminate )
{
   std::cout << "Hello from detached thread" << std::endl;
   C = A;

   const CComputationNode& node = nodes[0];

   DoubleArray array;
   array.push_back( A( 0, 0 ) );
   array.push_back( A( 1, 0 ) );
   array.push_back( A( 2, 0 ) );
   array.push_back( A( 3, 0 ) );

   FutureDoubleArray result = node.asyncSum( array );
   try
   {
      DoubleArray resultArray = result.get();

      std::cout << resultArray.size() << std::endl;
      std::cout << resultArray[0] << std::endl;
   }
   catch( ... )
   {
      std::cout << "Something goes wrong here. Exception was thrown." << std::endl;
   }

   terminate( false );
}
/** @}*/

