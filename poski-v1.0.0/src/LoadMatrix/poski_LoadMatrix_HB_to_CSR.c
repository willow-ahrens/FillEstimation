/**
 * @file src/LoadMatrix/poski_LoadMatrix_HB_to_CSR.c
 * @brief Read the pattern of a file stored in Harwell-Boeing format.
 *
 *  This module implements the routines to read the pattern of a file stored in Harwell-Boeing format.
 *  Modified from the HB format loader routines written by Sam Williams \<swwilliams@lbl.gov\>.
 *
 * @attention Called by
 * @ref poski_LoadMatrixHandler.c
 *
 *  Also, refer
 * @ref poski_LoadMatrix_common.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <poski/poski_loadmatcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// Internal functions
/**
 * @internal
 * @brief [library's internal use] Read the decode format.
 *
 * @param[in,out] FMT Format
 * @param[in,out] num
 * @param[in,out] type
 * @param[in,out] characters
 * @param[in,out] mantissa
 */
static void poski_decode_format(char *FMT, poski_index_t *num, char *type, poski_index_t *characters, poski_index_t *mantissa)
{
	poski_index_t i;
	char c;
	*num = -1;
	*type = '?';
	*characters = -1;
	*mantissa = -1;
	if (sscanf(FMT,"%d%c%d%c%d.%d)",&i,&c,num,type,characters,mantissa)==6)
	{
	}
	else if(sscanf(FMT,"(%d%c%d.%d)",num,type,characters,mantissa)==4)
	{
	}
	else if(sscanf(FMT,"(%d%c%d)",num,type,characters,mantissa)==3)
	{
	}
	else
	{
		poski_error("poski_decode_format", "unknown format- '%s'\n", FMT);
		assert(0);
	}	
}

/** 
 * @brief Read the pattern of a file stored in Harwell-Boeing format.
 *
 * @internal
 * @param[in] fname File to read
 * @param[in] MakeUnsymmetric
 *
 * @return A sparse matrix in CSR format if success, or an error message otherwise.
 */
