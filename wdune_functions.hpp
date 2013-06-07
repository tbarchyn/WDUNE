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

// Core functions for Werner Dune

void periodic_bounds()              // set periodic boundaries
{
    // setup the lookup arrays for the periodic boundaries
    for (int i = 0; i < nrows; i++)
    {
        i_n[i] = i - 1;
        i_s[i] = i + 1;
    }
    i_n[0] = (nrows - 1);
    i_s[nrows - 1] = 0;

    for (int j = 0; j < ncols; j++)
    {
        j_e[j] = j + 1;
        j_w[j] = j - 1;
    }
    j_w[0] = (ncols - 1);
    j_e[ncols - 1] = 0;

    // setup deposition coordinates
    if (wdir == 1) // northerly: periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i + depjump) < nrows)  // if still within range, set shift
            {
                i_dp[i] = i + depjump;
            }
            else                        // else write from the beginning again
            {
                i_dp[i] = i + depjump - nrows;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 2;
    }

    if (wdir == 2) // southerly: periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i - depjump) >= 0)  // if still within range, set shift
            {
                i_dp[i] = i - depjump;
            }
            else                     // else write from the beginning again
            {
                i_dp[i] = nrows + i - depjump;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 2;
    }

    if (wdir == 3) // easterly: periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j - depjump) >= 0)  // if still within range, set shift
            {
                j_dp[j] = j - depjump;
            }
            else        // else write from the beginning again
            {
                j_dp[j] = ncols + j - depjump;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 2;

    }

    if (wdir == 4) // westerly: periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j + depjump) < ncols)  // if still within range, set shift
            {
                j_dp[j] = j + depjump;
            }
            else        // else write from the beginning again
            {
                j_dp[j] = j + depjump - ncols;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 2;
    }
}

void nonperiodic_bounds()           // set up non-periodic boundaries
{
    /*
    Setup the lookup arrays, the edges are duplicated with non-periodic
    boundaries (similar to a mirror). Sand will not avalanche off the edge of the modelspace, but
    it can be transported out of the modelspace by the wind. We've chosen to mirror the edges,
	but this could be modified. See Fonstad (2006, Geomorphology 77, 217-234) for more discussion.
    */
    for (int i = 0; i < nrows; i++)
    {
        i_n[i] = i - 1;
        i_s[i] = i + 1;
    }
    i_n[0] = 0;                         // mirrored edges
    i_s[nrows - 1] = (nrows - 1);

    for (int j = 0; j < ncols; j++)
    {
        j_e[j] = j + 1;
        j_w[j] = j - 1;
    }
    j_w[0] = 0;                         // mirrored edges
    j_e[ncols - 1] = (ncols - 1);

    // setup deposition coordinates
    if (wdir == 1) // northerly: non-periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i + depjump) < nrows)  // if still within range, set shift
            {
                i_dp[i] = i + depjump;
            }
            else                        // else set the coordinate as toxic
            {
                i_dp[i] = i_toxic;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 1;
    }

    if (wdir == 2) // southerly: non-periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i - depjump) >= 0)  // if still within range, set shift
            {
                i_dp[i] = i - depjump;
            }
            else                        // else set the coordinate as toxic
            {
                i_dp[i] = i_toxic;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 1;
    }

    if (wdir == 3) // easterly: non-periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j - depjump) >= 0)  // if still within range, set shift
            {
                j_dp[j] = j - depjump;
            }
            else        // else set the coordinate as toxic
            {
                j_dp[j] = j_toxic;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 1;
    }

    if (wdir == 4) // westerly: non-periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j + depjump) < ncols)  // if still within range, set shift
            {
                j_dp[j] = j + depjump;
            }
            else        // else set the coordinate as toxic
            {
                j_dp[j] = j_toxic;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 1;
    }
}

