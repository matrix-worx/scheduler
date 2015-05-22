#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <fstream>

#include <boost/program_options.hpp>
#include <jsoncpp/include/json/json.h>

#include "CComputationNode.hpp"
#include "CSandBox.hpp"

#include "CMatrix.hpp"
#include "MatrixIO.hpp"

namespace Parameters
{
    enum cmd_line_params {
        NONE,
        MATRIX_A = 1,
        MATRIX_B = 2,
        OUTPUT = 4,
        HOSTS = 8
    };
}

uint32_t requiredParamsMask = Parameters::MATRIX_A
                              | Parameters::MATRIX_B
                              | Parameters::OUTPUT
                              | Parameters::HOSTS;

namespace po = boost::program_options;

po::options_description optDescr( "Allowed options" );
po::variables_map options;

void prepareOptionsDescription( void )
{
    optDescr.add_options()
        ( "help,h", "show this help message" )
        ( "matrixA,A", po::value<std::string>(), "file with matrix A data (by default assumes text format)" )
        ( "matrixB,B", po::value<std::string>(), "file with matrix B data (by default assumes text format)" )
        ( "output,o", po::value<std::string>(), "place result matrix to this output file (by default - binary format)" )
        ( "hosts", po::value<std::string>(), "path to the json file with computation nodes host names or IPs" )
        ( "otxt", "produce output in plain text format" )
        ( "rbin", "consume input in binary format ( by default assume text format)" );
}


bool parseCommandLineArguments( std::string& matrixAFile,
                                std::string& matrixBFile,
                                std::string& matrixCFile,
                                std::string& hostsFile,
                                bool& isTxtOutput,
                                bool& isConsumeBinary )
{
   uint32_t inputParametersMask = 0;

   if ( options.count( "matrixA" ) )
   {
       matrixAFile.append( options["matrixA"].as<std::string>() );
       inputParametersMask |= Parameters::MATRIX_A;
   }

   if ( options.count( "matrixB" ) )
   {
       matrixBFile.append( options["matrixB"].as<std::string>() );
       inputParametersMask |= Parameters::MATRIX_B;
   }

   if ( options.count( "output" ) )
   {
       matrixCFile.append( options["output"].as<std::string>() );
       inputParametersMask |= Parameters::OUTPUT;
   }

   if ( options.count( "hosts" ) )
   {
       hostsFile.append( options["hosts"].as<std::string>() );
       inputParametersMask |= Parameters::HOSTS;
   }

   if ( options.count( "otxt" ) )
   {
       isTxtOutput = true;
   }

   if ( options.count( "rbin" ) )
   {
       isConsumeBinary = true;
   }

   return ( ( inputParametersMask & requiredParamsMask ) == requiredParamsMask );
}

void printHelp( void )
{
    std::cout << optDescr << std::endl;
}

void populateComputationNodesHelper( std::vector<CComputationNode>& nodes,
                                     Json::Value& jsonValue )
{
   nodes.push_back( CComputationNode( jsonValue.asString() ) );
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
       return 0;
   }

   std::string matrixAFile;
   std::string matrixBFile;
   std::string matrixCFile;
   std::string hostsFile;

   bool isTxtOutput = false;
   bool isConsumeBinary = false;

   if( !parseCommandLineArguments( matrixAFile,
                                   matrixBFile,
                                   matrixCFile,
                                   hostsFile,
                                   isTxtOutput,
                                   isConsumeBinary ) )
   {
      std::cout << "Error. Wrong options were passed." << std::endl;
      printHelp();
      return -1;
   }

   matrix::CMatrix matrixA;
   matrix::CMatrix matrixB;
   matrix::CMatrix matrixC;

   if ( isConsumeBinary )
   {
      if ( !matrix::io::readFromBinFile( matrixAFile, matrixA ) )
      {
         std::cout << "Error while reading matrix A from binary file." << std::endl;
         return -1;
      }
      if ( !matrix::io::readFromBinFile( matrixBFile, matrixB ) )
      {
         std::cout << "Error while reading matrix B from binary file." << std::endl;
         return -1;
      }
   }
   else
   {
      if ( !matrix::io::readFromTextFile( matrixAFile, matrixA ) )
      {
         std::cout << "Error while reading matrix A from text file." << std::endl;
         return -1;
      }
      if ( !matrix::io::readFromTextFile( matrixBFile, matrixB ) )
      {
         std::cout << "Error while reading matrix B from text file." << std::endl;
         return -1;
      }
   }
   std::cout << "Input matrices were read successfully." << std::endl;

   Json::Reader reader;
   Json::Value hostsArray;

   std::ifstream fileStream( hostsFile.c_str() );

   if ( !reader.parse( fileStream, hostsArray ) )
   {
      std::cout << "Error while reading hosts json file." << std::endl;
      return -1;
   }

   fileStream.close();

   std::vector<CComputationNode> compNodes;

   std::for_each( hostsArray.begin(),
                  hostsArray.end(),
                  boost::bind( &populateComputationNodesHelper, boost::ref( compNodes ), _1 ) );

   CSandBox sandBox( matrixA, matrixB, matrixC, compNodes );

   if ( sandBox.exec() )
   {
      std::cout << "SandBox was finished successfully" << std::endl;
      if ( isTxtOutput )
      {
         matrix::io::writeToTextFile( matrixCFile, matrixC );
      }
      else
      {
         matrix::io::writeToBinFile( matrixCFile, matrixC );
      }
   }
   else
   {
      std::cout << "SandBox was finished with error" << std::endl;
   }

   return 0;
}
