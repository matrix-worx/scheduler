/*************************************************************************
 * scheduler
 *************************************************************************
 * @file    CComputationNode.cpp
 * @date    16.05.15
 * @author  Hlieb Romanov <rgewebppc@gmail.com>
 * @brief   CComputationNode class definition
 ************************************************************************/
/** @addtogroup scheduler Distributed operations scheduler
 *  @{
 */
#include "CComputationNode.hpp"

#include <algorithm>
#include <strstream>
#include <utility>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/lexical_cast.hpp>

#include <jsoncpp/include/json/json.h>

#include <stdexcept>

using boost::asio::ip::tcp;

CComputationNode::CComputationNode( void )
   : mHost()
   , mIsValid( false )
{

}

CComputationNode::CComputationNode( const std::string& host )
   : mHost( host )
   , mIsValid( true )
{

}

CComputationNode::~CComputationNode( void )
{

}

std::string CComputationNode::getName( void ) const
{
   return mHost;
}

bool CComputationNode::isValid( void ) const
{
   return mIsValid;
}

/**
 * @brief Helper function that performs HTTP request to remote service
 * @param host - host name
 * @param uri - URI of remote REST method
 * @param param - DoubleArray for passing to the remote service
 * @return Calculation result from remote service
 */
static DoubleArray asyncRequest( std::string host, std::string uri, DoubleArray param )
{

   using boost::adaptors::transformed;
   using boost::algorithm::join;

   std::stringstream ss;

   ss << "["
      << join( param |
                  transformed( static_cast<std::string(*)(long double)>(std::to_string) ),
                  ", " )
      << "]";

   std::string jsonString = ss.str();

   boost::asio::io_service io_service;

   // Get a list of endpoints corresponding to the server name.
   tcp::resolver resolver( io_service );
   tcp::resolver::query query( host, boost::lexical_cast<std::string>(8080) );
   tcp::resolver::iterator endpoint_iterator = resolver.resolve( query );

   // Try each endpoint until we successfully establish a connection.
   tcp::socket socket( io_service );
   boost::asio::connect( socket, endpoint_iterator );

   // Form the request. We specify the "Connection: close" header so that the
   // server will close the socket after transmitting the response. This will
   // allow us to treat all data up until the EOF as the content.
   boost::asio::streambuf request;
   std::ostream request_stream( &request );
   request_stream << "POST " << uri << " HTTP/1.0\r\n";
   request_stream << "Host: " << host << "\r\n";
   request_stream << "Accept: application/json\r\n";
   request_stream << "Content-Type: application/json\r\n";
   request_stream << "Content-Length: " << jsonString.size() << "\r\n";
   request_stream << "Connection: close\r\n\r\n";
   request_stream << jsonString;
   // Send the request.
    boost::asio::write( socket, request );

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until( socket, response, "\r\n" );

    // Check that response is OK.
    std::istream response_stream( &response );
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline( response_stream, status_message );
    if ( !response_stream || http_version.substr( 0, 5 ) != "HTTP/" )
    {
      throw std::runtime_error( "Invalid HTTP response" );

    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until( socket, response, "\r\n\r\n" );

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while ( boost::asio::read( socket, response,
          boost::asio::transfer_at_least( 1 ), error ) );

    std::stringstream dataStream;
    dataStream << &response;

    std::vector<std::string> strs;
    boost::algorithm::split_regex( strs, dataStream.str(), boost::regex( "\r\n\r\n" ) );

    DoubleArray resultDoubleArray;

    if ( strs.size() == 2 )
    {

       if ( status_code != 200 )
       {
          throw std::runtime_error( "Wrong request: \n" + strs[1] );
       }

       Json::Reader reader;
       Json::Value resultArray;
       if ( reader.parse( strs[1], resultArray ) )
       {
          if ( resultArray.isArray() )
          {
             std::transform( resultArray.begin(),
                             resultArray.end(),
                             std::back_inserter( resultDoubleArray ),
                             boost::bind( &Json::Value::asDouble, _1 ) );
          }
       }
    }
    else
    {
       throw std::runtime_error( "Wrong request: \n" + dataStream.str() );
    }

    if ( error != boost::asio::error::eof )
    {
      throw boost::system::system_error(error);
    }

   return resultDoubleArray;
}

FutureDoubleArray CComputationNode::asyncMultiplyPairs( const DoubleArray& array ) const
{
   return boost::async( boost::launch::async, boost::bind( asyncRequest, mHost, "/multiply", array ) );
}

FutureDoubleArray CComputationNode::asyncSum( const DoubleArray& array ) const
{
   return boost::async( boost::launch::async, boost::bind( asyncRequest, mHost, "/sum", array ) );
}
/** @}*/