void nonperiodic_bounds_EW()        // set up non-periodic boundaries E-W, periodic boundaries N-S
{
    /*
    Setup the lookup arrays, the edges are duplicated with non-periodic
    boundaries (similar to a mirror). Sand will not avalanche off the edge of the modelspace, but
    it can be transported out of the modelspace by the wind. We've chosen to mirror the edges,
	but this could be modified. See Fonstad (2006, Geomorphology 77, 217-234) for more discussion.
    */
    for (int i = 0; i < nrows; i++)
    {
        i_n[i] = i - 1;
        i_s[i] = i + 1;
    }
    i_n[0] = (nrows - 1);
    i_s[nrows - 1] = 0;

    for (int j = 0; j < ncols; j++)
    {
        j_e[j] = j + 1;
        j_w[j] = j - 1;
    }
    j_w[0] = 0;                     // mirrored edges
    j_e[ncols - 1] = (ncols - 1);

    // setup deposition coordinates
    if (wdir == 1) // northerly: periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i + depjump) < nrows)  // if still within range, set shift
            {
                i_dp[i] = i + depjump;
            }
            else                        // else write from the beginning again
            {
                i_dp[i] = i + depjump - nrows;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 2;
    }

    if (wdir == 2) // southerly: periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i - depjump) >= 0)  // if still within range, set shift
            {
                i_dp[i] = i - depjump;
            }
            else                        // else write from the beginning again
            {
                i_dp[i] = nrows + i - depjump;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 2;
    }

    if (wdir == 3) // easterly: non-periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j - depjump) >= 0)  // if still within range, set shift
            {
                j_dp[j] = j - depjump;
            }
            else        // else set the coordinate as toxic
            {
                j_dp[j] = j_toxic;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 1;
    }

    if (wdir == 4) // westerly: non-periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j + depjump) < ncols)  // if still within range, set shift
            {
                j_dp[j] = j + depjump;
            }
            else        // else set the coordinate as toxic
            {
                j_dp[j] = j_toxic;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 1;
    }
}

void nonperiodic_bounds_NS()        // set up non-periodic boundaries N-S, periodic boundaries E-W
{
    /*
    Setup the lookup arrays, the edges are duplicated with non-periodic
    boundaries (similar to a mirror). Sand will not avalanche off the edge of the modelspace, but
    it can be transported out of the modelspace by the wind. We've chosen to mirror the edges,
	but this could be modified. See Fonstad (2006, Geomorphology 77, 217-234) for more discussion.
    */
    for (int i = 0; i < nrows; i++)
    {
        i_n[i] = i - 1;
        i_s[i] = i + 1;
    }
    i_n[0] = 0;             // mirrored edges
    i_s[nrows - 1] = (nrows - 1);

    for (int j = 0; j < ncols; j++)
    {
        j_e[j] = j + 1;
        j_w[j] = j - 1;
    }
    j_w[0] = (ncols - 1);
    j_e[ncols - 1] = 0;

    // setup deposition coordinates
    if (wdir == 1) // northerly: non-periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i + depjump) < nrows)  // if still within range, set shift
            {
                i_dp[i] = i + depjump;
            }
            else                        // else set the coordinate as toxic
            {
                i_dp[i] = i_toxic;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 1;
    }

    if (wdir == 2) // southerly: non-periodic boundary
    {
        for (int i = 0; i < nrows; i++)
        {
            if ((i - depjump) >= 0)  // if still within range, set shift
            {
                i_dp[i] = i - depjump;
            }
            else                        // else set the coordinate as toxic
            {
                i_dp[i] = i_toxic;
            }
        }
        for (int j = 0; j < ncols; j++) // set j coordinates with no shift
        {
            j_dp[j] = j;
        }
        shadloops = 1;
    }

    if (wdir == 3) // easterly: periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j - depjump) >= 0)  // if still within range, set shift
            {
                j_dp[j] = j - depjump;
            }
            else        // else write from the beginning again
            {
                j_dp[j] = ncols + j - depjump;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 2;

    }

    if (wdir == 4) // westerly: periodic boundary
    {
        for (int j = 0; j < ncols; j++)
        {
            if ((j + depjump) < ncols)  // if still within range, set shift
            {
                j_dp[j] = j + depjump;
            }
            else        // else write from the beginning again
            {
                j_dp[j] = j + depjump - ncols;
            }
        }
        for (int i = 0; i < nrows; i++) // set i coordinates with no shift
        {
            i_dp[i] = i;
        }
        shadloops = 2;
    }
}

