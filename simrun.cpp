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

#include "simrun.hpp"
#include "model.hpp"

#include <iostream>
using namespace std;


void simrun_const( const Options& opts )
{
  bool verbose = opts.count( "verbose" );

  cout << "-> Creating the model" << endl;
  SandpileModel m( opts["size"].as<unsigned int>(), true,
                   opts["toppling"].as<topplemode_t>(),
                   opts["seed"].as<unsigned int>() );
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Filling to const density" << endl;
  while ( m.get_density() < opts["density"].as<double>() ) {
    m.add_grain();
  }
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Topple a little bit" << endl;
  for ( unsigned int i = 0; i < opts["mcs-equil"].as<unsigned int>(); ++i ) {
    if ( m.topple() == false ) {
      break;
    }
  }
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Measure the density of active sites while toppling" << endl;
  double asd_sum = 0.f;
  for ( unsigned int i = 0; i < opts["mcs-measure"].as<unsigned int>(); ++i ) {
    asd_sum += m.get_active_site_density();
    if ( m.topple() ==  false ) {
      asd_sum = 0;
      break;
    }
  }
  if ( verbose ) {
    cout << m << endl;
  }

  double asd =
    asd_sum / static_cast<double>( opts["mcs-measure"].as<unsigned int>() );

  cout << "-> Result:" << endl;
  cout << opts["density"].as<double>() << " " << asd << endl;
}



void simrun_drop(  const Options& opts )
{
  bool verbose = opts.count( "verbose" );

  cout << "-> Creating the model" << endl;
  SandpileModel m( opts["size"].as<unsigned int>(), false,
                   opts["toppling"].as<topplemode_t>(),
                   opts["seed"].as<unsigned int>() );
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Add grain + topple to equilibrate" << endl;
  for ( unsigned int s = 0; s < opts["mcs-equil"].as<unsigned int>(); ++s ) {
    for ( unsigned int i = 0; i < m.get_num_sites(); ++i ) {
      m.add_grain();
      while ( m.topple() == true ) { };
    }
  }
  cout << "Density after equilibration: " << m.get_density() << endl;
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Add grain + topple to measure the density" << endl;
  double dens_sum = 0.f;
  for ( unsigned int s = 0; s < opts["mcs-measure"].as<unsigned int>(); ++s ) {
    for ( unsigned int i = 0; i < m.get_num_sites(); ++i ) {
      m.add_grain();
      while ( m.topple() == true ) { };
      dens_sum += m.get_density();
    }
  }
  if ( verbose ) {
    cout << m << endl;
  }

  double dens
    = dens_sum / static_cast<double>(
        opts["mcs-measure"].as<unsigned int>() * m.get_num_sites()
      );

  cout << "-> Result:" << endl;
  cout << dens << endl;
}
