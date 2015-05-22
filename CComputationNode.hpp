#ifndef CCOMPUTATIONNODE_HPP
#define CCOMPUTATIONNODE_HPP

#define BOOST_THREAD_PROVIDES_FUTURE

#include <string>
#include <list>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>

typedef std::vector<double> DoubleArray;

typedef boost::future<DoubleArray> FutureDoubleArray;

class CComputationNode
{
public:
   CComputationNode( void );
   explicit CComputationNode( const std::string& host );
   ~CComputationNode( void );

   std::string getName( void ) const;
   bool isValid( void ) const;

   FutureDoubleArray asyncMultiplyPairs( const DoubleArray& array ) const;
   FutureDoubleArray asyncSum( const DoubleArray& array ) const;

private:
   std::string mHost;
   bool mIsValid;
};

#endif // CCOMPUTATIONNODE_HPP
