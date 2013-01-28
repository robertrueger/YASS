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
  bool verbose              = opts.count( "verbose" );
  unsigned int size         = opts["size"].as<unsigned int>();
  float density             = opts["density"].as<float>();
  unsigned int rng_seed     = opts["seed"].as<unsigned int>();
  unsigned int mcs_equil    = opts["mcs-equil"].as<unsigned int>();
  unsigned int mcs_measure  = opts["mcs-measure"].as<unsigned int>();

  cout << "-> Creating the model" << endl;
  SandpileModel m( size, true, rng_seed );
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Filling to const density" << endl;
  while ( m.get_density() < density ) {
    m.add_grain();
  }
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Topple a little bit" << endl;
  for ( unsigned int i = 0; i < mcs_equil; ++i ) {
    if ( m.topple() == false ) {
      break;
    }
  }
  if ( verbose ) {
    cout << m << endl;
  }

  cout << "-> Measure the density of active sites while toppling" << endl;
  float asd_sum = 0.f;
  for ( unsigned int i = 0; i < mcs_measure; ++i ) {
    asd_sum += m.get_active_site_density();
    if ( m.topple() ==  false ) {
      asd_sum = 0;
      break;
    }
  }
  float asd = asd_sum / static_cast<float>( mcs_measure );

  cout << "-> Result:" << endl;
  cout << density << " " << asd << endl;
}



void simrun_drop(  const Options& opts )
{

}
