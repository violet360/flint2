/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 Fredrik Johansson

******************************************************************************/

#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "nmod_mat.h"

long
nmod_mat_nullspace(nmod_mat_t X, const nmod_mat_t A)
{
    long i, j, k, m, n, rank, nullity;
    long * p;
    long * pivots;
    long * nonpivots;
    nmod_mat_t tmp;

    m = A->r;
    n = A->c;

    p = malloc(sizeof(long) * FLINT_MAX(m, n));

    nmod_mat_init_set(tmp, A);
    rank = nmod_mat_rref(p, tmp);
    nullity = n - rank;

    nmod_mat_zero(X);

    if (rank == 0)
    {
        for (i = 0; i < nullity; i++)
            nmod_mat_entry(X, i, i) = 1UL;
    }
    else if (nullity)
    {
        pivots = p;            /* length = rank */
        nonpivots = p + rank;  /* length = nullity */

/*
        for (i = j = k = 0; i < rank; i++)
        {
            while (fmpz_is_zero(tmp->rows[i] + j))
            {
                nonpivots[k] = j;
                k++;
                j++;
            }
            pivots[i] = j;
            j++;
        }
        while (k < nullity)
        {
            nonpivots[k] = j;
            k++;
            j++;
        }

        fmpz_set(den, tmp->rows[0] + pivots[0]);

        for (i = 0; i < nullity; i++)
        {
            for (j = 0; j < rank; j++)
                fmpz_set(res->rows[pivots[j]] + i, tmp->rows[j] + nonpivots[i]);
            fmpz_neg(res->rows[nonpivots[i]] + i, den);
        }
*/


        for (i = j = k = 0; i < rank; i++)
        {
            while (nmod_mat_entry(tmp, i, j) == 0UL)
            {
                nonpivots[k] = j;
                k++;
                j++;
            }
            pivots[i] = j;
            j++;
        }
        while (k < nullity)
        {
            nonpivots[k] = j;
            k++;
            j++;
        }

        for (i = 0; i < nullity; i++)
        {
            for (j = 0; j < rank; j++)
            {
                mp_limb_t c = nmod_mat_entry(tmp, j, nonpivots[i]);
                nmod_mat_entry(X, pivots[j], i) = nmod_neg(c, A->mod);
            }

            nmod_mat_entry(X, nonpivots[i], i) = 1UL;
        }
    }

    free(p);
    nmod_mat_clear(tmp);

    return nullity;
}

/*

            ONE = R(1)
            for i in xrange(self._ncols):
                if not (i in pivots_set):
                    v = V(0)
                    v[i] = ONE
                    for r in range(len(pivots)):
                        v[pivots[r]] = -E[r,i]
                    basis.append(v)




WANT:
(1, 0, 0, 0, 100, 0, 0, 51),
(0, 1, 0, 0, 1, 0, 0, 51),
(0, 0, 1, 0, 0, 0, 100, 50),
(0, 0, 0, 1, 100, 100, 0, 0)


RREF:
[  1   0   0   0  51  50  51 100]
[  0   1   0   0  50  51  51 100]
[  0   0   1   0   0   0   1   0]
[  0   0   0   1   0   1   0   0]
[  0   0   0   0   0   0   0   0]

HAVE(STUPID):
[ 51  50  51 100   0   0   0   0]
[ 50  51  51 100   0   0   0   0]
[  0   0   1   0   0   0   0   0]
[  0   1   0   0   0   0   0   0]
[  1   0   0   0   0   0   0   0]
[  0   1   0   0   0   0   0   0]
[  0   0   1   0   0   0   0   0]
[  0   0   0   1   0   0   0   0]


*/