poski_sparse_matrix_t *poski_LoadMatrix_HBF_to_CSR(char *fname, poski_index_t MakeUnsymmetric)
{
	FILE *fp;
	char temp[100];

	poski_index_t i,j;
	poski_index_t ZerosInFile=0;	

	poski_printMessage(2, "Matrix information...\n");

	fp = fopen(fname,"r");
	if(fp==NULL)
	{
		poski_error("poski_LoadMatrix_HBF_to_CSR", "can not open the file %s\n", fname);
		assert(0);
	}
	else
	{
		poski_printMessage(3, "Read %s HBF file into CSR format\n",fname);
	}

	poski_sparse_matrix_t *SpA = poski_malloc(poski_sparse_matrix_t, 1, 0);

	poski_InitSparseMatrix(SpA);	

	// load Header -----------------------
	poski_printMessage(3,"=> Header information\n");
	char L1[100];fgets(L1,100,fp);
	char TITLE[100];temp[71-00+1]='\0';sscanf(strncpy(temp,L1+00,71-00+1),"%s", TITLE   );	// 00..71 = TITLE
	char   KEY[100];temp[79-72+1]='\0';sscanf(strncpy(temp,L1+72,79-72+1),"%s", KEY     );	// 72..79 = KEY
	poski_printMessage(3, "\tTITLE='%s' KEY='%s'\n",TITLE,KEY);

	char L2[100];fgets(L2,100,fp);
	poski_index_t TOTLines;temp[13-00+1]='\0';   sscanf(strncpy(temp,L2+00,13-00+1),"%d",&TOTLines);		// 00..13 = Total number of lines excluding header
	poski_index_t PTRLines;temp[27-14+1]='\0';   sscanf(strncpy(temp,L2+14,27-14+1),"%d",&PTRLines);		// 14..27 = Number of lines for pointers
	poski_index_t INDLines;temp[41-28+1]='\0';   sscanf(strncpy(temp,L2+28,41-28+1),"%d",&INDLines);		// 28..41 = Number of lines for row (or variable) indices
	poski_index_t VALLines;temp[55-42+1]='\0';   sscanf(strncpy(temp,L2+42,55-42+1),"%d",&VALLines);		// 42..55 = Number of lines for numerical values
	poski_index_t RHSLines;temp[69-56+1]='\0';if(sscanf(strncpy(temp,L2+56,69-56+1),"%d",&RHSLines)==0)RHSLines=0;// 56..69 = Number of lines for right-hand sides
	if(strlen(L2)<=57)RHSLines=0;
	poski_printMessage(3, "\tTOTLines='%d' PTRLines='%d' INDLines='%d' VALLines='%d' RHSLines='%d'\n",TOTLines,PTRLines,INDLines,VALLines,RHSLines);

	char L3[100];fgets(L3,100,fp);
	char MXTYPE[100];temp[02-00+1]='\0';sscanf(strncpy(temp,L3+00,02-00+1),"%s", MXTYPE  );	// 00 - 02 = Matrix type (see below) {RCP,SUHNZR,AE}
	poski_index_t   NROW;temp[27-14+1]='\0';sscanf(strncpy(temp,L3+14,27-14+1),"%d",&NROW    );	// 14 - 27 = Number of rows (or variables)
	poski_index_t   NCOL;temp[41-28+1]='\0';sscanf(strncpy(temp,L3+28,41-28+1),"%d",&NCOL    );	// 28 - 41 = Number of columns (or elements)
	poski_index_t NNZERO;temp[55-42+1]='\0';sscanf(strncpy(temp,L3+42,55-42+1),"%d",&NNZERO  );	// 42 - 55 = Number of row (or variable) indices
	poski_index_t NELTVL;temp[69-56+1]='\0';sscanf(strncpy(temp,L3+56,69-56+1),"%d",&NELTVL  );	// 56 - 69 = Number of elemental matrix entries
	poski_printMessage(3, "\tMXTYPE='%s' NROW='%d' NCOL='%d' NNZERO='%d' NELTVL='%d'\n",MXTYPE,NROW,NCOL,NNZERO,NELTVL);

	if( (MXTYPE[1] == 'U') || (MXTYPE[1] == 'u') ){MXTYPE[1]='U';}
	else if( (MXTYPE[1] == 'R') || (MXTYPE[1] == 'r') ){MXTYPE[1]='R';}
	else if( (MXTYPE[1] == 'S') || (MXTYPE[1] == 's') ){MXTYPE[1]='S';}
	else {poski_error("poski_LoadMatrix_HBF_to_CSR", "unknown matrix type - %s\n",MXTYPE);assert(0);}
	if( (MXTYPE[0] == 'P') || (MXTYPE[0] == 'p') ){MXTYPE[0]='P';}
	else if( (MXTYPE[0] == 'R') || (MXTYPE[0] == 'r') ){MXTYPE[0]='R';}
	else {poski_error("poski_LoadMatrix_HBF_to_CSR", "unknown matrix type - %s\n",MXTYPE);assert(0);}

	poski_index_t NRowsPadded = CacheLineSizeInDoubles*((NROW+CacheLineSizeInDoubles-1)/CacheLineSizeInDoubles);
	poski_index_t NColsPadded = CacheLineSizeInDoubles*((NCOL+CacheLineSizeInDoubles-1)/CacheLineSizeInDoubles);

	char L4[100];fgets(L4,100,fp);
	char PTRFMT[100];temp[15-00+1]='\0';sscanf(strncpy(temp,L4+00,15-00+1),"%s" , PTRFMT  );	// 00..15 = Format for pointers - get rid of ()'s
	char INDFMT[100];temp[31-16+1]='\0';sscanf(strncpy(temp,L4+16,31-16+1),"%s" , INDFMT  );	// 16..31 = Format for row (or variable) indices  - get rid of ()'s
	char VALFMT[100];temp[51-32+1]='\0';sscanf(strncpy(temp,L4+32,51-32+1),"%s" , VALFMT  );	// 32..51 = Format for numerical values of coefficient matrix  - get rid of ()'s
	char RHSFMT[100];temp[71-52+1]='\0';sscanf(strncpy(temp,L4+52,71-52+1),"%s" , RHSFMT  );	// 52..71 = Format for numerical values of right-hand sides - get rid of ()'s
	poski_printMessage(3, "\tPTRFMT='%s' INDFMT='%s' VALFMT='%s' RHSFMT='%s'\n",PTRFMT,INDFMT,VALFMT,RHSFMT);

	if(RHSLines)
	{
		char L5[100];fgets(L5,100,fp);
		char RHSType =L5[0];									//00..00 = Right-hand side type: (F=Full or M=same as Matrix)
		char RHSGuess=L5[1];									//01..01 = G if a starting vector(s) (Guess) is supplied. (RHSTYP)
		char RHSExact=L5[2];									//02..02 = X if an exact solution vector(s) is supplied.
		poski_index_t NRHS;  temp[27-14+1]='\0';sscanf(strncpy(temp,L5+14,27-14+1),"%d",&NRHS    );	//14..27 = Number of right-hand sides (NRHS)
		poski_index_t NRHSIX;temp[41-28+1]='\0';sscanf(strncpy(temp,L5+28,41-28+1),"%d",&NRHSIX  );	//28..41 = Number of row indices (NRHSIX)
		poski_printMessage(3, "\tRHSType='%c' RHSGuess='%c' RHSExact='%c' NRHS='%d' NRHSIX='%d'\n",RHSType,RHSGuess,RHSExact,NRHS,NRHSIX);
	}
	poski_index_t PatternOnly = 0;
	if(VALLines==0)PatternOnly=1;
	if(MXTYPE[0]=='P')PatternOnly=1;

	// Decode Formats - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	poski_index_t PTRFMT_num,PTRFMT_chars,PTRFMT_mantissa;char PTRFMT_type;if(PTRLines)poski_decode_format(PTRFMT,&PTRFMT_num,&PTRFMT_type,&PTRFMT_chars,&PTRFMT_mantissa);
	poski_index_t INDFMT_num,INDFMT_chars,INDFMT_mantissa;char INDFMT_type;if(INDLines)poski_decode_format(INDFMT,&INDFMT_num,&INDFMT_type,&INDFMT_chars,&INDFMT_mantissa);
	poski_index_t VALFMT_num,VALFMT_chars,VALFMT_mantissa;char VALFMT_type;if(VALLines)poski_decode_format(VALFMT,&VALFMT_num,&VALFMT_type,&VALFMT_chars,&VALFMT_mantissa);
	if(PTRLines) poski_printMessage(3, "\t%s -> %d %c %d %d\n",PTRFMT,PTRFMT_num,PTRFMT_type,PTRFMT_chars,PTRFMT_mantissa);
	if(INDLines) poski_printMessage(3, "\t%s -> %d %c %d %d\n",INDFMT,INDFMT_num,INDFMT_type,INDFMT_chars,INDFMT_mantissa);
	if(VALLines) poski_printMessage(3, "\t%s -> %d %c %d %d\n",VALFMT,VALFMT_num,VALFMT_type,VALFMT_chars,VALFMT_mantissa);
	poski_printMessage(3, "\t%d(%d) x %d(%d), %d nonzeros\n",NRowsPadded,NROW,NColsPadded,NCOL,NNZERO);

	// Init Matrix / Choose strategies - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	SpA->nrows=NRowsPadded;
	SpA->ncols=NColsPadded;

	// Read Matrix into array of nodes - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	char WorkingLine[100];

	poski_printMessage(3, "=> Reading Column Pointers...");
	poski_index_t CPValue, Column=0;
	poski_index_t *ColumnPointers = poski_malloc(poski_index_t,(NCOL+1), 0);
	for(i=0;i<PTRLines;i++)
	{
		fgets(WorkingLine,100,fp);
		for(j=0;j<PTRFMT_num;j++)if(Column <= NCOL)
		{
			temp[PTRFMT_chars] = '\0';CPValue = atoi(strncpy(temp,WorkingLine + (PTRFMT_chars*j),PTRFMT_chars))-1; // -1 for conversion to arrays that start at 0
			ColumnPointers[Column] = CPValue;
			Column++;
		}
	}
	poski_printMessage1(3, "%sdone%s\n",CRT_DONE,CRT_RESET);

	poski_printMessage(3, "=> Reading Row Indices...");
	NonZeroNode *ArrayOfNodes = poski_malloc(NonZeroNode,(NNZERO+100), 0); 
	poski_index_t  Node=0;
	poski_index_t VNode=0;
	poski_index_t RowValue;
	Column=0;
	while(ColumnPointers[Column+1] == Node)Column++;
	for(i=0;i<INDLines;i++){
		fgets(WorkingLine,100,fp);
		for(j=0;j<INDFMT_num;j++){if(Node<NNZERO){
			temp[INDFMT_chars] = '\0';RowValue = atoi(strncpy(temp,WorkingLine + (INDFMT_chars*j),INDFMT_chars))-1; // -1 for conversion to arrays that start at 0
			ArrayOfNodes[Node].Row = RowValue;
			ArrayOfNodes[Node].Col = Column;
			ArrayOfNodes[Node].next = NULL;
#ifdef READ_VALUES
			ArrayOfNodes[Node].value = 1.0; // FIX
#endif
			Node++;
			while(ColumnPointers[Column+1] == Node)Column++;
		}
		}
	}
	poski_printMessage1(3, "%sdone%s\n",CRT_DONE,CRT_RESET);

	VNode=Node;
#ifdef READ_VALUES
	poski_printMessage(3, "=> Reading NonZero Values...");
	if(PatternOnly){
		poski_printMessage1(3, "%sIGNORED (Pattern Only)%s\n",CRT_ERROR,CRT_RESET);
	}
	else
	{
		for(i=0;i<=NCOL;i++)ColumnPointers[i]=0;

		oski_value_t NZValue;
		Node=0;
		VNode=0;

		for(i=0;i<VALLines;i++)
		{
			fgets(WorkingLine,100,fp);
			for(j=0;j<VALFMT_num;j++)
			{
				if(Node<NNZERO)
				{
					temp[VALFMT_chars] = '\0';NZValue = atof(strncpy(temp,WorkingLine + (VALFMT_chars*j),VALFMT_chars));
					if(NZValue != 0.0)
					{
						ColumnPointers[ArrayOfNodes[Node].Col+1]++;
						ArrayOfNodes[VNode].Row = ArrayOfNodes[Node].Row;
						ArrayOfNodes[VNode].Col = ArrayOfNodes[Node].Col;
						ArrayOfNodes[VNode].next = NULL;
						ArrayOfNodes[VNode].value = NZValue; // FIX
						VNode++;
					}
					Node++;
					if(NZValue==0.0)ZerosInFile++;
				}
			}
		}
		poski_printMessage1(3, "%sdone%s :",CRT_DONE,CRT_RESET);
		poski_printMessage(3, "%d nonzeros, %d total\n",VNode,Node);
		for(i=0;i<NCOL;i++)ColumnPointers[i+1] += ColumnPointers[i];
	}
#endif

	// Link nodes in node array into row pointer array - - - - - - - - - - - - - - - - - - - -
	poski_printMessage(3, "=> Internal conversion from CSC to CSR...");
	NonZeroNode ** ArrayOfLinkedListsOfNodes;
	ArrayOfLinkedListsOfNodes = poski_malloc(NonZeroNode *, (SpA->nrows), 0);
	for(i=0;i<(poski_index_t)SpA->nrows;i++)ArrayOfLinkedListsOfNodes[i] = NULL;
	for(i=(VNode-1);i>=0;i--)
	{
		ArrayOfNodes[i].next = ArrayOfLinkedListsOfNodes[ArrayOfNodes[i].Row];
		ArrayOfLinkedListsOfNodes[ArrayOfNodes[i].Row] = &(ArrayOfNodes[i]);
	}
	poski_printMessage1(3, "%sdone%s\n",CRT_DONE,CRT_RESET);

	// Create SpA's structures & copy data into them - - - - - - - - - - - - - -
	// allocate RowPointers
	poski_printMessage(3, "=> Allocating Row Pointer Structures...");
	SpA->Aptr = poski_malloc(poski_index_t, (SpA->nrows+1), 0); 
	poski_printMessage1(3, "%sdone%s\n",CRT_DONE,CRT_RESET);

	// update RowPointers - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	poski_printMessage(3, "=> Counting actual nonzeros...");
	SpA->Aptr[0] = 0;
	for(i=0;i<(poski_index_t)SpA->nrows;i++)
	{
		SpA->Aptr[i+1] = SpA->Aptr[i];
		NonZeroNode * tPtr = ArrayOfLinkedListsOfNodes[i];
		while(tPtr)
		{
			if(tPtr->Col >= SpA->ncols){poski_error("poski_LoadMatrix_HBF_to_CSR", "NNZ.col too big - (%d,%d)\n",i,tPtr->Col); assert(0);}
			SpA->Aptr[i+1]++; 
			tPtr = tPtr->next;
		}

		if((i<NROW)&&(MakeUnsymmetric)&&(MXTYPE[1] == 'S'))
		{
			for(j=ColumnPointers[i];j<ColumnPointers[i+1];j++)
			{
				if((ArrayOfNodes[j].Row != ArrayOfNodes[j].Col)
#ifdef READ_VALUES
						&&(ArrayOfNodes[j].value != 0.0)
#endif
				  )
				{
					if(ArrayOfNodes[j].Row >= SpA->ncols){poski_error("poski_LoadMatrix_HBF_to_CSR", "transposed NNZ.row too big - (%d,%d)\n",i,ArrayOfNodes[j].Row); assert(0);}
					if(ArrayOfNodes[j].Row <           i){poski_error("poski_LoadMatrix_HBF_to_CSR", "transposed NNZ.row too small - (%d,%d)\n",ArrayOfNodes[j].Row,i); assert(0);}
					SpA->Aptr[i+1]++; 
				}
			}
		}
	} 
	poski_printMessage1(3, "%sdone%s(%d)\n",CRT_DONE,CRT_RESET,SpA->Aptr[SpA->nrows]);

	// allocate col's & values (function of size of blocked column) - - - - - - - - - - - - - - - - - - - -
	poski_printMessage(3, "=> Allocating Column and Data Arrays...");
	SpA->Aval = poski_malloc(poski_value_t,SpA->Aptr[SpA->nrows], 0); 
	SpA->Aind = poski_malloc(poski_index_t,SpA->Aptr[SpA->nrows], 0); 
	poski_printMessage1(3, "%sdone%s\n",CRT_DONE,CRT_RESET);

	// for all NZ's in table, add to SpA's col/value pointers - - - - - - - - - - - - - - - - - - - - - - - 
	poski_printMessage(3, "=> Copying NonZeros into Column and Data Arrays...");
	poski_value_t *VPtr=SpA->Aval;
	poski_index_t *CPtr=SpA->Aind;

	SpA->nnz = 0;
	SpA->Aptr[0] = 0;
	for(i=0;i<(poski_index_t)SpA->nrows;i++)
	{
		SpA->Aptr[i+1] = SpA->Aptr[i];
		NonZeroNode * tPtr = ArrayOfLinkedListsOfNodes[i];
		while(tPtr)
		{
			if(tPtr->Col >= SpA->ncols){poski_error("poski_LoadMatrix_HBF_to_CSR", "NNZ.col too big - (%d,%d)\n",i,tPtr->Col); assert(0);}
			if((tPtr->Col > i)&&(MXTYPE[1] == 'S')){poski_error("poski_LoadMatrix_HBF_to_CSR", "NNZ.col too big for symmetric - (%d,%d)\n",i,tPtr->Col); assert(0);}
			SpA->Aptr[i+1]++;  
#ifdef READ_VALUES
			*VPtr = tPtr->value;
#else
#ifdef VERIFY
			*VPtr = 1.0*(1+rand()%1000);
#else
			*VPtr = 1.0;
#endif
#endif
			if(*VPtr!=0.0)SpA->nnz++;
			VPtr++;
			*CPtr = tPtr->Col;  CPtr++;
			tPtr = tPtr->next;
		}
		if((i<NROW)&&(MakeUnsymmetric)&&(MXTYPE[1] == 'S'))
		{
			for(j=ColumnPointers[i];j<ColumnPointers[i+1];j++)
			{
				if((ArrayOfNodes[j].Row != ArrayOfNodes[j].Col)
#ifdef READ_VALUES
						&&(ArrayOfNodes[j].value != 0.0)
#endif
				  )
				{
					if(ArrayOfNodes[j].Row >= SpA->ncols){poski_error("poski_LoadMatrix_HBF_to_CSR", "transposed NNZ.row too big - (%d,%d)\n",i,ArrayOfNodes[j].Row); assert(0);}
					if(ArrayOfNodes[j].Row <           i){poski_error("poski_LoadMatrix_HBF_to_CSR", "transposed NNZ.row too small - (%d,%d)\n",ArrayOfNodes[j].Row,i); assert(0);}
					SpA->Aptr[i+1]++;  
#ifdef READ_VALUES
					*VPtr = ArrayOfNodes[j].value;
#else
#ifdef VERIFY
					*VPtr = 1.0*(1+rand()%1000);
#else
					*VPtr = 1.0;
#endif
#endif
					if(*VPtr!=0.0)SpA->nnz++;
					VPtr++;
					*CPtr = ArrayOfNodes[j].Row;  CPtr++;
				}
			}
		}
	} 

	poski_printMessage1(3, "%sdone%s\n",CRT_DONE,CRT_RESET);

	// Cleanup - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	poski_printMessage(3, "=> Cleaning up temps...");
	fclose(fp);
	free(ArrayOfLinkedListsOfNodes);
	free(ArrayOfNodes);
	free(ColumnPointers);
	poski_printMessage1(3, "%sdone%s\n",CRT_DONE,CRT_RESET);

	return(SpA);
}
// Internal functions
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Test functions
/**
 * @brief Test the poski_LoadMatrix_HB_to_CSR's routine.
 *
 *  This routine is for testing the functionality of the poski_LoadMatrix_HB_to_CSR's routine.
 *
 * @return 0 if success, or an error message otherwise.
 */
