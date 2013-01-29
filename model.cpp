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

#include "model.hpp"

#include <assert.h>
#include <numeric>
#include <algorithm>
#include <iostream>
using namespace std;


SandpileModel::SandpileModel(
  unsigned int size_init, bool pbc_init, unsigned int rng_seed )
  : size( size_init ), pbc( pbc_init ),
    data( vector<unsigned int>( size_init* size_init, 0 ) ),
    rng( mt19937( rng_seed) ) { }


unsigned int SandpileModel::operator()( unsigned int x, unsigned int y ) const
{
  assert( y < size );
  assert( x < size );
  return data[ size * y + x ];
}


unsigned int& SandpileModel::operator()( unsigned int x, unsigned int y )
{
  assert( y < size );
  assert( x < size );
  return data[ size * y + x ];
}


void SandpileModel::add_grain() {
  data.at(
    uniform_int_distribution<unsigned int>(0, data.size() - 1 )( rng )
  ) += 1;
}


bool SandpileModel::topple()
{
  bool toppling_occurred = false;
  vector<unsigned int> toppling_sites;
  toppling_sites.reserve( size * size );

  // find toppling sites
  for ( unsigned int i = 0; i < size * size; ++i ) {
    if ( data.at( i ) >= 4 ) {
      toppling_sites.push_back( i );
      toppling_occurred = true;
    }
  }

  // let them topple
  for ( auto ts = toppling_sites.begin(); ts != toppling_sites.end(); ++ts ) {
    unsigned int x = *ts % size;
    unsigned int y = *ts / size;

    //while ( data[ *ts ] >= 4 ) {
      data[ *ts ] -= 4;

      // bottom neighbor
      if ( y == 0 ) {
        if ( pbc ) {
          operator()( x, size - 1 ) += 1;
        }
      } else {
        operator()( x, y - 1 ) += 1;
      }

      // right neighbor
      if ( x == size - 1 ) {
        if ( pbc ) {
          operator()( 0, y ) += 1;
        }
      } else {
        operator()( x + 1, y ) += 1;
      }

      // top neighbor
      if ( y == size - 1 ) {
        if ( pbc ) {
          operator()( x, 0 ) += 1;
        }
      } else {
        operator()( x, y + 1 ) += 1;
      }

      // left neighbor
      if ( x == 0 ) {
        if ( pbc ) {
          operator()( size - 1, y ) += 1;
        }
      } else {
        operator()( x - 1, y ) += 1;
      }
    //}
  }

  return toppling_occurred;
}


double SandpileModel::get_density() const
{
  return
    static_cast<double>( accumulate( data.begin(), data.end(), 0 ) ) /
    static_cast<double>( size * size );
}


double SandpileModel::get_active_site_density() const
{
  return
    static_cast<double>(
      count_if(
        data.begin(), data.end(),
        []( unsigned int h ) { return h >= 4; }
      )
    ) / static_cast<double>( size * size );
}


unsigned int SandpileModel::get_num_sites() const
{
  return size * size;
}


ostream& operator<<( ostream& out, const SandpileModel& m )
{
  for ( unsigned int y = 0; y < m.size; ++y ) {
    for ( unsigned int x = 0; x < m.size; ++x ) {
      out << m( x, y ) << ' ';
    }
    out << '\b' <<endl;
  }
  return out;
}
