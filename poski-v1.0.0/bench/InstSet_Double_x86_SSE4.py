# InstSet_Double_x86_SSE4.py
# -------
import exceptions

class CodeGeneratorException(exceptions.Exception):
    def __init__(self):
        return
        
    def __str__(self):
        print "Code generator exception."

class InstSet_Double_x86_SSE4:
    """
    Contains functions to generate instruction set specific code.
    This uses the SSE4 vector extensions for packed doubles.
    """
    def GetOSKIDataType(self):
        """
        Returns the extension OSKI adds onto this instruction set (Tid, Tis, etc)
        """
        return "Tid"
    
    def GetScalarSize(self):
        """
        Returns the size of a scalar, in bytes.
        """
        return 8
    
    def SIMDGetVectorLength(self):
        """
        Returns the number of scalars in a vector.
        """
        return 2
    
    def SIMDMultiply(self, arg1, arg2):
        """
        Returns code to do a vector multiply on arg1 and arg2.
        """
        return "_mm_mul_pd(" + arg1 + ", " + arg2 + ")"
    
    def SIMDAdd(self, arg1, arg2):
        """
        Returns code to do a vector add on arg1 and arg2.
        """
        return "_mm_add_pd(" + arg1 + ", " + arg2 + ")"
    
    def SIMDHAdd(self, arg1, arg2):
        """
        Returns code to do a vector horizontal add on arg1 and arg2.
        Specifically, this is the instruction which adds every two adjacent
        scalars and stores it in the output vector.
        """
        return "_mm_hadd_pd(" + arg1 + ", " + arg2 + ")"
    
    def SIMDDotProduct(self, arg1, arg2):
        """
        Returns code to do a dot product on arg1 and arg2 and place the result
		in the first element of the output vector.
        """
        return "_mm_dp_pd(" + arg1 + ", " + arg2 + ", 0x31)"
    
    def SIMDLoadMultiple(self, addr):
        """
        Returns code to load a SIMD vector with the contents of memory starting at addr.
        This loads all scalar elements into the vector.
        """
        return "_mm_loadu_pd(" + addr + ")"

    def SIMDLoadRepeated(self, addr):
        """
        Returns code to load a SIMD vector with repetitions of the contents of memory at addr.
        This loads all scalar elements into the vector.
        """
        return "_mm_load1_pd(" + addr + ")"

    def SIMDLoadPartial(self, addr, num):
        """
        Returns code to load a SIMD vector with the contents of memory starting at addr.
        This loads up to num scalar elements into the vector.
        """
        if (num != 1):
            raise CodeGeneratorException
        return "_mm_load_sd(" + addr + ")"
    
    def SIMDStoreMultiple(self, addr, arg1):
        """
        Returns code to store a SIMD vector arg1 to the memory starting at addr.
        """
        return "_mm_store_pd(" + addr + "," + arg1 + ")"
    
    def SIMDLoadZero(self):
        """
        Returns code to fill a SIMD vector with zeros
        """
        return "_mm_setzero_pd()"
    
    def ScalarType(self):
        """
        Returns the scalar type. Can be used to declare scalars.
        """
        return "double"

    def VectorType(self):
        """
        Returns the vector type. Can be used to declare vectors.
        """
        return "__m128d"
    
    def DeclareScalar(self, varName):
        """
        Returns code to declare scalar variables.
        """
        return "double" + varName + ";"
    
    def SIMDDeclareVector(self, varName):
        """
        Returns code to declare a SIMD vector.
        """
        return "__m128d " + varName + ";"
    
    def PrefetchReadNontemporal(self, addr, numScalars=1):
        """
        Returns code to do a non-temporal (no temporal locality), read-only prefetch at addr.
        """
        if (numScalars != 1):
            raise CodeGeneratorException    # block prefetch not supported yet
        return "__builtin_prefetch(" + addr + ", 0, 0);"
    