#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "CComputationNode.hpp"

/**
  * BLANK ft. JONES - Fallen (With Delerium And Rani)
  */

#include "CMatrix.hpp"
#include "MatrixIO.hpp"

namespace po = boost::program_options;

po::options_description optDescr( "Allowed options" );
po::variables_map options;

void prepareOptionsDescription( void )
{
    optDescr.add_options()
        ( "help,h", "produce help message" )
        ( "matrixA,A", po::value<std::string>(), "binary file with matrix A data" )
        ( "matrixB,B", po::value<std::string>(), "binary file with matrix B data" )
        ( "output,o", po::value<std::string>(), "place result matrix to this binary output file" )
        ( "hosts", po::value<std::string>(), "path to the hosts.json" )
        ( "txt", "produce output in plain text format" )
        ( "bin", "consume input in binary format ( by default assume text format)" );
}

void printHelp( void )
{
    std::cout << optDescr << std::endl;
}

int main( int argc, char* argv[] )
{
   prepareOptionsDescription();

   try
   {
       po::store( po::parse_command_line( argc, argv, optDescr), options );
       po::notify( options );
   }
   catch ( ... )
   {
       std::cout << "Unrecognized options" << std::endl;
       printHelp();
       return -1;
   }

   if ( options.count( "help" ) )
   {
       printHelp();
   }

   CComputationNode node( "127.0.0.1" );
   DoubleArray arr;
   arr.push_back( 1 );
   arr.push_back( 2 );
   arr.push_back( 3 );

   node.asyncSum( arr );

   return 0;
}

