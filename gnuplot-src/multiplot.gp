init_margins(left, right, bottom, top, dx, dy, rows, cols) = \
  sprintf('left_margin = %f; right_margin = %f; top_margin = %f; bottom_margin = %f; ', left, right, top, bottom) . \
  sprintf('col_count = %d; row_count = %d; gap_size_x = %f; gap_size_y = %f', cols, rows, dx, dy)

get_lmargin(col) = (left_margin + (col - 1) * (gap_size_x + ((right_margin - left_margin)-(col_count - 1) * gap_size_x)/col_count))
get_rmargin(col) = (left_margin + (col - 1) * gap_size_x + col * ((right_margin - left_margin)-(col_count - 1) * gap_size_x)/col_count)
get_tmargin(row) = (top_margin  - (row - 1) * gap_size_y - (row-1) * ((top_margin - bottom_margin  - gap_size_y * row_count) / row_count))
get_bmargin(row) = (top_margin  - (row - 1) * gap_size_y -  row    * ((top_margin - bottom_margin  - gap_size_y * row_count) / row_count))
set_margins(col, row) = \
  sprintf('set lmargin at screen %f;', get_lmargin(col)) . \
  sprintf('set rmargin at screen %f;', get_rmargin(col)) . \
  sprintf('set tmargin at screen %f;', get_tmargin(row)) . \
  sprintf('set bmargin at screen %f;', get_bmargin(row))    
