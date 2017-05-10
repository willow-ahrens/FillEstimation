#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define prob_examine 0.5

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
	size_t I;		/* block-row iteration variable */

	//if number of columns is 0
	if (n == 0) {
	    *p_nnz_est = 0;
		memset(p_nb_est, 0, sizeof(size_t) * tempsize);
		return 0;	/* Quick return */
    }
	
    //Allocate temporary space.
	assert(n >= 1);
	
    // TODO(hjxu) - not sure if this is the right size
    block_count = malloc(sizeof(size_t) * B * n * n);
	
    if (block_count == NULL) {
		return -1;
	}
	memset(block_count, 0, sizeof(size_t) * B * n);

	//note(hjxu):	might be able to go from B ^ 3 to B * r ^ 2
	memset(p_nb_est, 0, sizeof(size_t) * tempsize);
	num_nonzeros = 0;

	assert(p_nnz_est != NULL);
	assert(p_nb_est != NULL);

	for (size_t o_r = 0; o_r < r; o_r++) {
		int num_rows_to_add = r - o_r - 1;
		if (num_rows_to_add > 0) {
			//prepend rows
		}
			
        //number of total rows
		int new_rows = m + num_rows_to_add;
		int block_rows = new_rows / r;	/* # of full block-rows */

		/*
		 * phase I: Count the number of new blocks to create.
		 */

        // -1 block is the one that goes off the side.
        for (I = -1; I < block_rows; I++) {	/* loop over block rows */
				size_t i;
				size_t di;

                // if there is no o_r, we do not need the extra one
                if(o_r == 0) { I++; }
				double rand_val = (double)rand() / (double)RAND_MAX;

				if (rand_val > prob_examine)
					continue;	/* skip this block row */

				/*
				 * Count the number of blocks within
				 * block-row I, and remember in 'block_count'
				 * which of the possible blocks have been
				 * 'visited' (i.e., contain at least 1
				 * non-zero).
				 */
				for (i = I * r + o_r, di = 0; di < r; di++, i++) {
					size_t k;

                    // only if o_r > 0
                    // simulate a smaller "real" block
                    // TODO(hjxu): b_r - o_r might need to have a -1.
                    if (I == -1) { i = 0; di = r - o_r; }
					/*
					 * Invariant: block_count[J] == # of
					 * non-zeros * encountered in rows
					 * I*r .. I*r+di that should * be
					 * stored in column-block J (i.e.,
					 * that have * column indices J*c <=
					 * j < (J+1)*c).
					 */
					/*
					 * Count the number of additional
					 * logical blocks * needed to store
					 * non-zeros in row i, and mark * the
					 * blocks in block row I that have
					 * been visited.
					 */
					for (k = ptr[i]; k < ptr[i + 1]; k++) {
						size_t j = ind[k];	/* column index */
						size_t c;
						/** Get the block count for block column size c, block column J. */
#define GET_BC(A, bc, oc, J) (A)[((bc)*B*n) + (oc)*n + (J)]
						/** Increment the block count for block column size c, block column J. */
#define INC_BC(A, bc, oc, J) (A)[((bc)*B*n) + (oc)*n + (J)]++
						/** Set the block count for block column size c, block column J, to zero. */
#define ZERO_BC(A, bc, oc, J) (A)[((bc)*B*n) + (oc)*n + (J)] = 0

                       /** find index in p_nb_est given r, c, or, oc. */
#define INDEX_INC(r, c, or, oc) (r*c*(c-1)/2)+(or*c)+(oc)
						/*
						 * POST: num_blocks == total
						 * # of blocks in examined
						 * rows.
						 */
						/*
						 * POST: num_nonzeros ==
						 * total # of non-zeros in
						 * examined rows.
						 */
						for (size_t b_c = 1; b_c <= B; c++) {
                            for (size_t o_c = 0; o_c < b_c; o_c++) {
								int num_cols_to_add = b_c - o_c - 1;
								int new_cols = n + num_cols_to_add;
                                
    						    size_t J = (j + o_c - 1) / new_cols;	/* block column index */

                                //create(count) new block
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
					 * POST: num_nonzeros == total # of
					 * non-zeros examined so far
					 */
					/*
					 * POST: num_blocks == total # of new
					 * blocks in rows 0..i
					 */
					/*
					 * POST: block_count[c,J] == # of
					 * non-zeros in block J of block-row
					 * I
					 */
					/*
					 * Reset block_count for next
					 * iteration, I+1. This second loop *
					 * is needed to keep the complexity
					 * of phase I to O(nnz).
					 */
					
				    for (i = I * r + o_r, di = 0; di < r; di++, i++) {
					    size_t k;

                        // TODO(hjxu): b_r - o_r might need to have a -1.
                        if (I == -1) { i = 0; di = r - o_r; }

						for (k = ptr[i]; k < ptr[i + 1]; k++) {
							size_t j = ind[k];	/* column index */
							size_t c;
    						for (size_t b_c = 1; b_c <= B; c++) {
                                for (size_t o_c = 0; o_c < b_c; o_c++) {
		    						int num_cols_to_add = b_c - o_c - 1;
			    					int new_cols = n + num_cols_to_add;
                                    
    				    		    size_t J = (j + o_c - 1) / new_cols;	/* block column index */
                                    ZERO_BC(block_count, b_c, o_c, J);
								}
							}
						}
					}
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
	size_t i = 0;
	int err;

	//size of array estimating block counts
		int tempspace = B * B * B;
	size_t *nb_est = malloc(sizeof(size_t) * tempspace);

	//out of memory
		if (nb_est == NULL)
		return -1;

	//try all sizes and offsets
		for (size_t b_r = 1; b_r <= B; b_r++) {
		size_t nnz_est = 0;
		memset(nb_est, 0, sizeof(size_t) * tempspace);

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
					//fill[i] = fill for b_r,b_c, o_r, o_c
				    j++;
					i++;
				}
			}
		}
	}
	free(nb_est);
	return 0;

	/*
	 * ======= int estimate_fill (size_t m, size_t n, size_t nnz, const
	 * size_t *ptr, const size_t *ind, size_t B, double *fill, int
	 * verbose){ size_t i = 0; for (size_t b_r = 1; b_r <= B; b_r++) {
	 * for (size_t b_c = 1; b_c <= B; b_c++) { for (size_t o_r = 0; o_r <
	 * b_r; o_r++) { for (size_t o_c = 0; o_c < b_c; o_c++) { fill[i] =
	 * 0.5; i++; } } } } return 0; >>>>>>>
	 * c2a6897cfe5cc098aec042d814023c69ad757bce
	 */
}
