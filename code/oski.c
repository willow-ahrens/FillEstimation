#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define prob_examine 0.02
#define DEBUG 0

#ifdef DEBUG
#define DEBUG_PRINT(x) //printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

/**
 * \brief Given an m x n matrix in CSR
 * estimates the fill ratio if the matrix were converted
 * into r x c  BCSR format with offset o_x, o_y.
 *
 * The caller supplies this routine with a maximum column
 * block size B, and this routine returns the
 * estimated fill ratios for all 1 <= c <= B, 0 <= o_x <= r,
 * 0 <= o_y <= c.
 *
 * If the converted matrix has n_b blocks, this
 * implementation executes in
 * \f$O(\mbox{stored non-zeros}) = O(n_b\cdot r\cdot c)\f$
 * time, but requires \f$O(C\cdot n)\f$ auxiliary storage
 * space to store a dense copies of the block rows.
 *
 * EstimateBlockCounts(ptr, ind, m, n, r, B, &nnz_est, nb_est);
 * \param[in] ptr CSR row pointers
 * \param[in] ind CSR column indices
 * \param[in] m Logical number of matrix rows
 * \param[in] n Logical number of matrix columns
 * \param[in] r Desired row block size
 * \param[in] B Maximum block size
 * \param[in] tempsize size of array to be returned (B^3)
 * \param[in, out] nnz_est Used to return the number of
 *  non-zeros actually examined. Must be non-NULL.
 * \param[in, out] nb_est Used to return the number of
 * r x c blocks (offset by o_x, o_y) that would be created for the
 * non-zeros examined. Must be non-NULL array of length
 * B^3 (for all c, o_x, o_y).
 *
 * \returns On success, returns 0, sets *p_nnz_est
 * to the number of non-zeros examined, and sets nb_est
 * to the number of non-zero blocks per configuration
 * to a 1D vector in the order
 *
 *  size_t i = 0;
 *  for (size_t b_c = 1; b_c <= B; b_c++) {
 *    for (size_t o_r = 0; o_r < b_r; o_r++) {
 *      for (size_t o_c = 0; o_c < b_c; o_c++) {
 *        nb_est[i] = number of blocks that would be created for the r, c, o_x, o_y examined.
 *        i++;
 *      }
 *    }
 *  }
 *
 *  On error, return error code and leaves nnz_est, nb_est unchanged.
 */