void shadupdate (int i, int j)      // update the shadow at a given site
{
    // declare variables
    int lpCnt;

    // northerly
    if (wdir == 1)
    {
        // first change the shadow height to the topographic height
        for (int i_d = 0; i_d < nrows; i_d++)
        {
            shad[i_d][j] = surf[i_d][j];
        }

        lpCnt = 0; i = 0;   // set starting coordinates for the loop to update shadow
        while (lpCnt < (nrows * shadloops))
        {   // check to see if the shadow can be higher and update
            if ((shad[i_n[i]][j] - dropdist > surf[i][j]) &&
                (shad[i_n[i]][j] - dropdist > shad[i][j]))
                {
                    shad[i][j] = shad[i_n[i]][j] - dropdist;
                }
            i = i_s[i];     // re-assign to next row down
            lpCnt++;        // increment the loop counter
        }
    }
    // southerly
    if (wdir == 2)
    {
        // first change the shadow height to the topographic height
        for (int i_d = 0; i_d < nrows; i_d++)
        {
            shad[i_d][j] = surf[i_d][j];
        }

        lpCnt = 0; i = (nrows - 1);   // set starting coordinates for the loop to update shadow
        while (lpCnt < (nrows * shadloops))
        {   // check to see if the shadow can be higher and update
            if ((shad[i_s[i]][j] - dropdist > surf[i][j]) &&
                (shad[i_s[i]][j] - dropdist > shad[i][j]))
                {
                    shad[i][j] = shad[i_s[i]][j] - dropdist;
                }
            i = i_n[i];     // re-assign to next row down
            lpCnt++;        // increment the loop counter
        }
    }
    // easterly
    if (wdir == 3)
    {
        // first change the shadow height to the topographic height
        for (int j_d = 0; j_d < ncols; j_d++)
        {
            shad[i][j_d] = surf[i][j_d];
        }

        lpCnt = 0; j = (ncols - 1);   // set starting coordinates for the loop to update shadow
        while (lpCnt < (ncols * shadloops))
        {   // check to see if the shadow can be higher and update
            if ((shad[i][j_e[j]] - dropdist > surf[i][j]) &&
                (shad[i][j_e[j]] - dropdist > shad[i][j]))
                {
                    shad[i][j] = shad[i][j_e[j]] - dropdist;
                }
            j = j_w[j];     // re-assign the next column
            lpCnt++;        // increment the loop counter
        }
    }
    // westerly
    if (wdir == 4)
    {
        // first change the shadow height to the topographic height
        for (int j_d = 0; j_d < ncols; j_d++)
        {
            shad[i][j_d] = surf[i][j_d];
        }

        lpCnt = 0; j = 0;   // set starting coordinates for the loop to update shadow
        while (lpCnt < (ncols * shadloops))
        {   // check to see if the shadow can be higher and update
            if ((shad[i][j_w[j]] - dropdist > surf[i][j]) &&
                (shad[i][j_w[j]] - dropdist > shad[i][j]))
                {
                    shad[i][j] = shad[i][j_w[j]] - dropdist;
                }
            j = j_e[j];     // re-assign the next column
            lpCnt++;        // increment the loop counter
        }
    }
}

