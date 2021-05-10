 /* Formalidades.
 
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

#include <algorithm>

void pushback_double(double d, double** c, int& size);
void biMatrixNashEquilibria
(
	double** result, int& sizeResult, bool mode,
	double** p1, int sizeY,	double** p2, int sizeX
);


#endif // !NASH_RESOLVER


