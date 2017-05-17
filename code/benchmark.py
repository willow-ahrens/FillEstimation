from util import *
import numpy as np

matrices = ["freeFlyingRobot_5"]
matrices = ["pathological_asx"]
oski_kwargs= {"delta": 0.1}
asx_kwargs= {"epsilon": 0.1, "delta": 0.01}

reference = benchmark("reference", matrices)
oski = benchmark("oski", matrices, **oski_kwargs)
asx = benchmark("asx", matrices, **asx_kwargs)

print("       Reference Time: %g" % np.mean(reference))
print("            OSKI Time: %g" % np.mean(oski))
print("             ASX Time: %g" % np.mean(asx))
print("     ASX/OSKI Speedup: %g" % np.mean(oski/asx))
print("ASX/Reference Speedup: %g" % np.mean(reference/asx))

trials = 100;
references = get_references(matrices)
asx = fill_estimates("asx", matrices, results = True, trials = trials, **asx_kwargs)
oski = fill_estimates("oski", matrices, results = True, trials = trials, **asx_kwargs)
get_errors(asx, references)
get_errors(oski, references)
asx = np.concatenate([result["errors"] for result in asx], axis = 0)
oski = np.concatenate([result["errors"] for result in oski], axis = 0)
print("       Over %d trials..." % trials)
print(" Median asx max error: %g" % np.median(asx))
print("Median oski max error: %g" % np.median(oski))
print("   Mean asx max error: %g" % np.mean(asx))
print("  Mean oski max error: %g" % np.mean(oski))