void init_shadupdate()              // set shadow update for the first time
{
    // first set the shadow to be identical to the present topography
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            shad[i][j] = surf [i][j];
        }
    }

    // update the shadow with the shadupdate function
    if (wdir == 1) // northerly
    {
        for (int j = 0; j < ncols; j++)
        {
            shadupdate(0, j);
        }
    }

    if (wdir == 2) // southerly
    {
        for (int j = 0; j < ncols; j++)
        {
            shadupdate((nrows - 1), j);
        }
    }

    if (wdir == 3) // easterly
    {
        for (int i = 0; i < nrows; i++)
        {
            shadupdate(i, (ncols - 1));
        }
    }

    if (wdir == 4) // westerly
    {
        for (int i = 0; i < nrows; i++)
        {
            shadupdate(i, 0);
        }
    }
}

void avalanche_up(int i, int j)     // avalanche up (called after picking up a slab)
{
    // declare variables
	bool avidir[4] = {false, false, false, false};  // directions which a slab could fall from
    int avi_final;        							// final decision of avalanche direction
    // coordinates of boolean are referenced as: 0 = north, 1 = south, 2 = east, 3 = west

    // check the directions, check slope and availability of sand above the basement
    // look to the north
    if ((surf[i_n[i]][j] - surf[i][j] > avalanche_thresh) && (surf[i_n[i]][j] - bsmt[i_n[i]][j] > 0))
    {
        avidir[0] = true;
    }
    // look to the south
    if ((surf[i_s[i]][j] - surf[i][j] > avalanche_thresh) && (surf[i_s[i]][j] - bsmt[i_s[i]][j] > 0))
    {
        avidir[1] = true;
    }
    // look to the east
    if ((surf[i][j_e[j]] - surf[i][j] > avalanche_thresh) && (surf[i][j_e[j]] - bsmt[i][j_e[j]] > 0))
    {
        avidir[2] = true;
    }
    // look to the west
    if ((surf[i][j_w[j]] - surf[i][j] > avalanche_thresh) && (surf[i][j_w[j]] - bsmt[i][j_w[j]] > 0))
    {
        avidir[3] = true;
    }

    // check to see if there is an avalanche to fall
    if ((avidir[0]) || (avidir[1]) || (avidir[2]) || (avidir[3]))
    {
        // break any ties and make a final decision
        do
        {
            avi_final = genrand_int32() % 4;   // draw a random direction
        }
        while (!avidir[avi_final]);               // repeat until the direction is suitable for avalanche

        // move slabs	
		// move slab from the north
		if (avi_final == 0)
        {
			surf[i][j]++;               // add the slab of sand that avalanches
			i = i_n[i];                 // reset the focal coordinates
			
			// ------------------------------------------------------------------------------
			// Analysis add-in: slablogger
			wdune_slablogger.increment_avi(i, j, 2);	// moving slab to the south
			// ------------------------------------------------------------------------------

			surf[i][j]--;               // subtract the slab of sand
            avalanche_up (i, j);        // call the function recursively
        }
        // move slab from the south
        if (avi_final == 1)
        {
            surf[i][j]++;               // add the slab of sand that avalanches
            i = i_s[i];                 // reset the focal coordinates
            			
			// ------------------------------------------------------------------------------
			// Analysis add-in: slablogger
			wdune_slablogger.increment_avi(i, j, 1);	// moving slab to the north
			// ------------------------------------------------------------------------------

			surf[i][j]--;               // subtract the slab of sand
            avalanche_up (i, j);        // call the function recursively
        }
        // move slab from the east
        if (avi_final == 2)
        {
            surf[i][j]++;               // add the slab of sand that avalanches
            j = j_e[j];                 // reset the focal coordinates
            
			// ------------------------------------------------------------------------------
			// Analysis add-in: slablogger
			wdune_slablogger.increment_avi(i, j, 4);	// moving slab to the west
			// ------------------------------------------------------------------------------
			
			surf[i][j]--;               // subtract the slab of sand
            avalanche_up (i, j);        // call the function recursively
        }
        // move slab from the west
        if (avi_final == 3)
        {
            surf[i][j]++;               // add the slab of sand that avalanches
            j = j_w[j];                 // reset the focal coordinates

			// ------------------------------------------------------------------------------
			// Analysis add-in: slablogger
			wdune_slablogger.increment_avi(i, j, 3);	// moving slab to the east
			// ------------------------------------------------------------------------------

			surf[i][j]--;               // subtract the slab of sand
            avalanche_up (i, j);        // call the function recursively
        }
    }
    // else, slabs are finished being moved for this call, run the shadow update
    else
    {
        shadupdate (i, j);         // force run the shadupdate function
    }
}

