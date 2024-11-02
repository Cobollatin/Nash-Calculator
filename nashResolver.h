/*
   Copyright (C) 2021  Adrian Marquina Vichino

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#ifndef NASH_RESOLVER
#define NASH_RESOLVER

#include <vector>
#include <string>

std::string run_all_tests();
std::string calculate_pure_nash_equilibrium(const std::vector<double>& p1, const std::vector<double>& p2, const size_t& s1, const size_t& s2);
std::string calculate_mixed_nash_equilibrium(const std::vector<double>& p1, const std::vector<double>& p2, const size_t& s1, const size_t& s2);

#endif // !NASH_RESOLVER
