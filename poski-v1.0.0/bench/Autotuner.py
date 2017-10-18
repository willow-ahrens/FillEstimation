"""
THE Autotuner.
Run this to automatically tune the OSKI library to the target machine.
"""
import exceptions

import os
import shutil

import Toolchain_GCC
import Benchmarker_OSKIBench

import InstSet_Double_x86_SSE4
import CodeGen_BCSRRowMaj_Matmul
import CodeGen_MBCSRRowMaj_Matmul
import CodeInjector

class PathErrorException(exceptions.Exception):
    def __init__(self):
        return
        
    def __str__(self):
        print "Path error exception: unable to locate the OSKI source directory"

class Autotuner:
    def __init__(self, codeGen, instSet,
                 buildDir=None, srcDir=None,
                 toolchain=Toolchain_GCC.Toolchain_GCC, 
                 benchmarker=Benchmarker_OSKIBench.Benchmarker_OSKIBench):

        # initialize working directories
        if (buildDir == None):
            self.BuildDir = os.environ.get("OSKIBUILD", None)
        else:
            self.BuildDir = buildDir
            
        if not(os.path.isdir(self.BuildDir)):
            print "ERR: Invalid OSKI build directory '" + self.BuildDir + "'"
            raise PathErrorException
        print "Using OSKI build directory '" + self.BuildDir + "'"
        
        self.SrcDir = srcDir
        # if source directory was not specified, generate it from the build directory config
        # specifically, this looks inside config.status under the build directory
        if (self.SrcDir == None):
            configFile = open(self.BuildDir + "/config.status", "r")
            for line in configFile:
                if line[0:8] == "srcdir='":
                    path = line[8:-2]
                    #self.SrcDir = self.BuildDir + "/" + path
		    self.SrcDir = path
                    print self.SrcDir

        if not(os.path.isdir(self.SrcDir)):
            print "ERR: Invalid OSKI source directory '" + self.SrcDir + "'"
            raise PathErrorException
        print "Using OSKI source directory '" + self.SrcDir + "'"
            
        # initialize parameter space - related variables        
        self.CodeGen = codeGen;
        self.TuneParamSpace = codeGen.GetParamSpace()
        self.CodeParamSpace = codeGen.GetCodeParamSpace()
        self.InstSet = instSet;
        self.Toolchain = toolchain(self);
        self.Benchmarker = benchmarker(self);
        
        self.CodeParamSpace = self.GenerateParamSpace(codeGen.GetCodeParamSpace())
        self.TuneParamSpace = self.GenerateParamSpace(codeGen.GetParamSpace())

        codeParamSpace = self.CodeParamSpace
        
        # do code backups
        self.CreateCodeBackups()

    def GenerateParamSpace(self, paramSpace):
        """
        This returns a list of all possible parameter permutations.
        """
        paramList = []
        
        params = list(param for param in paramSpace.iterkeys())
        params.sort()
        paramLengths = list(len(paramSpace[param]) for param in params)
        paramArgs = list(0 for param in params)
        paramCount = len(params)
        
        # loop over all parameter permutations
        done = False
        while done == False:
            # initialize parameter dict
            paramDict = {}
            for i in range(0, paramCount):
                param = params[i]
                paramDict[param] = paramSpace[param][paramArgs[i]]
                
            paramList.append(paramDict)
            
            # go to the next parameter "permutation"
            for i in range(0, paramCount):
                paramArgs[i] = paramArgs[i] + 1
                if paramArgs[i] == paramLengths[i]:
                    paramArgs[i] = 0;
                    # if we've reset the last permutation, we're done
                    if (i == paramCount - 1):
                        done = True;
                else:
                    break;
        print paramList
        return paramList
            
    def Autotune(self):
        print "Beginning autotune operations"
        for tuneParams in self.TuneParamSpace:
            print "Autotunung tuneParams=" + str(tuneParams)
            self.GenerateAllCode(tuneParams)
            self.Toolchain.Compile()
            self.Benchmarker.RunBenchmark(self.CodeGen, self.InstSet, tuneParams)
            
        for codeParams in self.CodeParamSpace:
            bestTuneParams = self.Benchmarker.GetBest(codeParams)
            print "Generating best code for " + str(codeParams) + ": " + str(bestTuneParams)
            self.GenerateCode(codeParams, bestTuneParams)
            
            
    def CreateCodeBackups(self):
        print "Generating original code backups"
        
        codePath = self.SrcDir + self.CodeGen.GetCodePath() + "/"
        
        for codeParam in self.CodeParamSpace:
            fileName = self.CodeGen.GetCodeName(codeParam) 
            if (os.path.isfile(codePath + fileName)):
                if not(os.path.isfile(codePath + fileName + ".orig")):
                    print "* Backing up " + fileName + " to " + fileName + ".orig ... "
                    shutil.copy(codePath + fileName, codePath + fileName + ".orig")
                else:
                    print "* Backup of " + fileName + " already exists"
            else:
                print "* Warning: original " + codePath + fileName + " not found"
        
    def GenerateAllCode(self, tuneParams):
        print "Generating code with tuneParams=" + str(tuneParams)
        
        for codeParam in self.CodeParamSpace:
            self.GenerateCode(codeParam, tuneParams)


    def GenerateCode(self, codeParams, tuneParams):
        codePath = self.SrcDir + self.CodeGen.GetCodePath() + "/"
        fileName = self.CodeGen.GetCodeName(codeParams)
        genCode = self.CodeGen.GenerateCode(self.InstSet, codeParams, tuneParams)
          
        CodeInjector.CodeInject(codePath+fileName+".orig", codePath+fileName, "MBCSR_MatMult_v1_aX_b1_xs1_ysX", genCode)
            
        print "* Created " + fileName

if __name__ == '__main__':
    """
    The main function, which does the autotuning.
    """    
    instSet = InstSet_Double_x86_SSE4.InstSet_Double_x86_SSE4()
    codeGen = CodeGen_MBCSRRowMaj_Matmul.CodeGen_MBCSRRowMaj_Matmul()
    
    autotuner = Autotuner(codeGen, instSet)
    autotuner.Autotune()
