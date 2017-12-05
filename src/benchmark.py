from util import *
import numpy as np

matrices = ["freeFlyingRobot_5"]
oski_kwargs= {"delta": 0.1}
phil_kwargs= {"epsilon": 0.5, "delta": 0.01}

reference = benchmark("reference", matrices)
oski = benchmark("oski", matrices, **oski_kwargs)
phil = benchmark("phil", matrices, **phil_kwargs)

print("        Reference Time: %g" % np.mean(reference))
print("             OSKI Time: %g" % np.mean(oski))
print("             PHIL Time: %g" % np.mean(phil))
print("     PHIL/OSKI Speedup: %g" % np.mean(oski/phil))
print("PHIL/Reference Speedup: %g" % np.mean(reference/phil))

trials = 100;
references = get_references(matrices)
phil = fill_estimates("phil", matrices, results = True, trials = trials, **phil_kwargs)
oski = fill_estimates("oski", matrices, results = True, trials = trials, **oski_kwargs)
get_errors(phil, references)
get_errors(oski, references)
phil = np.concatenate([result["errors"] for result in phil], axis = 0)
oski = np.concatenate([result["errors"] for result in oski], axis = 0)
print("       Over %d trials..." % trials)
print("Median phil max error: %g" % np.median(phil))
print("Median oski max error: %g" % np.median(oski))
print("  Mean phil max error: %g" % np.mean(phil))
print("  Mean oski max error: %g" % np.mean(oski))