static int
EstimateBlockCounts(const size_t * ptr, const size_t * ind,
		    size_t m, size_t n,
		    size_t r, size_t B, size_t tempsize,
		    size_t * p_nnz_est, size_t * p_nb_est)
{
	/* block dimensions */
	size_t M;

	/* stores total number of non-zero blocks */
	size_t num_nonzeros;

	/* auxiliary storage: reused for each block-row */
	size_t *block_count;
	int I;			/* block-row iteration variable */

	/*
	 * if number of columns is 0
	 */
	if (n == 0) {
		*p_nnz_est = 0;
		memset(p_nb_est, 0, sizeof(size_t) * tempsize);
		return 0;	/* Quick return */
	}
	/* TODO(hjxu): check size is correct */
	block_count = malloc(sizeof(size_t) * B * n * n);

	if (block_count == NULL) {
		return -1;
	}
	memset(block_count, 0, sizeof(size_t) * B * n * n);

//note(hjxu):might be able to go from B ^ 3 to B * r ^ 2
	memset(p_nb_est, 0, sizeof(size_t) * tempsize);
	num_nonzeros = 0;

	assert(p_nnz_est != NULL);
	assert(p_nb_est != NULL);

	for (size_t o_r = 0; o_r < r; o_r++) {
		DEBUG_PRINT(("\to_r: %d\n", o_r));
		int num_rows_to_add = 0;

		if (o_r != 0) {
			num_rows_to_add = r - o_r;
		}
		DEBUG_PRINT(("\t\tnum_rows_to_add: %d\n", num_rows_to_add));

		//number of total rows
		int new_rows = m + num_rows_to_add;
		int block_rows = new_rows / r;	/* # of full block-rows */

		DEBUG_PRINT(("\t\tblock_rows: %d\n", block_rows));
		/*
		 * phase I: Count the number of new blocks to create.
		 */

		bool pastfirst = false;
		/* -1 block is the one that goes off the side. */
		int startindex = -1;
		if (o_r == 0) {
			startindex++;
		}
		for (I = startindex; I < block_rows; I++) {	/* loop over block rows */
			double rand_val = (double)rand() / (double)RAND_MAX;

			if (rand_val > prob_examine) {
				DEBUG_PRINT(("skipped row block %d\n", I));
				continue;	/* skip this block row */
			}
			DEBUG_PRINT(("doing row block %d\n", I));
			/*
			 * Count the number of blocks within block-row I, and
			 * remember in 'block_count' which of the possible
			 * blocks have been 'visited' (i.e., contain at least
			 * 1 non-zero).
			 */
            size_t i = I * r + o_r;
            size_t di;
			if (I == -1) {
			    i = 0;
				di = r - o_r;
			}
            while (di < r) {
                DEBUG_PRINT(("block index: %d, row index: %d, small row index: %d\n", I, i, di));
				size_t k;

			/*
				 * Invariant: block_count[J] == # of
				 * non-zeros * encountered in rows I*r ..
				 * I*r+di that should * be stored in
				 * column-block J (i.e., that have * column
				 * indices J*c <= j < (J+1)*c).
				 */
				/*
				 * Count the number of additional logical
				 * blocks * needed to store non-zeros in row
				 * i, and mark * the blocks in block row I
				 * that have been visited.
				 */

				for (k = ptr[i]; k < ptr[i + 1]; k++) {
					size_t j = ind[k];	/* column index */

#define BC_INDEX(bc, oc, J) ((bc)*B*n) + (oc)*n + (J)
                    /** Get the block count for block column size c, block column J. */
#define GET_BC(A, bc, oc, J) (A)[BC_INDEX(bc, oc, J)]
					/** Increment the block count for block column size c, block column J. */
#define INC_BC(A, bc, oc, J) (A)[BC_INDEX(bc, oc, J)]++
					/** Set the block count for block column size c, block column J, to zero. */
#define ZERO_BC(A, bc, oc, J) (A)[BC_INDEX(bc, oc, J)] = 0

					/** find index in p_nb_est given r, c, or, oc. */
#define INDEX_INC(r, c, or, oc) (r*c*(c-1)/2)+(or*c)+(oc)
					/*
					 * POST: num_blocks == total # of
					 * blocks in examined rows.
					 */

					DEBUG_PRINT(("NZ found at (%d, %d)\n", i, j));
					/*
					 * POST: num_nonzeros == total # of
					 * non-zeros in examined rows.
					 */
					for (size_t b_c = 1; b_c <= B; b_c++) {
						for (size_t o_c = 0; o_c < b_c; o_c++) {
							int num_cols_to_add = b_c - o_c - 1;
							int new_cols = n + num_cols_to_add;

							//block column index
								size_t J;
							if (o_c == 0) {
								J = j / new_cols;
							} else {
								J = (j + o_c - 1) / new_cols;
							}

							DEBUG_PRINT(("row: %d, col %d, o_r: %d, o_c: %d\n", r, b_c, o_r, o_c));
							//create(count) new block
                            DEBUG_PRINT(("BC_INDEX %d\n", BC_INDEX(b_c, o_c, J)));
							if (GET_BC(block_count, b_c, o_c, J) == 0) {
								INC_BC(block_count, b_c, o_c, J);
								//indexing based on c outer.
								p_nb_est[INDEX_INC(r, b_c, o_r, o_c)]++;
							}
						}
					}
				}
				 num_nonzeros += ptr[i] - ptr[I * r];

				/*
				 * POST: num_nonzeros == total # of non-zeros
				 * examined so far
				 */
				/*
				 * POST: num_blocks == total # of new blocks
				 * in rows 0..i
				 */
				/*
				 * POST: block_count[c,J] == # of non-zeros
				 * in block J of block-row I
				 */
				/*
				 * Reset block_count for next iteration, I+1.
				 * This second loop * is needed to keep the
				 * complexity of phase I to O(nnz).
				 */
			i++;
            di++;
            }
    
            i = I * r + o_r;
            if (I == -1) {
                i = 0;
                di = r - o_r;
            }                                            
            while (di < r) {
				size_t k;
				for (k = ptr[i]; k < ptr[i + 1]; k++) {
					size_t j = ind[k];	/* column index */
					size_t c;
					for (size_t b_c = 1; b_c <= B; b_c++) {
						for (size_t o_c = 0; o_c < b_c; o_c++) {
							DEBUG_PRINT(("CLEANUP: row: %d, col %d, o_r: %d, o_c: %d\n", r, b_c, o_r, o_c));
							int num_cols_to_add = b_c - o_c - 1;
							int new_cols = n + num_cols_to_add;
							size_t J;
							if (o_c == 0) {
								J = j / new_cols;
							} else {
								J = (j + o_c - 1) / new_cols;
							}

							ZERO_BC(block_count, b_c, o_c, J);
						}
					}
				}
                i++;
                di++;
			}
		}
	}

	free(block_count);

	*p_nnz_est = num_nonzeros;
	return 0;
}

