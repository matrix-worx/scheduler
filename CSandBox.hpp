/*************************************************************************
 * scheduler
 *************************************************************************
 * @file    CSandBox.hpp
 * @date    23.05.15
 * @author  Hlieb Romanov <rgewebppc@gmail.com>
 * @brief   CSandBox class declaration
 ************************************************************************/
#ifndef CSANDBOX_HPP
#define CSANDBOX_HPP
/** @addtogroup scheduler Distributed operations scheduler
 *  @{
 */
#include <vector>

#include <boost/function.hpp>

#include "CMatrix.hpp"
#include "CComputationNode.hpp"

using namespace matrix;

/**
 * @brief This class represents environment where student \n
 * can perform distributed operations on input matrices and \n
 * store result in output matrix.
 * @sa CSandBox::sandBoxMain()
 */
class CSandBox
{
public:
   /**
    * @brief Termination callback type
    */
   typedef boost::function<void( bool hasError )> Callback;

   /**
    * @brief Constructor. Initialize all internal references.
    * @param A - input matrix A
    * @param B - input matrix B
    * @param[out] C - output matrix C (here computation result will be stored)
    * @param nodes - a list of available computation nodes
    */
   CSandBox( const CMatrix& A,
             const CMatrix& B,
             CMatrix& C,
             const std::vector<CComputationNode>& nodes );

   ~CSandBox( void );

   /**
    * @brief Execute main sandbox function in separate thread and wait \n
    * until terminate() will be called.
    * @return True - if main sandbox function was finished successfully, false - otherwise
    * @sa sandBoxMain();
    */
   bool exec( void );

   /**
    * @brief Check whether main snadbox function was finished or not.
    * @return True if main sandbox function was finished.
    */
   bool isFinished( void ) const;

   /**
    * @brief Check whether main sandbox function was finished with error or not.
    * @return True if main sandbox function was finished with error.
    */
   bool hasError( void ) const;

   /**
    * @brief This method unblocks main application thread, and then application exits.
    * @remark This method should be called from main sandbox function.
    * @param hasError - whether sandbox function was finished with error or not
    * @sa sandBoxMain();
    */
   void terminate( bool hasError );

private:

   const CMatrix& mA;                              ///< Input matrix A
   const CMatrix& mB;                              ///< Input matrix B
   CMatrix& mC;                                    ///< Input matrix C
   const std::vector<CComputationNode>& mNodes;    ///< Computation nodes list

   bool mFinished;                                 ///< Sandbox finished flag
   bool mHasError;                                 ///< Sandbox error flag

   boost::condition_variable mWaitCondition;       ///< Conditional variable to perform wait for sandbox function exit
   boost::mutex mWaitGuard;                        ///< Mutex for conditional variable

private:
   /**
    * @brief Main sandbox function. \n
    * Student should implement this function according to his or her own task. \n
    * This function provides all necessary objects to perform distributed operations on matrices. \n
    * Student should call terminate callback in order to finish scheduler execution and save result matrix to file.
    * @param A - input matrix A
    * @param B - input matrix B
    * @param[out] C - result matrix C which will be saved to file after terminate
    * @param nodes - computation nodes list
    * @param terminate - terminate callback
    * @sa terminate();
    */
   static void sandBoxMain( const CMatrix& A,
                            const CMatrix& B,
                            CMatrix& C,
                            const std::vector<CComputationNode>& nodes, Callback terminate );
};
/** @}*/
#endif // CSANDBOX_HPP
