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

#include "options.hpp"


class SandpileModel final
{
    friend std::ostream& operator<<( std::ostream& out, const SandpileModel& m );

  private:

    unsigned int size;
    bool pbc;

    std::vector<unsigned int> data;
    unsigned int grain_counter;

    std::mt19937 rng;

    topplemode_t tm;
    void topple_site( unsigned int l );
    bool topple_sync();
    bool topple_async();

  public:

    SandpileModel( unsigned int size_init, bool pbc_init,
                   topplemode_t tm_init, unsigned int rng_seed );

    unsigned int  operator()( unsigned int x, unsigned int y ) const;
    unsigned int& operator()( unsigned int x, unsigned int y );

    void add_grain();
    bool topple();

    double get_density() const;
    double get_active_site_density() const;

    unsigned int get_num_sites() const;
};

std::ostream& operator<<( std::ostream& out, const SandpileModel& m );

#endif // MODEL_H_INCLUDED