int test_main_poski_LoadMatrix_HBF_to_CSR(char *fname)
{
	TEST_LINECUT(1);
	poski_printMessage(0, "\tTEST: checking poski_LoadMatrix_HBF_to_CSR...\n");

	char *matfile = NULL;

	if (matfile != NULL) 
	{
		poski_printMessage(0, "\tTEST: error on initializing matrix file!\n");
		exit(0);
	}
	
	matfile = fname;
	
	if (matfile == NULL)
	{	
		poski_printMessage(0, "\tTEST: error on initializing sparse matrix!\n");
		exit(0);
	}

	poski_sparse_matrix_t *SpA = NULL;
	
	if (SpA != NULL) 
	{
		poski_printMessage(0, "\tTEST: error on initializing sparse matrix!\n");
		exit(0);
	}

	SpA = poski_LoadMatrix_HBF_to_CSR(fname, 1);
	
	if (SpA == NULL) 
	{
		poski_printMessage(0, "\tTEST: error on poski_LoadMatrix_HBF_to_CSR!\n");
		exit(0);
	}
	
	free(SpA);

	poski_printMessage(0, "\tTEST: success on poski_LoadMatrix_HBF_to_CSR!\n");

	return 0;
}
// Test functions
//-----------------------------------------------------------------------------------------



