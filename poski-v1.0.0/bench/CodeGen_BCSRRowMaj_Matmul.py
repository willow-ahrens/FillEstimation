# SPMVGen_BCSR.py
# -------
import math
import InstSet_Double_x86_SSE4

class CodeGen_BCSRRowMaj_Matmul:
    def GetCodePath(self):
        """
        Return the path to where this code is to be stored.
        This path is relative to the OSKI source directory.
        """
        return "/src/BCSR/MatMult"
    
    def GetCodeName(self, params):
        """
        Returns the name of this code file for a certain code parameter set.
        """
        return str(params["r"]) + "x" + str(params["c"]) + ".c"
    
    def GetCodeParamSpace(self):
        """
        Returns the parameter space of the OSKI code.
        This does not include the 'extended' tuning autotuned
        by this program.
        """
        return {'r':[1,2,3,4,5,6,7,8],
                'c':[1,2,3,4,5,6,7,8]}
        
    def GetParamSpace(self):
        """
        Returns the tuning parameter space as a dict of parameter:[list of values]
        """
        return {'MultiplyOrder':['Normal', 'Interleaved'],   \
                'ReduceStyle':['HAdd', 'DotProduct'],        \
                'PrefetchDist':[0,16,32,64,128,256,512]}
        
    def GetDataStructure(self):
        return "BCSR"
    
    def GetOperation(self):
        return "MatMult"
        
    def GenerateCode(self, inst, codeParams, params):
        """
        Generates r x c register-blocked SPMV code for the BCSR row major data structure
        taking advantage of vector instructions in inst.
        r is the number of rows (or the y/vertical-size)
        c is the number of columns (or the x/horizontal-size)
        params contains a dict of paramaeters and their values,
          corresponding to the space allowed in GetParamSpace 
        """
        tab = "    "        # indent character
        nl = "\n"           # newline character
        
        r = codeParams["r"]
        c = codeParams["c"]
        
        code = ""
        vectorLen = inst.SIMDGetVectorLength();
        simdLen = c / vectorLen;                        # Number of whole SIMD vectors per row
        partialLen = c % inst.SIMDGetVectorLength();    # Number of leftover scalars per row after all SIMD vectors filled
        
        
        # fill in OSKI standard stuff
        code += tab + "oski_value_t* yp;" + nl;
        code += tab + "oski_index_t I;" + nl;
        code += tab + "for( I = 0, yp = y; I < M; I++, yp += " + str(r) + "*incy ) {" + nl;
        
        code += tab*2 + "oski_index_t K;" + nl;
        
        # Declare the output vectors
        yvars = []
        for i in range(0,r):
            varName = "_y" + str(i)
            yvars.append(varName)
            code += tab*2 + inst.SIMDDeclareVector(varName) + nl;
            
        # Initialize the output vectors
        for varName in yvars:
            code += tab*2 + varName + " = " + inst.SIMDLoadZero() + ";" + nl
        
        code += nl
        
        # Generate SpMV Inner Loop Code
        code += tab*2 + "for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += " + str(r) + "*" + str(c) + " ) {" + nl

        code += tab*3 + "oski_index_t j0 = bind[0]; /* block's leftmost col index */" + nl
        code += tab*3 + "const oski_value_t* xp = x + j0;" + nl
        code += nl
        
        # Generate row-vectors
        xvars = []
        for i in range(0,simdLen):
            varName = "_x"
            for j in range(0,vectorLen):
                varName += str(i*vectorLen+j)
            xvars.append(varName)
            code += tab*3 + inst.VectorType() + " " + varName + ";" + nl
        
        xPartial ="_x"
        if (partialLen > 0):
            for j in range(simdLen*vectorLen, c):
                xPartial += str(j)
            code += tab*3 + inst.VectorType() + " " + xPartial + ";" + nl
        
        if (params['PrefetchDist'] != 0):
            code += nl
            code += tab*3 + inst.PrefetchReadNontemporal("bval + " + str(params['PrefetchDist'])) + ";" + nl
            code += nl
        
        # Load variables with the x-vector
        for i in range(0, simdLen):
            code += tab*3 + xvars[i] + " = " + inst.SIMDLoadMultiple("xp + " + str(i * vectorLen)) + ";" + nl
        if (partialLen > 0):
            code += tab*3 + xPartial + " = " + inst.SIMDLoadPartial("xp + " + str(simdLen * vectorLen), partialLen) + ";" + nl
        code += nl
        
        # Do multiply operations
        if (params['MultiplyOrder'] == 'Normal'):
            code += self.GenerateMultiply(inst, r, c, xvars, yvars, partialLen, xPartial, 3)
        elif (params['MultiplyOrder'] == 'Interleaved'):
            code += self.GenerateMultiply_Interleaved(inst, r, c, xvars, yvars, partialLen, xPartial, 3)
        
        code += tab*2 + "}" + nl
        
        code += nl
        
        # Do accumulate operations
        if (params['ReduceStyle'] == 'DotProduct'):
            code += self.GenerateAccumulate_DotProduct(inst, r, c, xvars, yvars, partialLen, xPartial, 2)
        elif (params['ReduceStyle'] == 'HAdd'):
            code += self.GenerateAccumulate_HAdd(inst, r, c, xvars, yvars, partialLen, xPartial, 2)
        
        code += tab + "}" + nl
        
        
        return code
    
    def GenerateMultiply(self, inst, r, c, xvars, yvars, partialLen, xPartial, tabs):
        """
        Generates code for the multiply stage.
        """
        tab = "    "        # indent character
        nl = "\n"           # newline character
        
        code = ""
        vectorLen = inst.SIMDGetVectorLength();
        simdLen = c / vectorLen;                        # Number of whole SIMD vectors per row
        
        # Do multiply operations
        for i in range(0, r):
            ytarget = yvars[i]      # y-variable being accumulated into
            for j in range(0, simdLen):
                bvalOffset = str(i*c + j*vectorLen)  # memory offset of the b-vector being multiplied
                loadCode = inst.SIMDLoadMultiple("bval + " + bvalOffset)
                multiplyCode = inst.SIMDMultiply(xvars[j], loadCode)
                
                code += tab*tabs + ytarget + " = " + inst.SIMDAdd(ytarget, multiplyCode) + ";" + nl
            
            if (partialLen > 0):
                bvalOffset = str(i*c + simdLen*vectorLen)  # memory offset of the b-vector being multiplied
                loadCode = inst.SIMDLoadMultiple("bval + " + bvalOffset)
                multiplyCode = inst.SIMDMultiply(xPartial, loadCode)
                
                code += tab*tabs + ytarget + " = " + inst.SIMDAdd(ytarget, multiplyCode) + ";" + nl
            
            code += nl
        
        return code
        
    def GenerateMultiply_Interleaved(self, inst, r, c, xvars, yvars, partialLen, xPartial, tabs):
        """
        Generates code for the multiply stage where destination registers are interleaved
        (in other words, this has a different order of operations).
        """
        tab = "    "        # indent character
        nl = "\n"           # newline character
        
        code = ""
        vectorLen = inst.SIMDGetVectorLength();
        simdLen = c / vectorLen;                        # Number of whole SIMD vectors per row
        
        # Do multiply operations
        for j in range(0, simdLen):
            for i in range(0, r):
                ytarget = yvars[i]      # y-variable being accumulated into
                bvalOffset = str(i*c + j*vectorLen)  # memory offset of the b-vector being multiplied
                loadCode = inst.SIMDLoadMultiple("bval + " + bvalOffset)
                multiplyCode = inst.SIMDMultiply(xvars[j], loadCode)
                
                code += tab*tabs + ytarget + " = " + inst.SIMDAdd(ytarget, multiplyCode) + ";" + nl
                
            code += nl
            
        if (partialLen > 0):
            for i in range(0, r):
                ytarget = yvars[i]      # y-variable being accumulated into
                bvalOffset = str(i*c + simdLen*vectorLen)  # memory offset of the b-vector being multiplied
                loadCode = inst.SIMDLoadMultiple("bval + " + bvalOffset)
                multiplyCode = inst.SIMDMultiply(xPartial, loadCode)
                
                code += tab*tabs + ytarget + " = " + inst.SIMDAdd(ytarget, multiplyCode) + ";" + nl
            
            code += nl
        
        return code
        
    def GenerateAccumulate_HAdd(self, inst, r, c, xvars, yvars, partialLen, xPartial, tabs):
        """
        Generates code for the accumulate stage using horizontal add operations.
        """
        tab = "    "        # indent character
        nl = "\n"           # newline character
        
        code = ""
        vectorLen = inst.SIMDGetVectorLength();
        simdLen = c / vectorLen;                        # Number of whole SIMD vectors per row
        
        # Accumulate output vector into single variable and multiply by alpha
        code += tab*tabs + inst.VectorType() + " alpha_simd;" + nl
        code += tab*tabs + "alpha_simd = " + inst.SIMDLoadRepeated("&alpha") + ";" + nl
        code += nl
        
        # Do accumulate with horizontal adds until the sums of the two vectors are at the bottom of the first vector
        # Doing horizontal adds, we can accumulate two vectors at a time
        numIters = math.log(vectorLen, 2) - 1
        numIters = int(round(numIters))
        for i in range(0,r-1,2):
            code += tab*tabs + yvars[i] + " = " + inst.SIMDHAdd(yvars[i], yvars[i+1]) + ";" + nl
            for i in range(0,numIters):
                code += tab*tabs + yvars[i] + " = " + inst.SIMDHAdd(yvars[i], yvars[i]) + ";" + nl
            code += tab*tabs + yvars[i] + " = " + inst.SIMDMultiply(yvars[i], "alpha_simd") + ";" + nl
        
        if (r % 2 != 0):
            code += tab*tabs + yvars[r-1] + " = " + inst.SIMDHAdd(yvars[r-1], yvars[r-1]) + ";" + nl
            for i in range(0,numIters):
                code += tab*tabs + yvars[r-1] + " = " + inst.SIMDHAdd(yvars[r-1], yvars[r-1]) + ";" + nl
            code += tab*tabs + yvars[r-1] + " = " + inst.SIMDMultiply(yvars[r-1], "alpha_simd") + ";" + nl
        
        code += nl
        
        # Store the output vectors
        alignment = str(vectorLen * inst.GetScalarSize())
        code += tab*tabs + inst.ScalarType() + " R[" + str(vectorLen) + "] __attribute((aligned(" + alignment + ")));" + nl
        
        for i in range(0,r-1,2):
            code += tab*tabs + inst.SIMDStoreMultiple("R", yvars[i]) + ";" + nl
            code += tab*tabs + "yp[" + str(i) + "*incy] += R[0];" + nl
            code += tab*tabs + "yp[" + str(i+1) + "*incy] += R[1];" + nl  
        
        if (r % 2 != 0):
            code += tab*tabs + inst.SIMDStoreMultiple("R", yvars[r-1]) + ";" + nl
            code += tab*tabs + "yp[" + str(r-1) + "*incy] += R[0];" + nl
                    
        return code
     
    def GenerateAccumulate_DotProduct(self, inst, r, c, xvars, yvars, partialLen, xPartial, tabs):
        """
        Generates code for the accumulate stage using dot products.
        """
        tab = "    "        # indent character
        nl = "\n"           # newline character
        
        code = ""
        vectorLen = inst.SIMDGetVectorLength();
        simdLen = c / vectorLen;                        # Number of whole SIMD vectors per row
        
        # Accumulate output vector into single variable and multiply by alpha
        code += tab*tabs + inst.VectorType() + " alpha_simd;" + nl
        code += tab*tabs + "alpha_simd = " + inst.SIMDLoadRepeated("&alpha") + ";" + nl
        code += nl
        
        # Do accumulate with dot products
        for i in range(0,r):
            code += tab*tabs + yvars[i] + " = " + inst.SIMDDotProduct(yvars[i], "alpha_simd") + ";" + nl
        
        code += nl
        
        # Store the output vectors
        alignment = str(vectorLen * inst.GetScalarSize())
        code += tab*tabs + inst.ScalarType() + " R[" + str(vectorLen) + "] __attribute((aligned(" + alignment + ")));" + nl
        
        for i in range(0,r):
            code += tab*tabs + inst.SIMDStoreMultiple("R", yvars[i]) + ";" + nl
            code += tab*tabs + "yp[" + str(i) + "*incy] += R[0];" + nl
        
        return code
        
if __name__ == '__main__':
    """
    The main function, which generates register blocked code based on the input arguments
    or for the default value of r=3, c=5, and the x86 SSE4 vector instructions.
    """
    CodeGenerator = CodeGen_BCSRRowMaj_Matmul()
    print CodeGenerator.GenerateCode(
      InstSet_Double_x86_SSE4.InstSet_Double_x86_SSE4(),
      3, 5,
      {'MultiplyOrder':'Normal', 'ReduceStyle':'DotProduct', 'PrefetchDist':64})
