#!/bin/bash

echo "generating profile"
profile=$(sbatch generate_profile.sl)
echo "$profile"
echo "generating spmv_records"
spmv_records=$(sbatch generate_spmv_records.sl)
echo "$spmv_records"
echo "generating references"
references=$(sbatch generate_references.sl)
echo "$references"
echo "generating table_data"
table_data=$(sbatch --dependency=afterok:${profile##* }:${spmv_records##* }:${references##* } generate_table_data.sl)
echo "$table_data"
echo "generating table"
table=$(sbatch --dependency=afterok:${table_data##* } generate_table.sl)
echo "$table"
echo "generating plots"
plots=$(sbatch --dependency=afterok:${profile##* }:${spmv_records##* }:${references##* } generate_plots.sl)
echo "$plots"