char *
name()
{
	return "oski";
}

/**
 *  Given an m by n CSR matrix A, estimates the fill ratio if the matrix were
 *  converted into b_r by b_c (offset by o_r and o_c) BCSR format. The fill
 *  ratio is b_r times b_c times the number of nonzero blocks in the BCSR format
 *  divided by the number of nonzeros.
 *
 *  The caller supplies this routine with a maximum row and column block size B,
 *  and this routine returns the estimated fill ratios for all
 *  1 <= b_r, b_c <= B and 0 <= o_r < b_r and 0 <= o_c < b_c
 *
 *  This routine assumes the CSR matrix uses full storage, and assumes that
 *  column indicies are sorted.
 *
 *  \param[in] m Logical number of matrix rows
 *  \param[in] n Logical number of matrix columns
 *  \param[in] nnz Logical number of matrix nonzeros
 *  \param[in] *ptr CSR row pointers.
 *  \param[in] *ind CSR column indices.
 *  \param[in] B Maximum desired block size
 *  \param[out] *fill Fill ratios for all specified b_r, b_c, o_r, o_c in order
 *  \param[in] verbose 0 if you should be quiet
 *
 *  Note that the fill ratios should be stored according to the following order:
 *  size_t i = 0;
 *  for (size_t b_r = 1; b_r <= B; b_r++) {
 *    for (size_t b_c = 1; b_c <= B; b_c++) {
 *      for (size_t o_r = 0; o_r < b_r; o_r++) {
 *        for (size_t o_c = 0; o_c < b_c; o_c++) {
 *          //fill[i] = fill for b_r, b_c, o_r, o_c
 *          i++;
 *        }
 *      }
 *    }
 *  }
 *
 *  \returns On success, returns 0. On error, returns an error code.
 */
int
estimate_fill(size_t m,
	      size_t n,
	      size_t nnz,
	      const size_t * ptr,
	      const size_t * ind,
	      size_t B,
	      double *fill,
	      int verbose)
{
	DEBUG_PRINT(("nnz: %d\n", nnz));
	DEBUG_PRINT(("m: %d, n: %d\n", m, n));
	size_t i = 0;
	int err;

	//size of array estimating block counts
		int tempspace = B * B * B;
	size_t *nb_est = malloc(sizeof(size_t) * tempspace);

	//out of memory
		if (nb_est == NULL)
		return -1;

	DEBUG_PRINT(("MAX BLOCK SIZE: %d\n", B));
	//try all sizes and offsets
		for (size_t b_r = 1; b_r <= B; b_r++) {
		size_t nnz_est = 0;
		memset(nb_est, 0, sizeof(size_t) * tempspace);
		DEBUG_PRINT(("row size: %d\n", b_r));

		err = EstimateBlockCounts(ptr, ind, m, n, b_r, B, tempspace, &nnz_est, nb_est);
		if (err) {
			free(nb_est);
			return err;
		}
		int j = 0;

		for (size_t b_c = 1; b_c <= B; b_c++) {
			for (size_t o_r = 0; o_r < b_r; o_r++) {
				for (size_t o_c = 0; o_c < b_c; o_c++) {

					size_t nb_nnz = nb_est[j] * b_r * b_c;
					double ratio;
					if (!nnz_est)
						ratio = nb_nnz ? (1.0 / 0.0) : 1.0;
					else
						ratio = (double)nb_nnz / nnz_est;

					fill[i] = ratio;
					/*
					 * fill[i] = fill for b_r ,b_c, o_r,
					 * o_c
					 */
					j++;
					i++;
				}
			}
		}
	}
	free(nb_est);
	return 0;
}
