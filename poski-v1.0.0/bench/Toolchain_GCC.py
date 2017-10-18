import subprocess
import InstSet_Double_x86_SSE4

class Toolchain_GCC:
    def __init__(self, autotuner):
        self.Autotuner = autotuner
        
    """
    Functions for interacting with the compiler toolchain
    """
    
    def Compile(self):
        """
        Recompile the library. This does an incremental build.
        This returns the output from the operation.
        """
        print "Starting compile"
        make = subprocess.Popen(["make"],
                                cwd=self.Autotuner.BuildDir,
                                stdout = subprocess.PIPE,
                                stderr = subprocess.PIPE)
        
        print " ... waiting for compile"
        (makeout, makeerr) = make.communicate()
        print " ... compile completed"
        print makeout
        
    def Clean(self):
        """
        Clean (remove) any compiled files.
        The next compile will start from scratch.
        This returns the output from the operation.
        """
        print "Starting clean"
        make = subprocess.Popen(["make", "clean"],
                                cwd=self.Autotuner.BuildDir,
                                stdout = subprocess.PIPE,
                                stderr = subprocess.PIPE)
        
        print " ... waiting for clean"
        (makeout, makeerr) = make.communicate()
        print " ... clean completed"
        
    def GetInstructionSupport(self):
        """
        Returns the instruction sets supported by the machine and compiler
        """
        ## TODO TODO CURRENTLY NOT IMPLEMENTED
        return [InstSet_Double_x86_SSE4.InstSet_Double_x86_SSE4()]