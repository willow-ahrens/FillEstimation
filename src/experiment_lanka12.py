{
  "experiment_name" : "Lanka12",
  "data_path" : os.path.join(top, "data"),
  "matrix_path" : os.path.join(top, "../matrix"),
  "matrix_registry_path" : os.path.join(top, "../matrix/registry.json"),
  "run_path" : os.path.join(top, "run"),
  "fill_prefix" : "",
  "fill_vars" : {},
  "spmv_prefix" : "numactl -N 1",
  "spmv_vars" : {"TACO_CFLAGS":"-O3 -ffast-math -std=c99 -fopenmp -funroll-loops",
                 "OMP_NUM_THREADS":"12"},
  "create_script" : create_create_slurm_script(
"""
#SBATCH --nodes=1-1
#SBATCH --cpus-per-task=48
#SBATCH -t 06:00:00
#SBATCH --mail-user=pahrens@mit.edu
#SBATCH --mail-type=all
#SBATCH -p lanka-v3
"""),
  "B" : 12,
  "epsilon" : 0.5,
  "delta" : 0.01,
  "sigma" : 0.02,
  "trials" : 100,
  "profile_m" : 1000,
  "profile_n" : 1000,
  "profile_trials" : 100,
  "verbose" : False,
  "table_matrices" : [
    "3dtube_conv",
    "af_shell10_conv",
    "ASIC_680k_conv",
    "bmw7st_1_conv",
    "cage13_conv",
    "cfd2_conv",
    "cnr-2000_conv",
    "ct20stif",
    "degme_conv",
    "exdata_1_conv",
    "F1_conv",
    "G3_circuit_conv",
    "Ga41As41H72_conv",
    "gearbox_conv",
    "gupta1_conv",
    "Hamrle3_conv",
    "inline_1_conv",
    "kkt_power_conv",
    "largebasis_conv",
    "ldoor_conv",
    "nd24k_conv",
    "parabolic_fem_conv",
    "pathological_oski",
    "pathological_phil",
    "pds-100_conv",
    "pwtk_conv",
    "rail4284_conv",
    "rajat24_conv",
    "Stanford_conv",
    "stomach_conv",
    "thermal2_conv",
    "TSOPF_RS_b2383_conv"],
  "plot_points" : {"3dtube_conv": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.001, 0.06, 10))],
                   "gupta1_conv": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.001, 0.06, 10))],
                   "ct20stif": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.001, 0.06, 10))],
                   "pathological_phil": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(2, 0.3, 10), exprange(0.01, 1.0, 10))],
                   "pathological_oski": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(2, 0.3, 10), exprange(0.01, 1.0, 10))]}
}