void avalanche_down(int i, int j)   // avalanche down (called after placing a slab)
{
    // declare variables
	bool avidir[4] = {false, false, false, false};  // directions which a slab could fall from
    int avi_final;        							// final decision of avalanche direction
    // coordinates of boolean are referenced as: 0 = north, 1 = south, 2 = east, 3 = west

    // check the directions, check slope, no need to check availability because a slab was just deposited
    // look to the north
    if (surf [i][j] - surf[i_n[i]][j] > avalanche_thresh)
    {
        avidir[0] = true;
    }
    // look to the south
    if (surf[i][j] - surf[i_s[i]][j] > avalanche_thresh)
    {
        avidir[1] = true;
    }
    // look to the east
    if (surf[i][j] - surf[i][j_e[j]] > avalanche_thresh)
    {
        avidir[2] = true;
    }
    // look to the west
    if (surf[i][j] - surf[i][j_w[j]] > avalanche_thresh)
    {
        avidir[3] = true;
    }

    // check to see if there is an avalanche to fall
    if ((avidir[0]) || (avidir[1]) || (avidir[2]) || (avidir[3]))
    {
        // break any ties and make a final decision
        do
        {
            avi_final = genrand_int32() % 4;   // draw a random direction
        }
        while (!avidir[avi_final]);         // repeat until the direction is suitable for avalanche

        // move the sand

		// ------------------------------------------------------------------------------
		// Analysis add-in: slablogger
		wdune_slablogger.increment_avi(i, j, (avi_final + 1));
		// ------------------------------------------------------------------------------

        // move slab to the north
        if (avi_final == 0)
        {
            surf[i][j]--;               // subtract the slab of sand
            i = i_n[i];                 // reset the focal coordinates
            surf[i][j]++;               // add the slab of sand that avalanches
            avalanche_down (i, j);      // call the function recursively
        }
        // move slab to the south
        if (avi_final == 1)
        {
            surf[i][j]--;               // subtract the slab of sand
            i = i_s[i];                 // reset the focal coordinates
            surf[i][j]++;               // add the slab of sand that avalanches
            avalanche_down (i, j);      // call the function recursively
        }
        // move slab to the east
        if (avi_final == 2)
        {
            surf[i][j]--;               // subtract the slab of sand
            j = j_e[j];                 // reset the focal coordinates
            surf[i][j]++;               // add the slab of sand that avalanches
            avalanche_down (i, j);      // call the function recursively
        }
        // move slab to the west
        if (avi_final == 3)
        {
            surf[i][j]--;               // subtract the slab of sand
            j = j_w[j];                 // reset the focal coordinates
            surf[i][j]++;               // add the slab of sand that avalanches
            avalanche_down (i, j);      // call the function recursively
        }
    }
    // else, update the shadow
    else
    {
        shadupdate (i, j);         // run the shadupdate function
    }
}

