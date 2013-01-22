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

#include <iostream>

#include "options.hpp"

using namespace std;


int main( const int argc, const char* argv[] )
{
  cout << "    ==========================================" << endl;
  cout << "    | YASS - Yet Another Sandpile Simulation |" << endl;
  cout << "    ==========================================" << endl;
  cout << endl;

  const Options& opts = read_options( argc, argv );

  if ( opts["mode"].as<simmode_t>() == MODE_CONST ) {
    cout << ":: Running constant density simulation" << endl;

  } else { /* MODE_DROP */
    cout << ":: Running dropping sand simulation" << endl;

  }

  return 0;
}
