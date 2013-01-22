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

#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <vector>
#include <random>
#include <iosfwd>


class SandpileModel final
{
    friend std::ostream& operator<<( std::ostream& out, const SandpileModel& m );

  private:

    unsigned int size;
    bool pbc;

    std::vector<unsigned int> data;

    std::mt19937 rng;

  public:

    SandpileModel( unsigned int size_init, bool pbc_init, unsigned int rng_seed );

    unsigned int  operator()( unsigned int x, unsigned int y ) const;
    unsigned int& operator()( unsigned int x, unsigned int y );

    void add_grain();
    bool topple();

    float get_density() const;
};

std::ostream& operator<<( std::ostream& out, const SandpileModel& m );

#endif // MODEL_H_INCLUDED
