{
  "experiment_name" : "Lanka12",
  "data_path" : os.path.join(top, "data"),
  "matrix_path" : os.path.join(top, "data/matrix/full"),
  "matrix_registry_path" : os.path.join(top, "data/matrix/full/registry.json"),
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
#SBATCH --exclude lanka29
#SBATCH --exclude lanka27
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
	"af_shell10_conv",
	"pathological_phil",
	"kron_g500-logn17_conv",
	"nasasrb_conv",
	"rail4284_conv",
	"pdb1HYS_conv",
	"F1_conv",
	"troll_conv",
	"pwtk_conv",
	"boneS10_conv",
	"kkt_power_conv",
	"in2010_conv",
	"pathological_oski",
	"fcondp2_conv",
	"fem_hifreq_circuit_conv",
	"nd6k_conv",
	"spal_004_conv",
	"fl2010_conv",
	"crankseg_1_conv",
	"flickr_conv",
	"bmw7st_1_conv",
	"gearbox_conv",
	"BenElechi1_conv",
	"nd3k_conv",
	"af_shell9_conv",
	"ok2010_conv",
	"s3dkt3m2_conv",
	"nd24k_conv",
	"ct20stif_conv",
	"gupta1_conv",
	"TSOPF_RS_b2383_conv",
	"hugetric-00010_conv",
	"ship_001_conv",
	"degme_conv",
	"atmosmodl_conv",
	"largebasis_conv",
	"halfb_conv",
	"ldoor_conv",
	"3dtube_conv",
	"exdata_1_conv",
	"pds-100_conv",
	"kim2_conv",
	"Emilia_923_conv",
	"mip1_conv",
	"bundle_adj_conv",
	"inline_1_conv",
	"m_t1_conv"
	],
  "plot_points" : {"3dtube_conv": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.0001, 0.06, 10))],
                   "gupta1_conv": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.0001, 0.06, 10))],
                   "ct20stif_conv": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.0001, 0.06, 10))],
                   "pathological_phil": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.3, 10), exprange(0.0001, 0.032, 10))],
                   "pathological_oski": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.0001, 0.14, 10))]}
}