void newSandEngine()                // add new sand to the modelspace
{
    // declare variables
    int sandType, sandSide, lpcntr, i, j;

    if (newSandCode !=0)      // allow quick exit from function if no new sand
    {
        // decompose the new sand code
        sandType = newSandCode / 10;
        sandSide = newSandCode % 10;

        lpcntr = 0;    // reset the loop counter
        // point sources
        if (sandType == 1)
        {
            if (sandSide == 1)      // north side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = 0; j = ncols / 2;           // halfway along the edge (approx)
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }
            if (sandSide == 2)      // south side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = (nrows - 1); j = ncols / 2; // halfway along the edge (approx)
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }
            if (sandSide == 3)      // east side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = nrows / 2; j = (ncols - 1); // halfway along the edge (approx)
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }
            if (sandSide == 4)      // west side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = nrows / 2; j = 0;           // halfway along the edge (approx)
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }
        }
        if (sandType == 2)          // edge sources
        {
            if (sandSide == 1)      // north side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = 0;
                    j = genrand_int32() % ncols;    // random location on edge
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }
            if (sandSide == 2)      // south side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = (nrows - 1);
                    j = genrand_int32() % ncols;    // random location on edge
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }
            if (sandSide == 3)      // east side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = genrand_int32() % nrows;    // random location on edge
                    j = (ncols - 1);
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }
            if (sandSide == 4)      // west side
            {
                while (lpcntr < newSandSlabs)
                {
                    i = genrand_int32() % nrows;    // random location on edge
                    j = 0;
                    surf[i][j]++;                   // add a slab
                    avalanche_down(i, j);           // avalanche down
                    lpcntr++;                       // advance counter
                }
            }

        }
        init_shadupdate();    // force the shadow to be updated for the entire model
    }
}

void picksite_ero()                 // pick a site to erode from
{
    // first sample a random location
    int i = genrand_int32() % nrows;
    int j = genrand_int32() % ncols;
    /*
	Conditions for erosion:
        1) surface higher than basement
        2) surface higher or equal to shadow (e.g., not in a shadow zone)
    Notes:
    This function is an if statement, rather than a while loop to allow
    time to pass properly. If the conditions are assessed as part of a
    while loop, time stands unnaturally still searching for a site for erosion.
    */
    if ((surf[i][j] > bsmt[i][j]) && (surf[i][j] >= shad[i][j]))
    {
        i_ero = i; j_ero = j;   // if conditions are met, set the erosion coordinates
        ero_flag = true;        // set the flag high
    }
    else
    {
        ero_flag = false;       // else, no erosion this time, flag is low
    }
}

void picksite_depo(int i, int j)    // pick a site to deposit
{
    double probCut;                 // set local probability cutoff
    bool foundSite = false;         // set flag denoting whether a site has been found
    while (!foundSite)
    {
        // ------------------------------------------------------------------------
		// Slablogger analysis add-in: call before moving coordinates!
		wdune_slablogger.increment_trans(i, j);
		// ------------------------------------------------------------------------
		
		// if i or j is toxic, break the loop immediately, the site is off the model space
        if (i == i_toxic || j == j_toxic)
        {
            i_depo = i; j_depo = j;
            break;
        }

		// reset i and j with the deposition lookup (move downwind)
        i = i_dp[i]; j = j_dp[j];
        		
		// calculate the probability of depositing
        if (surf[i][j] < shad[i][j])
        {
            probCut = 1.0;
        }
        else        // else set the probability cutoff based on psand
        {
            if (surf[i][j] > bsmt[i][j])    // if surface greater than basement, there is sand
            {
                probCut = psand;
            }
            else				// else, there is no sand, set probability cutoff
            {
                probCut = pnosand;
            }
        }
        // now draw a random number and check the probability cutoff
        if (genrand_real1() < probCut)
        {
            i_depo = i; j_depo = j;   // set deposition coordinates
            foundSite = true;         // a site was found, break the loop
        }
    }
}

void deposit(int i, int j)          // deposit sand at a site
{
    if (i == i_toxic || j == j_toxic)
    {
        slabs_out++;                // the slab got blown out of the modelspace
    }
    else
    {
        surf[i][j]++;               // deposit the sand
        avalanche_down(i, j);       // run the avalanche down after depositing the sand
    }
}




