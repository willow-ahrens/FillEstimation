
#else
    int Y_0[W];
    int c_hi[B + 1];
    int c_lo[B + 1];
    for (int b_c = 1; b_c <= B; b_c++) {
      c_hi[b_c] = B + ((j + b_c - 1) % b_c);
      c_lo[b_c] = c_hi[b_c] - b_c;
    }
    for (int b_r = 1; b_r <= B; b_r++) {
      int r_hi = B + ((i + b_r - 1) % b_r);
      int r_lo = r_hi - b_r;
      for (int c = 0; c < W; c++) {
        Y_0[c] = Z[r_hi][c] - Z[r_lo][c];
      }
      for (int b_c = 1; b_c <= B; b_c++) {
        int y_1 = Y_0[c_hi[b_c]] - Y_0[c_lo[b_c]];
        fill[fill_index] += 1.0/y_1;
        fill_index++;
      }
    }

    /*
    for (int b_r = 1; b_r <= B; b_r++) {
      int r_hi = B + ((i + b_r - 1) % b_r);
      int r_lo = r_hi - b_r;
      for (int b_c = 1; b_c <= B; b_c++) {
        int c_hi = B + ((j + b_c - 1) % b_c);
        int c_lo = c_hi - b_c;
        int y_1 = Z[r_hi][c_hi] - Z[r_lo][c_hi] - Z[r_hi][c_lo] + Z[r_lo][c_lo];
        fill[fill_index] += 1.0/y_1;
        fill_index++;
      }
    }
    */
#endif
