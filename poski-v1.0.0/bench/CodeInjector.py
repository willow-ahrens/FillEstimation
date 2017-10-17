# CodeInjector.py
# -------
import os
import InstSet_Double_x86_SSE4
import CodeGen_BCSRRowMaj_Matmul

"""
Contains functions to replace function code in a .c file.
"""
def ReplaceFunction(codeLines, functionName, newFunction):
    """
    Takes a list of lines of code, and replaces the contents of the target
    function with the new function.
    """
    outputLines = []
    copyingHeader = False
    replacingFunction = False
    
    outputLines.append("#include <smmintrin.h>\n")
	
    for line in codeLines:
        if (line.find(functionName) == 0):
            copyingHeader = True
            outputLines += line
        elif (line.find("{") == 0 and copyingHeader == True):
            copyingHeader = False
            replacingFunction = True
            outputLines.append(line)
            outputLines.append(newFunction)
        elif (line.find("}") == 0 and replacingFunction == True):
            replacingFunction = False
            outputLines.append(line)
        elif (replacingFunction == False):
            outputLines.append(line)
            
    return outputLines

def CodeInject(srcFileName, dstFileName, fnName, newFn):
    srcFile = open(srcFileName)

    srcLines = srcFile.readlines()
    dstLines = ReplaceFunction(srcLines, fnName, newFn)

    dstFile = open(dstFileName, "w")
    for line in dstLines:
        dstFile.write(line)
                
if __name__ == '__main__':
    """
    The main function, which injects SIMDized code into the OSKI BCSR sources.
    Removed
    """

    

    
        