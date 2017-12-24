if [ -d "/Users/Peter/Projects/FillEstimation/deps/gsl/build/lib" ] && [[ ":$LD_LIBRARY_PATH:" != *":/Users/Peter/Projects/FillEstimation/deps/gsl/build/lib:"* ]]; then
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:+"$LD_LIBRARY_PATH:"}/Users/Peter/Projects/FillEstimation/deps/gsl/build/lib"
fi
if [ -d "/Users/Peter/Projects/FillEstimation/deps/taco/build/lib" ] && [[ ":$LD_LIBRARY_PATH:" != *":/Users/Peter/Projects/FillEstimation/deps/taco/build/lib:"* ]]; then
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:+"$LD_LIBRARY_PATH:"}/Users/Peter/Projects/FillEstimation/deps/taco/build/lib"
fi
