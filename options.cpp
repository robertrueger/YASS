/*
 * Copyright (c) 2013, Robert Rueger <rueger@itp.uni-frankfurt.de>
 *
 * This file is part of YASS.
 *
 * YASS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YASS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YASS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "options.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
namespace po  = boost::program_options;
namespace fs  = boost::filesystem;


Options read_options( const int argc, const char* argv[] )
{
  Options vm;

  // define command line only options

  po::options_description clionly( "command line options" );
  clionly.add_options()
  ( "help,h", "print this help message and exit" )
  ( "version,V", "print YASS's version and exit" )
  ( "verbose,v", "makes YASS write additional information to stdout" )
  ( "job-file,J", po::value<fs::path>(), "job file to execute" );
  po::positional_options_description p;
  p.add( "job-file", -1 );


  // define command line and jobfile options

  po::options_description allset( "physical parameters" );
  allset.add_options()

  ( "mode,m",
    po::value<simmode_t>()->required(),
    "simulation mode (const, drop)" )

  ( "toppling,t",
    po::value<topplemode_t>()->default_value( TOPPLE_SYNC, "sync" ),
    "toppling mode (async, sync)" )

  ( "size,s",
    po::value<unsigned int>()->required(),
    "size of the simulated lattice" )

  ( "seed,S",
    po::value<unsigned int>()->default_value(
      chrono::system_clock::now().time_since_epoch().count()
    ),
    "random number generator seed" )

  ( "mcs-equil,E",
    po::value<unsigned int>()->required(),
    "number of Monte Carlo steps for equilibration" )

  ( "mcs-measure,M",
    po::value<unsigned int>()->required(),
    "number of measurement bins" );

  po::options_description constset( "settings for constant density simulations" );
  constset.add_options()

  ( "density,d",
    po::value<double>(),
    "constant sand density" );


  // define option groups for cli and jobfile
  po::options_description cmdline_options;
  cmdline_options.add( clionly ).add( allset ).add( constset );
  po::options_description jobfile_options;
  jobfile_options.add( allset ).add( constset );


  try {
    // parse options from the command line
    cout << ":: Parsing command line ..." << endl;
    po::store( po::command_line_parser( argc, argv ).
               options( cmdline_options ).positional( p ).run(), vm );
  } catch ( const po::error& e ) {
    cerr << "Error while parsing the command line: " << e.what() << endl;
    exit( 1 );
  }


  // display help or YASS version information

  if ( vm.count( "help" ) ) {
    cout << endl;
    cout << "usage: yass [OPTIONS] JOBFILE -o OUTDIR" << endl;
    cout << cmdline_options << endl;
    exit( 0 );
  }

  if ( vm.count( "version" ) ) {
    cout << endl;

    cout << "yass - built from git commit " << GIT_HASH << endl;

#ifndef NDEBUG
    cout << "=========== DEBUG BUILD ============" << endl;
#else
    cout << "========== RELEASE BUILD ===========" << endl;
#endif

    cout << "compiled " << __DATE__ " with ";
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#  if defined(__GNUC_PATCHLEVEL__)
    cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__
         << "." << __GNUC_PATCHLEVEL__;
#  else
    cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__;
#  endif
#elif defined(__INTEL_COMPILER)
    cout << "ICC " << __INTEL_COMPILER;
#elif defined(_MSC_VER)
    cout << "MSC " << _MSC_VER;
#else
    cout << "unknown compiler";
#endif
    cout << endl << endl;

    cout
        << "Copyright (C) 2013, Robert Rueger <rueger@itp.uni-frankfurt.de>" << endl
        << "License GPLv3+: GNU GPL version 3 or later"
        " <http://gnu.org/licenses/gpl.html>" << endl
        << "This is free software: you are free to change and redistribute it." << endl
        << "There is NO WARRANTY, to the extent permitted by law." << endl << endl;

    exit( 0 );
  }


  if ( vm.count( "job-file" ) ) {
    // parse the jobfile
    cout << ":: Parsing jobfile ..." << endl;
    ifstream jobifs( vm["job-file"].as<fs::path>().string() );
    if ( jobifs.is_open() ) {
      try {
        po::store( po::parse_config_file( jobifs, jobfile_options ), vm );
      } catch ( const po::error& e ) {
        cerr << "Error while parsing the job file: " << e.what() << endl;
        exit( 1 );
      }
    } else {
      cerr << "Error: unable to open jobfile " << vm["job-file"].as<string>()
           << endl;
      exit( 1 );
    }
  }

  try {
    // finalize the variable map
    // (will throw exception on missing options)
    po::notify( vm );
  } catch ( const po::error& e ) {
    cerr << "Error in program options: " << e.what() << endl;
    exit( 1 );
  }


  // check for logical errors in the settings
  try {

    if ( vm["mode"].as<simmode_t>() == MODE_CONST ) {

      if ( vm.count( "density" ) == 0 ) {
        throw logic_error( "no density specified in constant density mode" );
      }

      if ( vm["density"].as<double>() < 0.f ) {
        throw logic_error( "density must be positive" );
      }

    }

  } catch ( const logic_error& e ) {
    cerr << "Logical error in settings: " << e.what() << endl;
    exit( 1 );
  }

  return vm;
}


istream& operator>>( std::istream& in, simmode_t& mode )
{
  string token;
  in >> token;
  if ( token == "const" ) {
    mode = MODE_CONST;
  } else if ( token == "drop" ) {
    mode = MODE_DROP;
  } else {
    throw po::validation_error( po::validation_error::invalid_option_value );
  }
  return in;
}


istream& operator>>( std::istream& in, topplemode_t& tmode )
{
  string token;
  in >> token;
  if ( token == "sync" ) {
    tmode = TOPPLE_SYNC;
  } else if ( token == "async" ) {
    tmode = TOPPLE_ASYNC;
  } else {
    throw po::validation_error( po::validation_error::invalid_option_value );
  }
  return in;
}
