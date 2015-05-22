#ifndef CSANDBOX_HPP
#define CSANDBOX_HPP

#include <vector>

#include <boost/function.hpp>

#include "CMatrix.hpp"
#include "CComputationNode.hpp"

using namespace matrix;

class CSandBox
{
public:
   typedef boost::function<void( bool hasError )> Callback;

   CSandBox( const CMatrix& A, const CMatrix& B, CMatrix& C, const std::vector<CComputationNode>& nodes );

   ~CSandBox( void );

   bool exec( void );

   bool isFinished( void ) const;
   bool hasError( void ) const;

   void terminate( bool hasError );

private:
   const CMatrix& mA;
   const CMatrix& mB;
   CMatrix& mC;
   const std::vector<CComputationNode>& mNodes;

   bool mFinished;
   bool mHasError;

   boost::condition_variable mWaitCondition;
   boost::mutex mWaitGuard;

private:
   static void sandBoxMain( const CMatrix& A,
                            const CMatrix& B,
                            CMatrix& C,
                            const std::vector<CComputationNode>& nodes, Callback terminate );
};

#endif // CSANDBOX_HPP
