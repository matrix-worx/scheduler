#include "CComputationNode.hpp"

#include <algorithm>
#include <strstream>
#include <utility>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/lexical_cast.hpp>

using boost::asio::ip::tcp;

/**
 * @brief CComputationNode::CComputationNode
 * MORCHEEBA - Enjoy The Ride
 * KARMON - Take My Hand feat. Terry Shand
 */
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

static DoubleArray asyncRequest( std::string host, std::string uri, DoubleArray param )
{

   using boost::adaptors::transformed;
   using boost::algorithm::join;

   std::stringstream ss;

   ss << "["
      << join( param |
                  transformed( static_cast<std::string(*)(double)>(std::to_string) ),
                  ", " )
      << "]";

   std::string jsonString = ss.str();

   try
   {
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
         std::cout << "Invalid response\n";
         return DoubleArray();
       }
       if ( status_code != 200 )
       {
         std::cout << "Response returned with status code " << status_code << "\n";
         return DoubleArray();
       }

       // Read the response headers, which are terminated by a blank line.
       boost::asio::read_until( socket, response, "\r\n\r\n" );

       // Read until EOF, writing data to output as we go.
       boost::system::error_code error;
       while ( boost::asio::read( socket, response,
             boost::asio::transfer_at_least( 1 ), error ) );

       std::cout << &response;

       if (error != boost::asio::error::eof)
         throw boost::system::system_error(error);
   }
   catch (std::exception& e)
   {
    std::cout << "Exception: " << e.what() << "\n";
   }
   return DoubleArray();
}

FutureDoubleArray CComputationNode::asyncMultiplyPairs( const DoubleArray& array )
{

}

void CComputationNode::asyncSum( const DoubleArray& array )
{
   asyncRequest( mHost, "/sum", array );
   //return boost::async( boost::bind( asyncRequest, mHost, "/sum", array ) );
}



