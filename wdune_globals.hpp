/*
wdune: This is an accessible and freely available interpretation of a cellular automata
simulation program for sand dunes. Please note that the random number generator
has a different license than this program, see file in this directory: 'mersenne_twister.h'.

Copyright (C) 2011 Thomas E. Barchyn, Chris H. Hugenholtz
Contact: tom.barchyn@uleth.ca, +1 (403) 332-4043

License:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Credits:
This program is further detailed in a accompanying publication. The code is an
interpretation of a simulation algorithm first described in the following publication:

Werner, B.T., 1995. Eolian dunes: Computer simulations and attractor interpretation.
Geology 23, 1107-1110. DOI: 10.1130/0091-7613(1995)023<1107:EDCSAA>2.3.CO;2

If you are using this program for research, we would appreciate citation of
both papers.

Notes:
This program is written in C/C++ and has been compiled successfully with GCC 4.4.1 in
both Windows (XP, Vista, 7) and Linux (Ubuntu 11.04). We have used the following compiler
flags: -Wall -pedantic -O1. The program will function on some systems with higher optimization
but we have encountered problems in some cases with -O2 and -O3.

This program is designed to be called exclusively from a Python script as a long string
of arguments need to be passed to the executable. The idea being that the Python script
can easily be modified for batch operation, etc. Please contact Tom Barchyn for further
assistance if you wish to extend the program (tom.barchyn@uleth.ca).
*/

// Global variables
// constants
const int maxNrow = 5000;
const int maxNcol = 5000;
const int avalanche_thresh = 5;         
/* 
Avalanche threshold is set as constant in this implementation
with each slab set to a height of 0.1 cell width units
30 degrees is roughly equal to 0.57 cell width units. Thus, the
maximum permissible slope is set to 5 slabs, 6 slabs will
avalanche.
*/

// model parameters
int numIterations, bound_type, wdir, depjump, ncols, nrows;
double dropdist, psand, pnosand;
int newSandCode, newSandSlabs;

// model operational variables
int i_n[maxNrow], i_s[maxNrow], j_e[maxNcol], j_w[maxNcol];     // adjacent coordinate lookups
int i_dp[maxNrow], j_dp[maxNcol];                               // deposition coordinate lookups
int i_ero, j_ero, i_depo, j_depo;                               // erosion and deposition coordinates
int shadloops;                                                  // number of loops the shadow updater performs
bool ero_flag;                                                  // flag to indicate that erosion is happening
int slabs_out = 0;                                              // number of slabs that fall of the edges
int t = 0;                                                      // main iteration counter

// model arrays
int surf [maxNrow][maxNcol];
int bsmt [maxNrow][maxNcol];

// wind shadow height
double shad [maxNrow][maxNcol];

// toxic coordinates: program will not deposit sand in these sites (effectively removing sand from modelspace)
int i_toxic = -1;               
int j_toxic = -1;              
/* 
These act as flags to trigger the deposition program to take the sand
out of the modelspace and are only used with non-periodic boundaries.
*/