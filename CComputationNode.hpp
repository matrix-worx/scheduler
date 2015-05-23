/*************************************************************************
 * scheduler
 *************************************************************************
 * @file    CComputationNode.hpp
 * @date    16.05.15
 * @author  Hlieb Romanov <rgewebppc@gmail.com>
 * @brief   CComputationNode class declaration
 ************************************************************************/
#ifndef CCOMPUTATIONNODE_HPP
#define CCOMPUTATIONNODE_HPP

/** @addtogroup scheduler Distributed operations scheduler
 *  @{
 */
#define BOOST_THREAD_PROVIDES_FUTURE

#include <string>
#include <list>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>

/**
 * @brief Vector of double values
 */
typedef std::vector<double> DoubleArray;

/**
 * @brief Async result type of CComputationNode methods
 * @sa CComputationNode::asyncMultiplyPairs()
 * @sa CComputationNode::asyncSum()
 */
typedef boost::future<DoubleArray> FutureDoubleArray;

/**
 * @brief This class represents remote web service \n
 * that provides two operations on DoubleArray: \n
 * multiply pairs of numbers and sum all numbers in the array.
 */
class CComputationNode
{
public:
   /**
    * @brief Default constructor. Creates invalid object
    */
   CComputationNode( void );

   /**
    * @brief Constructor. Initialize object with remote service host name.
    * @param host - remote service host name
    */
   explicit CComputationNode( const std::string& host );

   /**
    * @brief Destructor
    */
   ~CComputationNode( void );

   /**
    * @brief Get computation node host name
    * @return
    */
   std::string getName( void ) const;

   /**
    * @brief Determine whether this object is valid or not
    * @return True - if this is valid object, false - otherwise
    */
   bool isValid( void ) const;

   /**
    * @brief Asynchronously multiply pairs of numbers from passed array
    * @param array - DoubleArray with numbers to multiply
    * @return Async multiplication result
    */
   FutureDoubleArray asyncMultiplyPairs( const DoubleArray& array ) const;

   /**
    * @brief Asynchronously sum all numbers from passed array
    * @param array - DoubleArray with numbers to sum
    * @return Async sum result
    */
   FutureDoubleArray asyncSum( const DoubleArray& array ) const;

private:
   std::string mHost; ///< Remote service host name
   bool mIsValid;     ///< Valid/Invalid flag
};
/** @}*/
#endif // CCOMPUTATIONNODE_HPP
