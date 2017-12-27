{
  "experiment_name" : "DefaultExperiment",
  "data_path" : os.path.join(top, "data"),
  "matrix_path" : os.path.join(top, "data/matrix"),
  "matrix_registry_path" : os.path.join(top, "data/matrix/registry.json"),
  "run_path" : os.path.join(top, "run"),
  "fill_prefix" : "",
  "fill_vars" : {},
  "spmv_prefix" : "",
  "spmv_vars" : {},
  "create_script" : create_bash_script,
  "B" : 12,
  "epsilon" : 0.5,
  "delta" : 0.01,
  "sigma" : 0.02,
  "trials" : 100,
  "profile_m" : 1000,
  "profile_n" : 1000,
  "profile_trials" : 100,
  "verbose" : False,
  "table_matrices" : ["3dtube_conv", "gupta1_conv"],
  "plot_points" : {"3dtube_conv": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.001, 0.06, 10))],
                   "gupta1_conv": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.001, 0.06, 10))],
                   "ct20stif": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(7, 0.2, 10), exprange(0.001, 0.06, 10))],
                   "pathological_phil": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(2, 0.7, 10), exprange(0.01, 1.0, 10))],
                   "pathological_oski": [{"epsilon":e, "delta":0.01, "sigma":s} for (e, s) in zip(exprange(2, 0.7, 10), exprange(0.01, 1.0, 10))]}
}
