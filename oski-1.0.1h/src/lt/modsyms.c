/* Generated on Mon Jun 25 10:02:12 PDT 2007 by vuduc2 [robotron-l.llnl.gov] */
extern int oski_CloseBenchmarkData;
extern int oski_CloseHeurManager;
extern int oski_FindHeurRecord;
extern int oski_GetNumHeur;
extern int oski_InitHeurManager;
extern int oski_LookupHeur;
extern int oski_LookupHeurDescById;
extern int oski_LookupHeurId;
extern int oski_LookupHeurIdByNum;
extern int oski_LookupHeurIdMethod;
extern int oski_LookupHeurMethod;
extern int oski_OpenBenchmarkData;
extern int oski_ReadBenchmarkLine;
extern int oski_RegisterHeur;
extern int oski_Close;
extern int oski_Init;
extern int oski_ClearInMatPropSet;
extern int oski_CollectInMatProps;
extern int oski_CollectInMatProps_arr;
extern int oski_CollectInMatProps_va;
extern int oski_CompleteDefaultInMatPropSet;
extern int oski_DisplayInMatPropSet;
extern int oski_IsInMatPropSetConsistent;
extern int oski_AllocStructHintBlocksizes;
extern int oski_AllocStructHintDiaglens;
extern int oski_InitStructHint;
extern int oski_ResetStructHint;
extern int oski_ResetStructHintBlocksizes;
extern int oski_ResetStructHintDiaglens;
extern int FindFile;
extern int GetDirFromPath;
extern int oski_CallModuleInit;
extern int oski_CloseModule;
extern int oski_CloseModuleLoader;
extern int oski_InitModuleLoader;
extern int oski_LoadModule;
extern int oski_LoadModuleOptional;
extern int oski_LookupModuleMethod;
extern int oski_MakeModuleDesc;
extern int oski_MakeModuleName;
extern int oski_RegisterSiteModules;
extern int oski_UnloadModule;
extern int RegisterSiteModules;
extern int oski_LookupKernelInfo;
extern int oski_CreateTrace;
extern int oski_DestroyTrace;
extern int oski_GetNumCalls;
extern int oski_MatchesTraceRecord;
extern int oski_RecordCalls;
extern int oski_StringPrintf;
extern int oski_CloseMatTypeManager;
extern int oski_DisplayMatTypeInfo;
extern int oski_FindMatTypeRecord;
extern int oski_GetNumMatTypes;
extern int oski_InitMatTypeManager;
extern int oski_LookupMatTypeId;
extern int oski_LookupMatTypeIdMethod;
extern int oski_LookupMatTypeMethod;
extern int oski_RegisterMatType;
extern int oski_GetDebugLevel;
extern int oski_GetDebugOutput;
extern int oski_PrintDebugMessage;
extern int oski_PrintDebugMessageShort;
extern int oski_SetDebugLevel;
extern int oski_SetDebugOutput;
extern int simplelist_Append;
extern int simplelist_BeginIter;
extern int simplelist_Create;
extern int simplelist_Destroy;
extern int simplelist_GetElem;
extern int simplelist_GetIterId;
extern int simplelist_GetIterObj;
extern int simplelist_GetLastElem;
extern int simplelist_GetLength;
extern int simplelist_InitIter;
extern int simplelist_NextIter;
extern int oski_GetNumScalarIndexTypes;
extern int oski_GetNumScalarValueTypes;
extern int oski_GetScalarIndexName;
extern int oski_GetScalarIndexTag;
extern int oski_GetScalarValueName;
extern int oski_GetScalarValueTag;
extern int oski_LookupScalarIndexInfo;
extern int oski_LookupScalarValueInfo;
extern int oski_DuplicateString;
extern int oski_FreeAll;
extern int oski_FreeInternal;
extern int oski_GetFree;
extern int oski_GetMalloc;
extern int oski_GetRealloc;
extern int oski_MallocInternal;
extern int oski_MultiMalloc;
extern int oski_ReallocInternal;
extern int oski_SetFree;
extern int oski_SetMalloc;
extern int oski_SetRealloc;
extern int oski_ZeroMem;
extern int oski_GetErrorHandler;
extern int oski_GetErrorName;
extern int oski_HandleErrorDefault;
extern int oski_HandleErrorNull;
extern int oski_SetErrorHandler;
extern int oski_CalibrateTimer;
extern int oski_CreateTimer;
extern int oski_DestroyTimer;
extern int oski_GetTimerDesc;
extern int oski_GetTimerSecsPerTick;
extern int oski_ReadElapsedTime;
extern int oski_RestartTimer;
extern int oski_StopTimer;
extern int luaA_pushobject;
extern int lua_atpanic;
extern int lua_call;
extern int lua_checkstack;
extern int lua_concat;
extern int lua_cpcall;
extern int lua_dump;
extern int lua_equal;
extern int lua_error;
extern int lua_getfenv;
extern int lua_getgccount;
extern int lua_getgcthreshold;
extern int lua_getmetatable;
extern int lua_gettable;
extern int lua_gettop;
extern int lua_getupvalue;
extern int lua_ident;
extern int lua_insert;
extern int lua_iscfunction;
extern int lua_isnumber;
extern int lua_isstring;
extern int lua_isuserdata;
extern int lua_lessthan;
extern int lua_load;
extern int lua_newtable;
extern int lua_newthread;
extern int lua_newuserdata;
extern int lua_next;
extern int lua_pcall;
extern int lua_pushboolean;
extern int lua_pushcclosure;
extern int lua_pushfstring;
extern int lua_pushlightuserdata;
extern int lua_pushlstring;
extern int lua_pushnil;
extern int lua_pushnumber;
extern int lua_pushstring;
extern int lua_pushupvalues;
extern int lua_pushvalue;
extern int lua_pushvfstring;
extern int lua_rawequal;
extern int lua_rawget;
extern int lua_rawgeti;
extern int lua_rawset;
extern int lua_rawseti;
extern int lua_remove;
extern int lua_replace;
extern int lua_setfenv;
extern int lua_setgcthreshold;
extern int lua_setmetatable;
extern int lua_settable;
extern int lua_settop;
extern int lua_setupvalue;
extern int lua_strlen;
extern int lua_toboolean;
extern int lua_tocfunction;
extern int lua_tonumber;
extern int lua_topointer;
extern int lua_tostring;
extern int lua_tothread;
extern int lua_touserdata;
extern int lua_type;
extern int lua_typename;
extern int lua_version;
extern int lua_xmove;
extern int luaH_free;
extern int luaH_get;
extern int luaH_getnum;
extern int luaH_getstr;
extern int luaH_mainposition;
extern int luaH_new;
extern int luaH_next;
extern int luaH_set;
extern int luaH_setnum;
extern int luaC_callGCTM;
extern int luaC_collectgarbage;
extern int luaC_link;
extern int luaC_separateudata;
extern int luaC_sweep;
extern int luaS_freeall;
extern int luaS_newlstr;
extern int luaS_newudata;
extern int luaS_resize;
extern int luaD_call;
extern int luaD_callhook;
extern int luaD_growstack;
extern int luaD_pcall;
extern int luaD_poscall;
extern int luaD_precall;
extern int luaD_protectedparser;
extern int luaD_rawrunprotected;
extern int luaD_reallocCI;
extern int luaD_reallocstack;
extern int luaD_throw;
extern int lua_resume;
extern int lua_yield;
extern int luaV_concat;
extern int luaV_equalval;
extern int luaV_execute;
extern int luaV_gettable;
extern int luaV_lessthan;
extern int luaV_settable;
extern int luaV_tonumber;
extern int luaV_tostring;
extern int luaU_endianness;
extern int luaU_undump;
extern int luaG_aritherror;
extern int luaG_checkcode;
extern int luaG_concaterror;
extern int luaG_errormsg;
extern int lua_gethook;
extern int lua_gethookcount;
extern int lua_gethookmask;
extern int lua_getinfo;
extern int lua_getlocal;
extern int lua_getstack;
extern int luaG_inithooks;
extern int luaG_ordererror;
extern int luaG_runerror;
extern int luaG_typeerror;
extern int lua_sethook;
extern int lua_setlocal;
extern int luaX_checklimit;
extern int luaX_errorline;
extern int luaX_init;
extern int luaX_lex;
extern int luaX_setinput;
extern int luaX_syntaxerror;
extern int luaX_token2str;
extern int luaM_growaux;
extern int luaM_realloc;
extern int luaP_opmodes;
extern int luaK_checkstack;
extern int luaK_code;
extern int luaK_codeABC;
extern int luaK_codeABx;
extern int luaK_concat;
extern int luaK_dischargevars;
extern int luaK_exp2anyreg;
extern int luaK_exp2nextreg;
extern int luaK_exp2RK;
extern int luaK_exp2val;
extern int luaK_fixline;
extern int luaK_getlabel;
extern int luaK_goiffalse;
extern int luaK_goiftrue;
extern int luaK_indexed;
extern int luaK_infix;
extern int luaK_jump;
extern int luaK_nil;
extern int luaK_numberK;
extern int luaK_patchlist;
extern int luaK_patchtohere;
extern int luaK_posfix;
extern int luaK_prefix;
extern int luaK_reserveregs;
extern int luaK_self;
extern int luaK_setcallreturns;
extern int luaK_storevar;
extern int luaK_stringK;
extern int luaT_gettm;
extern int luaT_gettmbyobj;
extern int luaT_init;
extern int luaT_typenames;
extern int luaO_chunkid;
extern int luaO_int2fb;
extern int luaO_log2;
extern int luaO_nilobject;
extern int luaO_pushfstring;
extern int luaO_pushvfstring;
extern int luaO_rawequalObj;
extern int luaO_str2d;
extern int luaF_close;
extern int luaF_findupval;
extern int luaF_freeclosure;
extern int luaF_freeproto;
extern int luaF_getlocalname;
extern int luaF_newCclosure;
extern int luaF_newLclosure;
extern int luaF_newproto;
extern int lua_close;
extern int luaE_freethread;
extern int luaE_newthread;
extern int lua_open;
extern int luaZ_fill;
extern int luaZ_init;
extern int luaZ_lookahead;
extern int luaZ_openspace;
extern int luaZ_read;
extern int luaU_dump;
extern int luaY_parser;
extern int luaopen_base;
extern int lua_dobuffer;
extern int lua_dofile;
extern int lua_dostring;
extern int luaL_addlstring;
extern int luaL_addstring;
extern int luaL_addvalue;
extern int luaL_argerror;
extern int luaL_buffinit;
extern int luaL_callmeta;
extern int luaL_checkany;
extern int luaL_checklstring;
extern int luaL_checknumber;
extern int luaL_checkstack;
extern int luaL_checktype;
extern int luaL_checkudata;
extern int luaL_error;
extern int luaL_findstring;
extern int luaL_getmetafield;
extern int luaL_getmetatable;
extern int luaL_getn;
extern int luaL_loadbuffer;
extern int luaL_loadfile;
extern int luaL_newmetatable;
extern int luaL_openlib;
extern int luaL_optlstring;
extern int luaL_optnumber;
extern int luaL_prepbuffer;
extern int luaL_pushresult;
extern int luaL_ref;
extern int luaL_setn;
extern int luaL_typerror;
extern int luaL_unref;
extern int luaL_where;
extern int luaopen_loadlib;
extern int luaopen_string;
extern int luaopen_table;
extern int luaopen_io;
extern int luaopen_math;
extern int luaopen_debug;
extern int lt_dladderror;
extern int lt_dladdsearchdir;
extern int lt_dlcaller_get_data;
extern int lt_dlcaller_register;
extern int lt_dlcaller_set_data;
extern int lt_dlclose;
extern int lt_dlerror;
extern int lt_dlexit;
extern int lt_dlforeach;
extern int lt_dlforeachfile;
extern int lt_dlfree;
extern int lt_dlgetinfo;
extern int lt_dlgetsearchpath;
extern int lt_dlhandle_next;
extern int lt_dlinit;
extern int lt_dlinsertsearchdir;
extern int lt_dlisresident;
extern int lt_dlloader_add;
extern int lt_dlloader_data;
extern int lt_dlloader_find;
extern int lt_dlloader_name;
extern int lt_dlloader_next;
extern int lt_dlloader_remove;
extern int lt_dlmakeresident;
extern int lt_dlmalloc;
extern int lt_dlmutex_register;
extern int lt_dlopen;
extern int lt_dlopenext;
extern int lt_dlpreload;
extern int lt_dlpreload_default;
extern int lt_dlrealloc;
extern int lt_dlseterror;
extern int lt_dlsetsearchpath;
extern int lt_dlsym;
extern int oski_CheckArgsMatMultAndMatTransMult_Tid;
extern int oski_MakeArglistMatMultAndMatTransMult_Tid;
extern int oski_MatMultAndMatTransMult_Tid;
extern int oski_CheckArgsMatTransMatMult_Tid;
extern int oski_MakeArglistMatTransMatMult_Tid;
extern int oski_MatTransMatMult_Tid;
extern int oski_GetMatClique_Tid;
extern int oski_GetMatDiagValues_Tid;
extern int oski_GetMatEntry_Tid;
extern int oski_SetMatClique_Tid;
extern int oski_SetMatDiagValues_Tid;
extern int oski_SetMatEntry_Tid;
extern int oski_ApplyHeurResults_Tid;
extern int oski_EstimateHeurCost_Tid;
extern int oski_EvaluateHeur_Tid;
extern int oski_IsHeurApplicable_Tid;
extern int oski_SetHintMatMultAndMatTransMult_Tid;
extern int oski_SetHintMatMult_Tid;
extern int oski_SetHintMatPowMult_Tid;
extern int oski_SetHintMatTransMatMult_Tid;
extern int oski_SetHintMatTrisolve_Tid;
extern int oski_SetHint_Tid;
extern int oski_CalcMat1Norm_Tid;
extern int oski_CopyMat_Tid;
extern int oski_CreateMatCSC_arr_Tid;
extern int oski_CreateMatCSC_Tid;
extern int oski_CreateMatCSC_va_Tid;
extern int oski_CreateMatCSR_arr_Tid;
extern int oski_CreateMatCSR_Tid;
extern int oski_CreateMatCSR_va_Tid;
extern int oski_DestroyMat_Tid;
extern int oski_GetMatProps_Tid;
extern int oski_CheckArgsMatMult_Tid;
extern int oski_MakeArglistMatMult_Tid;
extern int oski_MatMult_Tid;
extern int oski_CheckArgsMatPowMult_Tid;
extern int oski_MakeArglistMatPowMult_Tid;
extern int oski_MatPowMult_Tid;
extern int oski_CreateMatStruct_Tid;
extern int oski_DestroyMatStruct_Tid;
extern int oski_IsMatPermuted_Tid;
extern int oski_PermuteVecView_Tid;
extern int oski_ViewPermutedMatColPerm_Tid;
extern int oski_ViewPermutedMatRowPerm_Tid;
extern int oski_ViewPermutedMat_Tid;
extern int PERM_IDENTITY_Tid;
extern int oski_CountTraceFlopsPerNz_Tid;
extern int oski_EstimateTraceTime_Tid;
extern int oski_EstimateTraceTimeTotal_Tid;
extern int oski_CheckArgsMatTrisolve_Tid;
extern int oski_MakeArglistMatTrisolve_Tid;
extern int oski_MatTrisolve_Tid;
extern int oski_TuneMat_Tid;
extern int oski_CalcVecViewInfNormDiff_Tid;
extern int oski_CalcVecViewInfNorm_Tid;
extern int oski_CopyVecView_Tid;
extern int oski_CreateMultiVecView_Tid;
extern int oski_CreateVecView_Tid;
extern int oski_DestroyVecView_Tid;
extern int oski_InitSubVecView_Tid;
extern int oski_PrintDebugVecView_Tid;
extern int SYMBOLIC_MULTIVEC_Tid;
extern int SYMBOLIC_VEC_Tid;
extern int oski_ApplyMatTransforms_Tid;
extern int oski_CreateLuaMatReprGeneric2IndexFromCSR_Tid;
extern int oski_CreateLuaMatReprGenericFromCSR_Tid;
extern int oski_FreeInputMatRepr_Tid;
extern int oski_GetMatTransforms_Tid;
extern int oski_ReplaceTunedMatRepr_Tid;
extern int dtrsv_;
extern int TVAL_IMAG_Tid;
extern int TVAL_NEG_IMAG_Tid;
extern int TVAL_NEG_ONE_Tid;
extern int TVAL_ONE_Tid;
extern int TVAL_ZERO_Tid;
extern int daxpy_;
extern int dscal_;
extern int oski_AXPYVecView_Tid;
extern int oski_ConjVecView_Tid;
extern int oski_RectScaledIdentityMult_Tid;
extern int oski_ScaleVecView_Tid;
extern int oski_ZeroVecView_Tid;
extern int oski_GetStackLuaMat_Tid;
extern int oski_PushEmptyLuaMat_Tid;
extern int dgemv_;
extern int oski_TransposePatternProp_Tid;
extern int oski_TransposeProps_Tid;
extern int oski_DestroyBCSRFillProfile_Tid;
extern int oski_EstimateFillBCSR_Tid;
extern int liboski_mat_CSR_Tid_LTX_oski_CalcMatRepr1Norm;
extern int liboski_mat_CSR_Tid_LTX_oski_ChangeIndexBase;
extern int liboski_mat_CSR_Tid_LTX_oski_CloseModule;
extern int liboski_mat_CSR_Tid_LTX_oski_ConvertMatReprToCSR;
extern int liboski_mat_CSR_Tid_LTX_oski_CopyMatRepr;
extern int liboski_mat_CSR_Tid_LTX_oski_CountZeroRowsCSR;
extern int liboski_mat_CSR_Tid_LTX_oski_CreateMatReprFromCSR;
extern int liboski_mat_CSR_Tid_LTX_oski_DestroyMatRepr;
extern int liboski_mat_CSR_Tid_LTX_oski_GetLongDesc;
extern int liboski_mat_CSR_Tid_LTX_oski_GetShortDesc;
extern int liboski_mat_CSR_Tid_LTX_oski_InitModule;
extern int liboski_mat_CSR_Tid_LTX_oski_StreamMatRepr;
extern int liboski_mat_CSR_Tid_LTX_oski_WrapCSR;
extern int liboski_mat_CSR_Tid_LTX_oski_CheckCSR;
extern int liboski_mat_CSR_Tid_LTX_oski_ConditionallyExpandCSRToFull;
extern int liboski_mat_CSR_Tid_LTX_oski_ExpandCSRToFull;
extern int liboski_mat_CSR_Tid_LTX_oski_SortIndices;
extern int liboski_mat_CSR_Tid_LTX_oski_TransposeCSR;
extern int liboski_mat_CSR_Tid_LTX_oski_GetMatReprEntry;
extern int liboski_mat_CSR_Tid_LTX_oski_SetMatReprEntry;
extern int liboski_mat_CSR_Tid_LTX_oski_MatReprMult;
extern int liboski_mat_CSR_Tid_LTX_oski_MatReprTrisolve;
extern int CSR_MatTransMult_v1_a1_b1_xsX_ysX_Tid;
extern int CSR_MatMult_v1_aX_b1_xs1_ysX_Tid;
extern int CSR_MatTransMult_v1_aX_b1_xsX_ys1_Tid;
extern int CSR_MatMult_v1_a1_b1_xs1_ysX_Tid;
extern int CSR_MatTransMult_v1_a1_b1_xsX_ys1_Tid;
extern int CSR_MatTransMult_v1_aN1_b1_xsX_ys1_Tid;
extern int CSR_MatTransMult_v1_aN1_b1_xsX_ysX_Tid;
extern int CSR_MatMult_v1_a1_b1_xsX_ysX_Tid;
extern int CSR_MatMult_v1_aX_b1_xsX_ysX_Tid;
extern int CSR_MatMult_v1_aN1_b1_xs1_ysX_Tid;
extern int CSR_MatTransMult_v1_aX_b1_xsX_ysX_Tid;
extern int CSR_MatMult_v1_aN1_b1_xsX_ysX_Tid;
extern int CSR_SymmMatMult_v1_a1_b1_xs1_ysX_Tid;
extern int CSR_SymmMatMult_v1_aN1_b1_xs1_ys1_Tid;
extern int CSR_SymmMatMult_v1_aN1_b1_xs1_ysX_Tid;
extern int CSR_SymmMatMult_v1_aN1_b1_xsX_ysX_Tid;
extern int CSR_SymmMatMult_v1_a1_b1_xs1_ys1_Tid;
extern int CSR_SymmMatMult_v1_aX_b1_xs1_ys1_Tid;
extern int CSR_SymmMatMult_v1_aX_b1_xsX_ysX_Tid;
extern int CSR_SymmMatMult_v1_aX_b1_xs1_ysX_Tid;
extern int CSR_SymmMatMult_v1_a1_b1_xsX_ys1_Tid;
extern int CSR_SymmMatMult_v1_aX_b1_xsX_ys1_Tid;
extern int CSR_SymmMatMult_v1_aN1_b1_xsX_ys1_Tid;
extern int CSR_SymmMatMult_v1_a1_b1_xsX_ysX_Tid;
extern int CSR_MatTransTrisolveLower_Tid;
extern int CSR_MatConjTrisolveUpper_Tid;
extern int CSR_MatConjTransTrisolveUpper_Tid;
extern int CSR_MatConjTrisolveLower_Tid;
extern int CSR_MatTransTrisolveUpper_Tid;
extern int CSR_MatConjTransTrisolveLower_Tid;
extern int CSR_MatTrisolveUpper_Tid;
extern int CSR_MatTrisolveLower_Tid;
extern int liboski_mat_CSC_Tid_LTX_oski_CloseModule;
extern int liboski_mat_CSC_Tid_LTX_oski_ConvertMatReprToCSR;
extern int liboski_mat_CSC_Tid_LTX_oski_CopyMatRepr;
extern int liboski_mat_CSC_Tid_LTX_oski_CreateLuaMatReprFromCSR;
extern int liboski_mat_CSC_Tid_LTX_oski_CreateMatReprFromCSR;
extern int liboski_mat_CSC_Tid_LTX_oski_DestroyMatRepr;
extern int liboski_mat_CSC_Tid_LTX_oski_GetLongDesc;
extern int liboski_mat_CSC_Tid_LTX_oski_GetShortDesc;
extern int liboski_mat_CSC_Tid_LTX_oski_InitModule;
extern int liboski_mat_CSC_Tid_LTX_oski_WrapCSC;
extern int liboski_mat_CSC_Tid_LTX_oski_MatReprMult;
extern int liboski_mat_CSC_Tid_LTX_oski_GetMatReprEntry;
extern int liboski_mat_CSC_Tid_LTX_oski_SetMatReprEntry;
extern int liboski_mat_CSC_Tid_LTX_oski_MatReprTrisolve;
extern int oski_CheckArgsMatMultAndMatTransMult_Tld;
extern int oski_MakeArglistMatMultAndMatTransMult_Tld;
extern int oski_MatMultAndMatTransMult_Tld;
extern int oski_CheckArgsMatTransMatMult_Tld;
extern int oski_MakeArglistMatTransMatMult_Tld;
extern int oski_MatTransMatMult_Tld;
extern int oski_GetMatClique_Tld;
extern int oski_GetMatDiagValues_Tld;
extern int oski_GetMatEntry_Tld;
extern int oski_SetMatClique_Tld;
extern int oski_SetMatDiagValues_Tld;
extern int oski_SetMatEntry_Tld;
extern int oski_ApplyHeurResults_Tld;
extern int oski_EstimateHeurCost_Tld;
extern int oski_EvaluateHeur_Tld;
extern int oski_IsHeurApplicable_Tld;
extern int oski_SetHintMatMultAndMatTransMult_Tld;
extern int oski_SetHintMatMult_Tld;
extern int oski_SetHintMatPowMult_Tld;
extern int oski_SetHintMatTransMatMult_Tld;
extern int oski_SetHintMatTrisolve_Tld;
extern int oski_SetHint_Tld;
extern int oski_CalcMat1Norm_Tld;
extern int oski_CopyMat_Tld;
extern int oski_CreateMatCSC_arr_Tld;
extern int oski_CreateMatCSC_Tld;
extern int oski_CreateMatCSC_va_Tld;
extern int oski_CreateMatCSR_arr_Tld;
extern int oski_CreateMatCSR_Tld;
extern int oski_CreateMatCSR_va_Tld;
extern int oski_DestroyMat_Tld;
extern int oski_GetMatProps_Tld;
extern int oski_CheckArgsMatMult_Tld;
extern int oski_MakeArglistMatMult_Tld;
extern int oski_MatMult_Tld;
extern int oski_CheckArgsMatPowMult_Tld;
extern int oski_MakeArglistMatPowMult_Tld;
extern int oski_MatPowMult_Tld;
extern int oski_CreateMatStruct_Tld;
extern int oski_DestroyMatStruct_Tld;
extern int oski_IsMatPermuted_Tld;
extern int oski_PermuteVecView_Tld;
extern int oski_ViewPermutedMatColPerm_Tld;
extern int oski_ViewPermutedMatRowPerm_Tld;
extern int oski_ViewPermutedMat_Tld;
extern int PERM_IDENTITY_Tld;
extern int oski_CountTraceFlopsPerNz_Tld;
extern int oski_EstimateTraceTime_Tld;
extern int oski_EstimateTraceTimeTotal_Tld;
extern int oski_CheckArgsMatTrisolve_Tld;
extern int oski_MakeArglistMatTrisolve_Tld;
extern int oski_MatTrisolve_Tld;
extern int oski_TuneMat_Tld;
extern int oski_CalcVecViewInfNormDiff_Tld;
extern int oski_CalcVecViewInfNorm_Tld;
extern int oski_CopyVecView_Tld;
extern int oski_CreateMultiVecView_Tld;
extern int oski_CreateVecView_Tld;
extern int oski_DestroyVecView_Tld;
extern int oski_InitSubVecView_Tld;
extern int oski_PrintDebugVecView_Tld;
extern int SYMBOLIC_MULTIVEC_Tld;
extern int SYMBOLIC_VEC_Tld;
extern int oski_ApplyMatTransforms_Tld;
extern int oski_CreateLuaMatReprGeneric2IndexFromCSR_Tld;
extern int oski_CreateLuaMatReprGenericFromCSR_Tld;
extern int oski_FreeInputMatRepr_Tld;
extern int oski_GetMatTransforms_Tld;
extern int oski_ReplaceTunedMatRepr_Tld;
extern int ldgemv_;
extern int TVAL_IMAG_Tld;
extern int TVAL_NEG_IMAG_Tld;
extern int TVAL_NEG_ONE_Tld;
extern int TVAL_ONE_Tld;
extern int TVAL_ZERO_Tld;
extern int ldaxpy_;
extern int ldscal_;
extern int oski_AXPYVecView_Tld;
extern int oski_ConjVecView_Tld;
extern int oski_RectScaledIdentityMult_Tld;
extern int oski_ScaleVecView_Tld;
extern int oski_ZeroVecView_Tld;
extern int ldtrsv_;
extern int oski_TransposePatternProp_Tld;
extern int oski_TransposeProps_Tld;
extern int oski_GetStackLuaMat_Tld;
extern int oski_PushEmptyLuaMat_Tld;
extern int oski_DestroyBCSRFillProfile_Tld;
extern int oski_EstimateFillBCSR_Tld;
extern int liboski_mat_CSR_Tld_LTX_oski_CalcMatRepr1Norm;
extern int liboski_mat_CSR_Tld_LTX_oski_ChangeIndexBase;
extern int liboski_mat_CSR_Tld_LTX_oski_CloseModule;
extern int liboski_mat_CSR_Tld_LTX_oski_ConvertMatReprToCSR;
extern int liboski_mat_CSR_Tld_LTX_oski_CopyMatRepr;
extern int liboski_mat_CSR_Tld_LTX_oski_CountZeroRowsCSR;
extern int liboski_mat_CSR_Tld_LTX_oski_CreateMatReprFromCSR;
extern int liboski_mat_CSR_Tld_LTX_oski_DestroyMatRepr;
extern int liboski_mat_CSR_Tld_LTX_oski_GetLongDesc;
extern int liboski_mat_CSR_Tld_LTX_oski_GetShortDesc;
extern int liboski_mat_CSR_Tld_LTX_oski_InitModule;
extern int liboski_mat_CSR_Tld_LTX_oski_StreamMatRepr;
extern int liboski_mat_CSR_Tld_LTX_oski_WrapCSR;
extern int liboski_mat_CSR_Tld_LTX_oski_CheckCSR;
extern int liboski_mat_CSR_Tld_LTX_oski_ConditionallyExpandCSRToFull;
extern int liboski_mat_CSR_Tld_LTX_oski_ExpandCSRToFull;
extern int liboski_mat_CSR_Tld_LTX_oski_SortIndices;
extern int liboski_mat_CSR_Tld_LTX_oski_TransposeCSR;
extern int liboski_mat_CSR_Tld_LTX_oski_GetMatReprEntry;
extern int liboski_mat_CSR_Tld_LTX_oski_SetMatReprEntry;
extern int liboski_mat_CSR_Tld_LTX_oski_MatReprMult;
extern int liboski_mat_CSR_Tld_LTX_oski_MatReprTrisolve;
extern int CSR_MatTransMult_v1_aN1_b1_xsX_ysX_Tld;
extern int CSR_MatMult_v1_a1_b1_xsX_ysX_Tld;
extern int CSR_MatTransMult_v1_a1_b1_xsX_ysX_Tld;
extern int CSR_MatMult_v1_aX_b1_xsX_ysX_Tld;
extern int CSR_MatTransMult_v1_aX_b1_xsX_ys1_Tld;
extern int CSR_MatMult_v1_a1_b1_xs1_ysX_Tld;
extern int CSR_MatMult_v1_aX_b1_xs1_ysX_Tld;
extern int CSR_MatTransMult_v1_a1_b1_xsX_ys1_Tld;
extern int CSR_MatTransMult_v1_aX_b1_xsX_ysX_Tld;
extern int CSR_MatMult_v1_aN1_b1_xs1_ysX_Tld;
extern int CSR_MatTransMult_v1_aN1_b1_xsX_ys1_Tld;
extern int CSR_MatMult_v1_aN1_b1_xsX_ysX_Tld;
extern int CSR_SymmMatMult_v1_a1_b1_xs1_ysX_Tld;
extern int CSR_SymmMatMult_v1_aX_b1_xs1_ys1_Tld;
extern int CSR_SymmMatMult_v1_aN1_b1_xsX_ysX_Tld;
extern int CSR_SymmMatMult_v1_aN1_b1_xsX_ys1_Tld;
extern int CSR_SymmMatMult_v1_aX_b1_xsX_ys1_Tld;
extern int CSR_SymmMatMult_v1_a1_b1_xsX_ys1_Tld;
extern int CSR_SymmMatMult_v1_aN1_b1_xs1_ys1_Tld;
extern int CSR_SymmMatMult_v1_aN1_b1_xs1_ysX_Tld;
extern int CSR_SymmMatMult_v1_aX_b1_xs1_ysX_Tld;
extern int CSR_SymmMatMult_v1_a1_b1_xsX_ysX_Tld;
extern int CSR_SymmMatMult_v1_aX_b1_xsX_ysX_Tld;
extern int CSR_SymmMatMult_v1_a1_b1_xs1_ys1_Tld;
extern int CSR_MatTrisolveLower_Tld;
extern int CSR_MatTransTrisolveUpper_Tld;
extern int CSR_MatConjTrisolveLower_Tld;
extern int CSR_MatTrisolveUpper_Tld;
extern int CSR_MatConjTransTrisolveUpper_Tld;
extern int CSR_MatConjTrisolveUpper_Tld;
extern int CSR_MatTransTrisolveLower_Tld;
extern int CSR_MatConjTransTrisolveLower_Tld;
extern int liboski_mat_CSC_Tld_LTX_oski_CloseModule;
extern int liboski_mat_CSC_Tld_LTX_oski_ConvertMatReprToCSR;
extern int liboski_mat_CSC_Tld_LTX_oski_CopyMatRepr;
extern int liboski_mat_CSC_Tld_LTX_oski_CreateLuaMatReprFromCSR;
extern int liboski_mat_CSC_Tld_LTX_oski_CreateMatReprFromCSR;
extern int liboski_mat_CSC_Tld_LTX_oski_DestroyMatRepr;
extern int liboski_mat_CSC_Tld_LTX_oski_GetLongDesc;
extern int liboski_mat_CSC_Tld_LTX_oski_GetShortDesc;
extern int liboski_mat_CSC_Tld_LTX_oski_InitModule;
extern int liboski_mat_CSC_Tld_LTX_oski_WrapCSC;
extern int liboski_mat_CSC_Tld_LTX_oski_MatReprMult;
extern int liboski_mat_CSC_Tld_LTX_oski_GetMatReprEntry;
extern int liboski_mat_CSC_Tld_LTX_oski_SetMatReprEntry;
extern int liboski_mat_CSC_Tld_LTX_oski_MatReprTrisolve;
struct { const char* name; void* addr; }
lt_preloaded_symbols[] = {
"liboski.a", 0x0,
"oski_CloseBenchmarkData", &oski_CloseBenchmarkData,
"oski_CloseHeurManager", &oski_CloseHeurManager,
"oski_FindHeurRecord", &oski_FindHeurRecord,
"oski_GetNumHeur", &oski_GetNumHeur,
"oski_InitHeurManager", &oski_InitHeurManager,
"oski_LookupHeur", &oski_LookupHeur,
"oski_LookupHeurDescById", &oski_LookupHeurDescById,
"oski_LookupHeurId", &oski_LookupHeurId,
"oski_LookupHeurIdByNum", &oski_LookupHeurIdByNum,
"oski_LookupHeurIdMethod", &oski_LookupHeurIdMethod,
"oski_LookupHeurMethod", &oski_LookupHeurMethod,
"oski_OpenBenchmarkData", &oski_OpenBenchmarkData,
"oski_ReadBenchmarkLine", &oski_ReadBenchmarkLine,
"oski_RegisterHeur", &oski_RegisterHeur,
"oski_Close", &oski_Close,
"oski_Init", &oski_Init,
"oski_ClearInMatPropSet", &oski_ClearInMatPropSet,
"oski_CollectInMatProps", &oski_CollectInMatProps,
"oski_CollectInMatProps_arr", &oski_CollectInMatProps_arr,
"oski_CollectInMatProps_va", &oski_CollectInMatProps_va,
"oski_CompleteDefaultInMatPropSet", &oski_CompleteDefaultInMatPropSet,
"oski_DisplayInMatPropSet", &oski_DisplayInMatPropSet,
"oski_IsInMatPropSetConsistent", &oski_IsInMatPropSetConsistent,
"oski_AllocStructHintBlocksizes", &oski_AllocStructHintBlocksizes,
"oski_AllocStructHintDiaglens", &oski_AllocStructHintDiaglens,
"oski_InitStructHint", &oski_InitStructHint,
"oski_ResetStructHint", &oski_ResetStructHint,
"oski_ResetStructHintBlocksizes", &oski_ResetStructHintBlocksizes,
"oski_ResetStructHintDiaglens", &oski_ResetStructHintDiaglens,
"FindFile", &FindFile,
"GetDirFromPath", &GetDirFromPath,
"oski_CallModuleInit", &oski_CallModuleInit,
"oski_CloseModule", &oski_CloseModule,
"oski_CloseModuleLoader", &oski_CloseModuleLoader,
"oski_InitModuleLoader", &oski_InitModuleLoader,
"oski_LoadModule", &oski_LoadModule,
"oski_LoadModuleOptional", &oski_LoadModuleOptional,
"oski_LookupModuleMethod", &oski_LookupModuleMethod,
"oski_MakeModuleDesc", &oski_MakeModuleDesc,
"oski_MakeModuleName", &oski_MakeModuleName,
"oski_RegisterSiteModules", &oski_RegisterSiteModules,
"oski_UnloadModule", &oski_UnloadModule,
"RegisterSiteModules", &RegisterSiteModules,
"oski_LookupKernelInfo", &oski_LookupKernelInfo,
"oski_CreateTrace", &oski_CreateTrace,
"oski_DestroyTrace", &oski_DestroyTrace,
"oski_GetNumCalls", &oski_GetNumCalls,
"oski_MatchesTraceRecord", &oski_MatchesTraceRecord,
"oski_RecordCalls", &oski_RecordCalls,
"oski_StringPrintf", &oski_StringPrintf,
"oski_CloseMatTypeManager", &oski_CloseMatTypeManager,
"oski_DisplayMatTypeInfo", &oski_DisplayMatTypeInfo,
"oski_FindMatTypeRecord", &oski_FindMatTypeRecord,
"oski_GetNumMatTypes", &oski_GetNumMatTypes,
"oski_InitMatTypeManager", &oski_InitMatTypeManager,
"oski_LookupMatTypeId", &oski_LookupMatTypeId,
"oski_LookupMatTypeIdMethod", &oski_LookupMatTypeIdMethod,
"oski_LookupMatTypeMethod", &oski_LookupMatTypeMethod,
"oski_RegisterMatType", &oski_RegisterMatType,
"oski_GetDebugLevel", &oski_GetDebugLevel,
"oski_GetDebugOutput", &oski_GetDebugOutput,
"oski_PrintDebugMessage", &oski_PrintDebugMessage,
"oski_PrintDebugMessageShort", &oski_PrintDebugMessageShort,
"oski_SetDebugLevel", &oski_SetDebugLevel,
"oski_SetDebugOutput", &oski_SetDebugOutput,
"simplelist_Append", &simplelist_Append,
"simplelist_BeginIter", &simplelist_BeginIter,
"simplelist_Create", &simplelist_Create,
"simplelist_Destroy", &simplelist_Destroy,
"simplelist_GetElem", &simplelist_GetElem,
"simplelist_GetIterId", &simplelist_GetIterId,
"simplelist_GetIterObj", &simplelist_GetIterObj,
"simplelist_GetLastElem", &simplelist_GetLastElem,
"simplelist_GetLength", &simplelist_GetLength,
"simplelist_InitIter", &simplelist_InitIter,
"simplelist_NextIter", &simplelist_NextIter,
"oski_GetNumScalarIndexTypes", &oski_GetNumScalarIndexTypes,
"oski_GetNumScalarValueTypes", &oski_GetNumScalarValueTypes,
"oski_GetScalarIndexName", &oski_GetScalarIndexName,
"oski_GetScalarIndexTag", &oski_GetScalarIndexTag,
"oski_GetScalarValueName", &oski_GetScalarValueName,
"oski_GetScalarValueTag", &oski_GetScalarValueTag,
"oski_LookupScalarIndexInfo", &oski_LookupScalarIndexInfo,
"oski_LookupScalarValueInfo", &oski_LookupScalarValueInfo,
"oski_DuplicateString", &oski_DuplicateString,
"oski_FreeAll", &oski_FreeAll,
"oski_FreeInternal", &oski_FreeInternal,
"oski_GetFree", &oski_GetFree,
"oski_GetMalloc", &oski_GetMalloc,
"oski_GetRealloc", &oski_GetRealloc,
"oski_MallocInternal", &oski_MallocInternal,
"oski_MultiMalloc", &oski_MultiMalloc,
"oski_ReallocInternal", &oski_ReallocInternal,
"oski_SetFree", &oski_SetFree,
"oski_SetMalloc", &oski_SetMalloc,
"oski_SetRealloc", &oski_SetRealloc,
"oski_ZeroMem", &oski_ZeroMem,
"oski_GetErrorHandler", &oski_GetErrorHandler,
"oski_GetErrorName", &oski_GetErrorName,
"oski_HandleErrorDefault", &oski_HandleErrorDefault,
"oski_HandleErrorNull", &oski_HandleErrorNull,
"oski_SetErrorHandler", &oski_SetErrorHandler,
"oski_CalibrateTimer", &oski_CalibrateTimer,
"oski_CreateTimer", &oski_CreateTimer,
"oski_DestroyTimer", &oski_DestroyTimer,
"oski_GetTimerDesc", &oski_GetTimerDesc,
"oski_GetTimerSecsPerTick", &oski_GetTimerSecsPerTick,
"oski_ReadElapsedTime", &oski_ReadElapsedTime,
"oski_RestartTimer", &oski_RestartTimer,
"oski_StopTimer", &oski_StopTimer,
"luaA_pushobject", &luaA_pushobject,
"lua_atpanic", &lua_atpanic,
"lua_call", &lua_call,
"lua_checkstack", &lua_checkstack,
"lua_concat", &lua_concat,
"lua_cpcall", &lua_cpcall,
"lua_dump", &lua_dump,
"lua_equal", &lua_equal,
"lua_error", &lua_error,
"lua_getfenv", &lua_getfenv,
"lua_getgccount", &lua_getgccount,
"lua_getgcthreshold", &lua_getgcthreshold,
"lua_getmetatable", &lua_getmetatable,
"lua_gettable", &lua_gettable,
"lua_gettop", &lua_gettop,
"lua_getupvalue", &lua_getupvalue,
"lua_ident", &lua_ident,
"lua_insert", &lua_insert,
"lua_iscfunction", &lua_iscfunction,
"lua_isnumber", &lua_isnumber,
"lua_isstring", &lua_isstring,
"lua_isuserdata", &lua_isuserdata,
"lua_lessthan", &lua_lessthan,
"lua_load", &lua_load,
"lua_newtable", &lua_newtable,
"lua_newthread", &lua_newthread,
"lua_newuserdata", &lua_newuserdata,
"lua_next", &lua_next,
"lua_pcall", &lua_pcall,
"lua_pushboolean", &lua_pushboolean,
"lua_pushcclosure", &lua_pushcclosure,
"lua_pushfstring", &lua_pushfstring,
"lua_pushlightuserdata", &lua_pushlightuserdata,
"lua_pushlstring", &lua_pushlstring,
"lua_pushnil", &lua_pushnil,
"lua_pushnumber", &lua_pushnumber,
"lua_pushstring", &lua_pushstring,
"lua_pushupvalues", &lua_pushupvalues,
"lua_pushvalue", &lua_pushvalue,
"lua_pushvfstring", &lua_pushvfstring,
"lua_rawequal", &lua_rawequal,
"lua_rawget", &lua_rawget,
"lua_rawgeti", &lua_rawgeti,
"lua_rawset", &lua_rawset,
"lua_rawseti", &lua_rawseti,
"lua_remove", &lua_remove,
"lua_replace", &lua_replace,
"lua_setfenv", &lua_setfenv,
"lua_setgcthreshold", &lua_setgcthreshold,
"lua_setmetatable", &lua_setmetatable,
"lua_settable", &lua_settable,
"lua_settop", &lua_settop,
"lua_setupvalue", &lua_setupvalue,
"lua_strlen", &lua_strlen,
"lua_toboolean", &lua_toboolean,
"lua_tocfunction", &lua_tocfunction,
"lua_tonumber", &lua_tonumber,
"lua_topointer", &lua_topointer,
"lua_tostring", &lua_tostring,
"lua_tothread", &lua_tothread,
"lua_touserdata", &lua_touserdata,
"lua_type", &lua_type,
"lua_typename", &lua_typename,
"lua_version", &lua_version,
"lua_xmove", &lua_xmove,
"luaH_free", &luaH_free,
"luaH_get", &luaH_get,
"luaH_getnum", &luaH_getnum,
"luaH_getstr", &luaH_getstr,
"luaH_mainposition", &luaH_mainposition,
"luaH_new", &luaH_new,
"luaH_next", &luaH_next,
"luaH_set", &luaH_set,
"luaH_setnum", &luaH_setnum,
"luaC_callGCTM", &luaC_callGCTM,
"luaC_collectgarbage", &luaC_collectgarbage,
"luaC_link", &luaC_link,
"luaC_separateudata", &luaC_separateudata,
"luaC_sweep", &luaC_sweep,
"luaS_freeall", &luaS_freeall,
"luaS_newlstr", &luaS_newlstr,
"luaS_newudata", &luaS_newudata,
"luaS_resize", &luaS_resize,
"luaD_call", &luaD_call,
"luaD_callhook", &luaD_callhook,
"luaD_growstack", &luaD_growstack,
"luaD_pcall", &luaD_pcall,
"luaD_poscall", &luaD_poscall,
"luaD_precall", &luaD_precall,
"luaD_protectedparser", &luaD_protectedparser,
"luaD_rawrunprotected", &luaD_rawrunprotected,
"luaD_reallocCI", &luaD_reallocCI,
"luaD_reallocstack", &luaD_reallocstack,
"luaD_throw", &luaD_throw,
"lua_resume", &lua_resume,
"lua_yield", &lua_yield,
"luaV_concat", &luaV_concat,
"luaV_equalval", &luaV_equalval,
"luaV_execute", &luaV_execute,
"luaV_gettable", &luaV_gettable,
"luaV_lessthan", &luaV_lessthan,
"luaV_settable", &luaV_settable,
"luaV_tonumber", &luaV_tonumber,
"luaV_tostring", &luaV_tostring,
"luaU_endianness", &luaU_endianness,
"luaU_undump", &luaU_undump,
"luaG_aritherror", &luaG_aritherror,
"luaG_checkcode", &luaG_checkcode,
"luaG_concaterror", &luaG_concaterror,
"luaG_errormsg", &luaG_errormsg,
"lua_gethook", &lua_gethook,
"lua_gethookcount", &lua_gethookcount,
"lua_gethookmask", &lua_gethookmask,
"lua_getinfo", &lua_getinfo,
"lua_getlocal", &lua_getlocal,
"lua_getstack", &lua_getstack,
"luaG_inithooks", &luaG_inithooks,
"luaG_ordererror", &luaG_ordererror,
"luaG_runerror", &luaG_runerror,
"luaG_typeerror", &luaG_typeerror,
"lua_sethook", &lua_sethook,
"lua_setlocal", &lua_setlocal,
"luaX_checklimit", &luaX_checklimit,
"luaX_errorline", &luaX_errorline,
"luaX_init", &luaX_init,
"luaX_lex", &luaX_lex,
"luaX_setinput", &luaX_setinput,
"luaX_syntaxerror", &luaX_syntaxerror,
"luaX_token2str", &luaX_token2str,
"luaM_growaux", &luaM_growaux,
"luaM_realloc", &luaM_realloc,
"luaP_opmodes", &luaP_opmodes,
"luaK_checkstack", &luaK_checkstack,
"luaK_code", &luaK_code,
"luaK_codeABC", &luaK_codeABC,
"luaK_codeABx", &luaK_codeABx,
"luaK_concat", &luaK_concat,
"luaK_dischargevars", &luaK_dischargevars,
"luaK_exp2anyreg", &luaK_exp2anyreg,
"luaK_exp2nextreg", &luaK_exp2nextreg,
"luaK_exp2RK", &luaK_exp2RK,
"luaK_exp2val", &luaK_exp2val,
"luaK_fixline", &luaK_fixline,
"luaK_getlabel", &luaK_getlabel,
"luaK_goiffalse", &luaK_goiffalse,
"luaK_goiftrue", &luaK_goiftrue,
"luaK_indexed", &luaK_indexed,
"luaK_infix", &luaK_infix,
"luaK_jump", &luaK_jump,
"luaK_nil", &luaK_nil,
"luaK_numberK", &luaK_numberK,
"luaK_patchlist", &luaK_patchlist,
"luaK_patchtohere", &luaK_patchtohere,
"luaK_posfix", &luaK_posfix,
"luaK_prefix", &luaK_prefix,
"luaK_reserveregs", &luaK_reserveregs,
"luaK_self", &luaK_self,
"luaK_setcallreturns", &luaK_setcallreturns,
"luaK_storevar", &luaK_storevar,
"luaK_stringK", &luaK_stringK,
"luaT_gettm", &luaT_gettm,
"luaT_gettmbyobj", &luaT_gettmbyobj,
"luaT_init", &luaT_init,
"luaT_typenames", &luaT_typenames,
"luaO_chunkid", &luaO_chunkid,
"luaO_int2fb", &luaO_int2fb,
"luaO_log2", &luaO_log2,
"luaO_nilobject", &luaO_nilobject,
"luaO_pushfstring", &luaO_pushfstring,
"luaO_pushvfstring", &luaO_pushvfstring,
"luaO_rawequalObj", &luaO_rawequalObj,
"luaO_str2d", &luaO_str2d,
"luaF_close", &luaF_close,
"luaF_findupval", &luaF_findupval,
"luaF_freeclosure", &luaF_freeclosure,
"luaF_freeproto", &luaF_freeproto,
"luaF_getlocalname", &luaF_getlocalname,
"luaF_newCclosure", &luaF_newCclosure,
"luaF_newLclosure", &luaF_newLclosure,
"luaF_newproto", &luaF_newproto,
"lua_close", &lua_close,
"luaE_freethread", &luaE_freethread,
"luaE_newthread", &luaE_newthread,
"lua_open", &lua_open,
"luaZ_fill", &luaZ_fill,
"luaZ_init", &luaZ_init,
"luaZ_lookahead", &luaZ_lookahead,
"luaZ_openspace", &luaZ_openspace,
"luaZ_read", &luaZ_read,
"luaU_dump", &luaU_dump,
"luaY_parser", &luaY_parser,
"luaopen_base", &luaopen_base,
"lua_dobuffer", &lua_dobuffer,
"lua_dofile", &lua_dofile,
"lua_dostring", &lua_dostring,
"luaL_addlstring", &luaL_addlstring,
"luaL_addstring", &luaL_addstring,
"luaL_addvalue", &luaL_addvalue,
"luaL_argerror", &luaL_argerror,
"luaL_buffinit", &luaL_buffinit,
"luaL_callmeta", &luaL_callmeta,
"luaL_checkany", &luaL_checkany,
"luaL_checklstring", &luaL_checklstring,
"luaL_checknumber", &luaL_checknumber,
"luaL_checkstack", &luaL_checkstack,
"luaL_checktype", &luaL_checktype,
"luaL_checkudata", &luaL_checkudata,
"luaL_error", &luaL_error,
"luaL_findstring", &luaL_findstring,
"luaL_getmetafield", &luaL_getmetafield,
"luaL_getmetatable", &luaL_getmetatable,
"luaL_getn", &luaL_getn,
"luaL_loadbuffer", &luaL_loadbuffer,
"luaL_loadfile", &luaL_loadfile,
"luaL_newmetatable", &luaL_newmetatable,
"luaL_openlib", &luaL_openlib,
"luaL_optlstring", &luaL_optlstring,
"luaL_optnumber", &luaL_optnumber,
"luaL_prepbuffer", &luaL_prepbuffer,
"luaL_pushresult", &luaL_pushresult,
"luaL_ref", &luaL_ref,
"luaL_setn", &luaL_setn,
"luaL_typerror", &luaL_typerror,
"luaL_unref", &luaL_unref,
"luaL_where", &luaL_where,
"luaopen_loadlib", &luaopen_loadlib,
"luaopen_string", &luaopen_string,
"luaopen_table", &luaopen_table,
"luaopen_io", &luaopen_io,
"luaopen_math", &luaopen_math,
"luaopen_debug", &luaopen_debug,
"lt_dladderror", &lt_dladderror,
"lt_dladdsearchdir", &lt_dladdsearchdir,
"lt_dlcaller_get_data", &lt_dlcaller_get_data,
"lt_dlcaller_register", &lt_dlcaller_register,
"lt_dlcaller_set_data", &lt_dlcaller_set_data,
"lt_dlclose", &lt_dlclose,
"lt_dlerror", &lt_dlerror,
"lt_dlexit", &lt_dlexit,
"lt_dlforeach", &lt_dlforeach,
"lt_dlforeachfile", &lt_dlforeachfile,
"lt_dlfree", &lt_dlfree,
"lt_dlgetinfo", &lt_dlgetinfo,
"lt_dlgetsearchpath", &lt_dlgetsearchpath,
"lt_dlhandle_next", &lt_dlhandle_next,
"lt_dlinit", &lt_dlinit,
"lt_dlinsertsearchdir", &lt_dlinsertsearchdir,
"lt_dlisresident", &lt_dlisresident,
"lt_dlloader_add", &lt_dlloader_add,
"lt_dlloader_data", &lt_dlloader_data,
"lt_dlloader_find", &lt_dlloader_find,
"lt_dlloader_name", &lt_dlloader_name,
"lt_dlloader_next", &lt_dlloader_next,
"lt_dlloader_remove", &lt_dlloader_remove,
"lt_dlmakeresident", &lt_dlmakeresident,
"lt_dlmalloc", &lt_dlmalloc,
"lt_dlmutex_register", &lt_dlmutex_register,
"lt_dlopen", &lt_dlopen,
"lt_dlopenext", &lt_dlopenext,
"lt_dlpreload", &lt_dlpreload,
"lt_dlpreload_default", &lt_dlpreload_default,
"lt_dlrealloc", &lt_dlrealloc,
"lt_dlseterror", &lt_dlseterror,
"lt_dlsetsearchpath", &lt_dlsetsearchpath,
"lt_dlsym", &lt_dlsym,
"liboski_Tid.a", 0x0,
"oski_CheckArgsMatMultAndMatTransMult_Tid", &oski_CheckArgsMatMultAndMatTransMult_Tid,
"oski_MakeArglistMatMultAndMatTransMult_Tid", &oski_MakeArglistMatMultAndMatTransMult_Tid,
"oski_MatMultAndMatTransMult_Tid", &oski_MatMultAndMatTransMult_Tid,
"oski_CheckArgsMatTransMatMult_Tid", &oski_CheckArgsMatTransMatMult_Tid,
"oski_MakeArglistMatTransMatMult_Tid", &oski_MakeArglistMatTransMatMult_Tid,
"oski_MatTransMatMult_Tid", &oski_MatTransMatMult_Tid,
"oski_GetMatClique_Tid", &oski_GetMatClique_Tid,
"oski_GetMatDiagValues_Tid", &oski_GetMatDiagValues_Tid,
"oski_GetMatEntry_Tid", &oski_GetMatEntry_Tid,
"oski_SetMatClique_Tid", &oski_SetMatClique_Tid,
"oski_SetMatDiagValues_Tid", &oski_SetMatDiagValues_Tid,
"oski_SetMatEntry_Tid", &oski_SetMatEntry_Tid,
"oski_ApplyHeurResults_Tid", &oski_ApplyHeurResults_Tid,
"oski_EstimateHeurCost_Tid", &oski_EstimateHeurCost_Tid,
"oski_EvaluateHeur_Tid", &oski_EvaluateHeur_Tid,
"oski_IsHeurApplicable_Tid", &oski_IsHeurApplicable_Tid,
"oski_SetHintMatMultAndMatTransMult_Tid", &oski_SetHintMatMultAndMatTransMult_Tid,
"oski_SetHintMatMult_Tid", &oski_SetHintMatMult_Tid,
"oski_SetHintMatPowMult_Tid", &oski_SetHintMatPowMult_Tid,
"oski_SetHintMatTransMatMult_Tid", &oski_SetHintMatTransMatMult_Tid,
"oski_SetHintMatTrisolve_Tid", &oski_SetHintMatTrisolve_Tid,
"oski_SetHint_Tid", &oski_SetHint_Tid,
"oski_CalcMat1Norm_Tid", &oski_CalcMat1Norm_Tid,
"oski_CopyMat_Tid", &oski_CopyMat_Tid,
"oski_CreateMatCSC_arr_Tid", &oski_CreateMatCSC_arr_Tid,
"oski_CreateMatCSC_Tid", &oski_CreateMatCSC_Tid,
"oski_CreateMatCSC_va_Tid", &oski_CreateMatCSC_va_Tid,
"oski_CreateMatCSR_arr_Tid", &oski_CreateMatCSR_arr_Tid,
"oski_CreateMatCSR_Tid", &oski_CreateMatCSR_Tid,
"oski_CreateMatCSR_va_Tid", &oski_CreateMatCSR_va_Tid,
"oski_DestroyMat_Tid", &oski_DestroyMat_Tid,
"oski_GetMatProps_Tid", &oski_GetMatProps_Tid,
"oski_CheckArgsMatMult_Tid", &oski_CheckArgsMatMult_Tid,
"oski_MakeArglistMatMult_Tid", &oski_MakeArglistMatMult_Tid,
"oski_MatMult_Tid", &oski_MatMult_Tid,
"oski_CheckArgsMatPowMult_Tid", &oski_CheckArgsMatPowMult_Tid,
"oski_MakeArglistMatPowMult_Tid", &oski_MakeArglistMatPowMult_Tid,
"oski_MatPowMult_Tid", &oski_MatPowMult_Tid,
"oski_CreateMatStruct_Tid", &oski_CreateMatStruct_Tid,
"oski_DestroyMatStruct_Tid", &oski_DestroyMatStruct_Tid,
"oski_IsMatPermuted_Tid", &oski_IsMatPermuted_Tid,
"oski_PermuteVecView_Tid", &oski_PermuteVecView_Tid,
"oski_ViewPermutedMatColPerm_Tid", &oski_ViewPermutedMatColPerm_Tid,
"oski_ViewPermutedMatRowPerm_Tid", &oski_ViewPermutedMatRowPerm_Tid,
"oski_ViewPermutedMat_Tid", &oski_ViewPermutedMat_Tid,
"PERM_IDENTITY_Tid", &PERM_IDENTITY_Tid,
"oski_CountTraceFlopsPerNz_Tid", &oski_CountTraceFlopsPerNz_Tid,
"oski_EstimateTraceTime_Tid", &oski_EstimateTraceTime_Tid,
"oski_EstimateTraceTimeTotal_Tid", &oski_EstimateTraceTimeTotal_Tid,
"oski_CheckArgsMatTrisolve_Tid", &oski_CheckArgsMatTrisolve_Tid,
"oski_MakeArglistMatTrisolve_Tid", &oski_MakeArglistMatTrisolve_Tid,
"oski_MatTrisolve_Tid", &oski_MatTrisolve_Tid,
"oski_TuneMat_Tid", &oski_TuneMat_Tid,
"oski_CalcVecViewInfNormDiff_Tid", &oski_CalcVecViewInfNormDiff_Tid,
"oski_CalcVecViewInfNorm_Tid", &oski_CalcVecViewInfNorm_Tid,
"oski_CopyVecView_Tid", &oski_CopyVecView_Tid,
"oski_CreateMultiVecView_Tid", &oski_CreateMultiVecView_Tid,
"oski_CreateVecView_Tid", &oski_CreateVecView_Tid,
"oski_DestroyVecView_Tid", &oski_DestroyVecView_Tid,
"oski_InitSubVecView_Tid", &oski_InitSubVecView_Tid,
"oski_PrintDebugVecView_Tid", &oski_PrintDebugVecView_Tid,
"SYMBOLIC_MULTIVEC_Tid", &SYMBOLIC_MULTIVEC_Tid,
"SYMBOLIC_VEC_Tid", &SYMBOLIC_VEC_Tid,
"oski_ApplyMatTransforms_Tid", &oski_ApplyMatTransforms_Tid,
"oski_CreateLuaMatReprGeneric2IndexFromCSR_Tid", &oski_CreateLuaMatReprGeneric2IndexFromCSR_Tid,
"oski_CreateLuaMatReprGenericFromCSR_Tid", &oski_CreateLuaMatReprGenericFromCSR_Tid,
"oski_FreeInputMatRepr_Tid", &oski_FreeInputMatRepr_Tid,
"oski_GetMatTransforms_Tid", &oski_GetMatTransforms_Tid,
"oski_ReplaceTunedMatRepr_Tid", &oski_ReplaceTunedMatRepr_Tid,
"dtrsv_", &dtrsv_,
"TVAL_IMAG_Tid", &TVAL_IMAG_Tid,
"TVAL_NEG_IMAG_Tid", &TVAL_NEG_IMAG_Tid,
"TVAL_NEG_ONE_Tid", &TVAL_NEG_ONE_Tid,
"TVAL_ONE_Tid", &TVAL_ONE_Tid,
"TVAL_ZERO_Tid", &TVAL_ZERO_Tid,
"daxpy_", &daxpy_,
"dscal_", &dscal_,
"oski_AXPYVecView_Tid", &oski_AXPYVecView_Tid,
"oski_ConjVecView_Tid", &oski_ConjVecView_Tid,
"oski_RectScaledIdentityMult_Tid", &oski_RectScaledIdentityMult_Tid,
"oski_ScaleVecView_Tid", &oski_ScaleVecView_Tid,
"oski_ZeroVecView_Tid", &oski_ZeroVecView_Tid,
"oski_GetStackLuaMat_Tid", &oski_GetStackLuaMat_Tid,
"oski_PushEmptyLuaMat_Tid", &oski_PushEmptyLuaMat_Tid,
"dgemv_", &dgemv_,
"oski_TransposePatternProp_Tid", &oski_TransposePatternProp_Tid,
"oski_TransposeProps_Tid", &oski_TransposeProps_Tid,
"oski_DestroyBCSRFillProfile_Tid", &oski_DestroyBCSRFillProfile_Tid,
"oski_EstimateFillBCSR_Tid", &oski_EstimateFillBCSR_Tid,
"liboski_mat_CSR_Tid.a", 0x0,
"liboski_mat_CSR_Tid_LTX_oski_CalcMatRepr1Norm", &liboski_mat_CSR_Tid_LTX_oski_CalcMatRepr1Norm,
"liboski_mat_CSR_Tid_LTX_oski_ChangeIndexBase", &liboski_mat_CSR_Tid_LTX_oski_ChangeIndexBase,
"liboski_mat_CSR_Tid_LTX_oski_CloseModule", &liboski_mat_CSR_Tid_LTX_oski_CloseModule,
"liboski_mat_CSR_Tid_LTX_oski_ConvertMatReprToCSR", &liboski_mat_CSR_Tid_LTX_oski_ConvertMatReprToCSR,
"liboski_mat_CSR_Tid_LTX_oski_CopyMatRepr", &liboski_mat_CSR_Tid_LTX_oski_CopyMatRepr,
"liboski_mat_CSR_Tid_LTX_oski_CountZeroRowsCSR", &liboski_mat_CSR_Tid_LTX_oski_CountZeroRowsCSR,
"liboski_mat_CSR_Tid_LTX_oski_CreateMatReprFromCSR", &liboski_mat_CSR_Tid_LTX_oski_CreateMatReprFromCSR,
"liboski_mat_CSR_Tid_LTX_oski_DestroyMatRepr", &liboski_mat_CSR_Tid_LTX_oski_DestroyMatRepr,
"liboski_mat_CSR_Tid_LTX_oski_GetLongDesc", &liboski_mat_CSR_Tid_LTX_oski_GetLongDesc,
"liboski_mat_CSR_Tid_LTX_oski_GetShortDesc", &liboski_mat_CSR_Tid_LTX_oski_GetShortDesc,
"liboski_mat_CSR_Tid_LTX_oski_InitModule", &liboski_mat_CSR_Tid_LTX_oski_InitModule,
"liboski_mat_CSR_Tid_LTX_oski_StreamMatRepr", &liboski_mat_CSR_Tid_LTX_oski_StreamMatRepr,
"liboski_mat_CSR_Tid_LTX_oski_WrapCSR", &liboski_mat_CSR_Tid_LTX_oski_WrapCSR,
"liboski_mat_CSR_Tid_LTX_oski_CheckCSR", &liboski_mat_CSR_Tid_LTX_oski_CheckCSR,
"liboski_mat_CSR_Tid_LTX_oski_ConditionallyExpandCSRToFull", &liboski_mat_CSR_Tid_LTX_oski_ConditionallyExpandCSRToFull,
"liboski_mat_CSR_Tid_LTX_oski_ExpandCSRToFull", &liboski_mat_CSR_Tid_LTX_oski_ExpandCSRToFull,
"liboski_mat_CSR_Tid_LTX_oski_SortIndices", &liboski_mat_CSR_Tid_LTX_oski_SortIndices,
"liboski_mat_CSR_Tid_LTX_oski_TransposeCSR", &liboski_mat_CSR_Tid_LTX_oski_TransposeCSR,
"liboski_mat_CSR_Tid_LTX_oski_GetMatReprEntry", &liboski_mat_CSR_Tid_LTX_oski_GetMatReprEntry,
"liboski_mat_CSR_Tid_LTX_oski_SetMatReprEntry", &liboski_mat_CSR_Tid_LTX_oski_SetMatReprEntry,
"liboski_mat_CSR_Tid_LTX_oski_MatReprMult", &liboski_mat_CSR_Tid_LTX_oski_MatReprMult,
"liboski_mat_CSR_Tid_LTX_oski_MatReprTrisolve", &liboski_mat_CSR_Tid_LTX_oski_MatReprTrisolve,
"CSR_MatTransMult_v1_a1_b1_xsX_ysX_Tid", &CSR_MatTransMult_v1_a1_b1_xsX_ysX_Tid,
"CSR_MatMult_v1_aX_b1_xs1_ysX_Tid", &CSR_MatMult_v1_aX_b1_xs1_ysX_Tid,
"CSR_MatTransMult_v1_aX_b1_xsX_ys1_Tid", &CSR_MatTransMult_v1_aX_b1_xsX_ys1_Tid,
"CSR_MatMult_v1_a1_b1_xs1_ysX_Tid", &CSR_MatMult_v1_a1_b1_xs1_ysX_Tid,
"CSR_MatTransMult_v1_a1_b1_xsX_ys1_Tid", &CSR_MatTransMult_v1_a1_b1_xsX_ys1_Tid,
"CSR_MatTransMult_v1_aN1_b1_xsX_ys1_Tid", &CSR_MatTransMult_v1_aN1_b1_xsX_ys1_Tid,
"CSR_MatTransMult_v1_aN1_b1_xsX_ysX_Tid", &CSR_MatTransMult_v1_aN1_b1_xsX_ysX_Tid,
"CSR_MatMult_v1_a1_b1_xsX_ysX_Tid", &CSR_MatMult_v1_a1_b1_xsX_ysX_Tid,
"CSR_MatMult_v1_aX_b1_xsX_ysX_Tid", &CSR_MatMult_v1_aX_b1_xsX_ysX_Tid,
"CSR_MatMult_v1_aN1_b1_xs1_ysX_Tid", &CSR_MatMult_v1_aN1_b1_xs1_ysX_Tid,
"CSR_MatTransMult_v1_aX_b1_xsX_ysX_Tid", &CSR_MatTransMult_v1_aX_b1_xsX_ysX_Tid,
"CSR_MatMult_v1_aN1_b1_xsX_ysX_Tid", &CSR_MatMult_v1_aN1_b1_xsX_ysX_Tid,
"CSR_SymmMatMult_v1_a1_b1_xs1_ysX_Tid", &CSR_SymmMatMult_v1_a1_b1_xs1_ysX_Tid,
"CSR_SymmMatMult_v1_aN1_b1_xs1_ys1_Tid", &CSR_SymmMatMult_v1_aN1_b1_xs1_ys1_Tid,
"CSR_SymmMatMult_v1_aN1_b1_xs1_ysX_Tid", &CSR_SymmMatMult_v1_aN1_b1_xs1_ysX_Tid,
"CSR_SymmMatMult_v1_aN1_b1_xsX_ysX_Tid", &CSR_SymmMatMult_v1_aN1_b1_xsX_ysX_Tid,
"CSR_SymmMatMult_v1_a1_b1_xs1_ys1_Tid", &CSR_SymmMatMult_v1_a1_b1_xs1_ys1_Tid,
"CSR_SymmMatMult_v1_aX_b1_xs1_ys1_Tid", &CSR_SymmMatMult_v1_aX_b1_xs1_ys1_Tid,
"CSR_SymmMatMult_v1_aX_b1_xsX_ysX_Tid", &CSR_SymmMatMult_v1_aX_b1_xsX_ysX_Tid,
"CSR_SymmMatMult_v1_aX_b1_xs1_ysX_Tid", &CSR_SymmMatMult_v1_aX_b1_xs1_ysX_Tid,
"CSR_SymmMatMult_v1_a1_b1_xsX_ys1_Tid", &CSR_SymmMatMult_v1_a1_b1_xsX_ys1_Tid,
"CSR_SymmMatMult_v1_aX_b1_xsX_ys1_Tid", &CSR_SymmMatMult_v1_aX_b1_xsX_ys1_Tid,
"CSR_SymmMatMult_v1_aN1_b1_xsX_ys1_Tid", &CSR_SymmMatMult_v1_aN1_b1_xsX_ys1_Tid,
"CSR_SymmMatMult_v1_a1_b1_xsX_ysX_Tid", &CSR_SymmMatMult_v1_a1_b1_xsX_ysX_Tid,
"CSR_MatTransTrisolveLower_Tid", &CSR_MatTransTrisolveLower_Tid,
"CSR_MatConjTrisolveUpper_Tid", &CSR_MatConjTrisolveUpper_Tid,
"CSR_MatConjTransTrisolveUpper_Tid", &CSR_MatConjTransTrisolveUpper_Tid,
"CSR_MatConjTrisolveLower_Tid", &CSR_MatConjTrisolveLower_Tid,
"CSR_MatTransTrisolveUpper_Tid", &CSR_MatTransTrisolveUpper_Tid,
"CSR_MatConjTransTrisolveLower_Tid", &CSR_MatConjTransTrisolveLower_Tid,
"CSR_MatTrisolveUpper_Tid", &CSR_MatTrisolveUpper_Tid,
"CSR_MatTrisolveLower_Tid", &CSR_MatTrisolveLower_Tid,
"liboski_mat_CSC_Tid.a", 0x0,
"liboski_mat_CSC_Tid_LTX_oski_CloseModule", &liboski_mat_CSC_Tid_LTX_oski_CloseModule,
"liboski_mat_CSC_Tid_LTX_oski_ConvertMatReprToCSR", &liboski_mat_CSC_Tid_LTX_oski_ConvertMatReprToCSR,
"liboski_mat_CSC_Tid_LTX_oski_CopyMatRepr", &liboski_mat_CSC_Tid_LTX_oski_CopyMatRepr,
"liboski_mat_CSC_Tid_LTX_oski_CreateLuaMatReprFromCSR", &liboski_mat_CSC_Tid_LTX_oski_CreateLuaMatReprFromCSR,
"liboski_mat_CSC_Tid_LTX_oski_CreateMatReprFromCSR", &liboski_mat_CSC_Tid_LTX_oski_CreateMatReprFromCSR,
"liboski_mat_CSC_Tid_LTX_oski_DestroyMatRepr", &liboski_mat_CSC_Tid_LTX_oski_DestroyMatRepr,
"liboski_mat_CSC_Tid_LTX_oski_GetLongDesc", &liboski_mat_CSC_Tid_LTX_oski_GetLongDesc,
"liboski_mat_CSC_Tid_LTX_oski_GetShortDesc", &liboski_mat_CSC_Tid_LTX_oski_GetShortDesc,
"liboski_mat_CSC_Tid_LTX_oski_InitModule", &liboski_mat_CSC_Tid_LTX_oski_InitModule,
"liboski_mat_CSC_Tid_LTX_oski_WrapCSC", &liboski_mat_CSC_Tid_LTX_oski_WrapCSC,
"liboski_mat_CSC_Tid_LTX_oski_MatReprMult", &liboski_mat_CSC_Tid_LTX_oski_MatReprMult,
"liboski_mat_CSC_Tid_LTX_oski_GetMatReprEntry", &liboski_mat_CSC_Tid_LTX_oski_GetMatReprEntry,
"liboski_mat_CSC_Tid_LTX_oski_SetMatReprEntry", &liboski_mat_CSC_Tid_LTX_oski_SetMatReprEntry,
"liboski_mat_CSC_Tid_LTX_oski_MatReprTrisolve", &liboski_mat_CSC_Tid_LTX_oski_MatReprTrisolve,
"liboski_Tld.a", 0x0,
"oski_CheckArgsMatMultAndMatTransMult_Tld", &oski_CheckArgsMatMultAndMatTransMult_Tld,
"oski_MakeArglistMatMultAndMatTransMult_Tld", &oski_MakeArglistMatMultAndMatTransMult_Tld,
"oski_MatMultAndMatTransMult_Tld", &oski_MatMultAndMatTransMult_Tld,
"oski_CheckArgsMatTransMatMult_Tld", &oski_CheckArgsMatTransMatMult_Tld,
"oski_MakeArglistMatTransMatMult_Tld", &oski_MakeArglistMatTransMatMult_Tld,
"oski_MatTransMatMult_Tld", &oski_MatTransMatMult_Tld,
"oski_GetMatClique_Tld", &oski_GetMatClique_Tld,
"oski_GetMatDiagValues_Tld", &oski_GetMatDiagValues_Tld,
"oski_GetMatEntry_Tld", &oski_GetMatEntry_Tld,
"oski_SetMatClique_Tld", &oski_SetMatClique_Tld,
"oski_SetMatDiagValues_Tld", &oski_SetMatDiagValues_Tld,
"oski_SetMatEntry_Tld", &oski_SetMatEntry_Tld,
"oski_ApplyHeurResults_Tld", &oski_ApplyHeurResults_Tld,
"oski_EstimateHeurCost_Tld", &oski_EstimateHeurCost_Tld,
"oski_EvaluateHeur_Tld", &oski_EvaluateHeur_Tld,
"oski_IsHeurApplicable_Tld", &oski_IsHeurApplicable_Tld,
"oski_SetHintMatMultAndMatTransMult_Tld", &oski_SetHintMatMultAndMatTransMult_Tld,
"oski_SetHintMatMult_Tld", &oski_SetHintMatMult_Tld,
"oski_SetHintMatPowMult_Tld", &oski_SetHintMatPowMult_Tld,
"oski_SetHintMatTransMatMult_Tld", &oski_SetHintMatTransMatMult_Tld,
"oski_SetHintMatTrisolve_Tld", &oski_SetHintMatTrisolve_Tld,
"oski_SetHint_Tld", &oski_SetHint_Tld,
"oski_CalcMat1Norm_Tld", &oski_CalcMat1Norm_Tld,
"oski_CopyMat_Tld", &oski_CopyMat_Tld,
"oski_CreateMatCSC_arr_Tld", &oski_CreateMatCSC_arr_Tld,
"oski_CreateMatCSC_Tld", &oski_CreateMatCSC_Tld,
"oski_CreateMatCSC_va_Tld", &oski_CreateMatCSC_va_Tld,
"oski_CreateMatCSR_arr_Tld", &oski_CreateMatCSR_arr_Tld,
"oski_CreateMatCSR_Tld", &oski_CreateMatCSR_Tld,
"oski_CreateMatCSR_va_Tld", &oski_CreateMatCSR_va_Tld,
"oski_DestroyMat_Tld", &oski_DestroyMat_Tld,
"oski_GetMatProps_Tld", &oski_GetMatProps_Tld,
"oski_CheckArgsMatMult_Tld", &oski_CheckArgsMatMult_Tld,
"oski_MakeArglistMatMult_Tld", &oski_MakeArglistMatMult_Tld,
"oski_MatMult_Tld", &oski_MatMult_Tld,
"oski_CheckArgsMatPowMult_Tld", &oski_CheckArgsMatPowMult_Tld,
"oski_MakeArglistMatPowMult_Tld", &oski_MakeArglistMatPowMult_Tld,
"oski_MatPowMult_Tld", &oski_MatPowMult_Tld,
"oski_CreateMatStruct_Tld", &oski_CreateMatStruct_Tld,
"oski_DestroyMatStruct_Tld", &oski_DestroyMatStruct_Tld,
"oski_IsMatPermuted_Tld", &oski_IsMatPermuted_Tld,
"oski_PermuteVecView_Tld", &oski_PermuteVecView_Tld,
"oski_ViewPermutedMatColPerm_Tld", &oski_ViewPermutedMatColPerm_Tld,
"oski_ViewPermutedMatRowPerm_Tld", &oski_ViewPermutedMatRowPerm_Tld,
"oski_ViewPermutedMat_Tld", &oski_ViewPermutedMat_Tld,
"PERM_IDENTITY_Tld", &PERM_IDENTITY_Tld,
"oski_CountTraceFlopsPerNz_Tld", &oski_CountTraceFlopsPerNz_Tld,
"oski_EstimateTraceTime_Tld", &oski_EstimateTraceTime_Tld,
"oski_EstimateTraceTimeTotal_Tld", &oski_EstimateTraceTimeTotal_Tld,
"oski_CheckArgsMatTrisolve_Tld", &oski_CheckArgsMatTrisolve_Tld,
"oski_MakeArglistMatTrisolve_Tld", &oski_MakeArglistMatTrisolve_Tld,
"oski_MatTrisolve_Tld", &oski_MatTrisolve_Tld,
"oski_TuneMat_Tld", &oski_TuneMat_Tld,
"oski_CalcVecViewInfNormDiff_Tld", &oski_CalcVecViewInfNormDiff_Tld,
"oski_CalcVecViewInfNorm_Tld", &oski_CalcVecViewInfNorm_Tld,
"oski_CopyVecView_Tld", &oski_CopyVecView_Tld,
"oski_CreateMultiVecView_Tld", &oski_CreateMultiVecView_Tld,
"oski_CreateVecView_Tld", &oski_CreateVecView_Tld,
"oski_DestroyVecView_Tld", &oski_DestroyVecView_Tld,
"oski_InitSubVecView_Tld", &oski_InitSubVecView_Tld,
"oski_PrintDebugVecView_Tld", &oski_PrintDebugVecView_Tld,
"SYMBOLIC_MULTIVEC_Tld", &SYMBOLIC_MULTIVEC_Tld,
"SYMBOLIC_VEC_Tld", &SYMBOLIC_VEC_Tld,
"oski_ApplyMatTransforms_Tld", &oski_ApplyMatTransforms_Tld,
"oski_CreateLuaMatReprGeneric2IndexFromCSR_Tld", &oski_CreateLuaMatReprGeneric2IndexFromCSR_Tld,
"oski_CreateLuaMatReprGenericFromCSR_Tld", &oski_CreateLuaMatReprGenericFromCSR_Tld,
"oski_FreeInputMatRepr_Tld", &oski_FreeInputMatRepr_Tld,
"oski_GetMatTransforms_Tld", &oski_GetMatTransforms_Tld,
"oski_ReplaceTunedMatRepr_Tld", &oski_ReplaceTunedMatRepr_Tld,
"ldgemv_", &ldgemv_,
"TVAL_IMAG_Tld", &TVAL_IMAG_Tld,
"TVAL_NEG_IMAG_Tld", &TVAL_NEG_IMAG_Tld,
"TVAL_NEG_ONE_Tld", &TVAL_NEG_ONE_Tld,
"TVAL_ONE_Tld", &TVAL_ONE_Tld,
"TVAL_ZERO_Tld", &TVAL_ZERO_Tld,
"ldaxpy_", &ldaxpy_,
"ldscal_", &ldscal_,
"oski_AXPYVecView_Tld", &oski_AXPYVecView_Tld,
"oski_ConjVecView_Tld", &oski_ConjVecView_Tld,
"oski_RectScaledIdentityMult_Tld", &oski_RectScaledIdentityMult_Tld,
"oski_ScaleVecView_Tld", &oski_ScaleVecView_Tld,
"oski_ZeroVecView_Tld", &oski_ZeroVecView_Tld,
"ldtrsv_", &ldtrsv_,
"oski_TransposePatternProp_Tld", &oski_TransposePatternProp_Tld,
"oski_TransposeProps_Tld", &oski_TransposeProps_Tld,
"oski_GetStackLuaMat_Tld", &oski_GetStackLuaMat_Tld,
"oski_PushEmptyLuaMat_Tld", &oski_PushEmptyLuaMat_Tld,
"oski_DestroyBCSRFillProfile_Tld", &oski_DestroyBCSRFillProfile_Tld,
"oski_EstimateFillBCSR_Tld", &oski_EstimateFillBCSR_Tld,
"liboski_mat_CSR_Tld.a", 0x0,
"liboski_mat_CSR_Tld_LTX_oski_CalcMatRepr1Norm", &liboski_mat_CSR_Tld_LTX_oski_CalcMatRepr1Norm,
"liboski_mat_CSR_Tld_LTX_oski_ChangeIndexBase", &liboski_mat_CSR_Tld_LTX_oski_ChangeIndexBase,
"liboski_mat_CSR_Tld_LTX_oski_CloseModule", &liboski_mat_CSR_Tld_LTX_oski_CloseModule,
"liboski_mat_CSR_Tld_LTX_oski_ConvertMatReprToCSR", &liboski_mat_CSR_Tld_LTX_oski_ConvertMatReprToCSR,
"liboski_mat_CSR_Tld_LTX_oski_CopyMatRepr", &liboski_mat_CSR_Tld_LTX_oski_CopyMatRepr,
"liboski_mat_CSR_Tld_LTX_oski_CountZeroRowsCSR", &liboski_mat_CSR_Tld_LTX_oski_CountZeroRowsCSR,
"liboski_mat_CSR_Tld_LTX_oski_CreateMatReprFromCSR", &liboski_mat_CSR_Tld_LTX_oski_CreateMatReprFromCSR,
"liboski_mat_CSR_Tld_LTX_oski_DestroyMatRepr", &liboski_mat_CSR_Tld_LTX_oski_DestroyMatRepr,
"liboski_mat_CSR_Tld_LTX_oski_GetLongDesc", &liboski_mat_CSR_Tld_LTX_oski_GetLongDesc,
"liboski_mat_CSR_Tld_LTX_oski_GetShortDesc", &liboski_mat_CSR_Tld_LTX_oski_GetShortDesc,
"liboski_mat_CSR_Tld_LTX_oski_InitModule", &liboski_mat_CSR_Tld_LTX_oski_InitModule,
"liboski_mat_CSR_Tld_LTX_oski_StreamMatRepr", &liboski_mat_CSR_Tld_LTX_oski_StreamMatRepr,
"liboski_mat_CSR_Tld_LTX_oski_WrapCSR", &liboski_mat_CSR_Tld_LTX_oski_WrapCSR,
"liboski_mat_CSR_Tld_LTX_oski_CheckCSR", &liboski_mat_CSR_Tld_LTX_oski_CheckCSR,
"liboski_mat_CSR_Tld_LTX_oski_ConditionallyExpandCSRToFull", &liboski_mat_CSR_Tld_LTX_oski_ConditionallyExpandCSRToFull,
"liboski_mat_CSR_Tld_LTX_oski_ExpandCSRToFull", &liboski_mat_CSR_Tld_LTX_oski_ExpandCSRToFull,
"liboski_mat_CSR_Tld_LTX_oski_SortIndices", &liboski_mat_CSR_Tld_LTX_oski_SortIndices,
"liboski_mat_CSR_Tld_LTX_oski_TransposeCSR", &liboski_mat_CSR_Tld_LTX_oski_TransposeCSR,
"liboski_mat_CSR_Tld_LTX_oski_GetMatReprEntry", &liboski_mat_CSR_Tld_LTX_oski_GetMatReprEntry,
"liboski_mat_CSR_Tld_LTX_oski_SetMatReprEntry", &liboski_mat_CSR_Tld_LTX_oski_SetMatReprEntry,
"liboski_mat_CSR_Tld_LTX_oski_MatReprMult", &liboski_mat_CSR_Tld_LTX_oski_MatReprMult,
"liboski_mat_CSR_Tld_LTX_oski_MatReprTrisolve", &liboski_mat_CSR_Tld_LTX_oski_MatReprTrisolve,
"CSR_MatTransMult_v1_aN1_b1_xsX_ysX_Tld", &CSR_MatTransMult_v1_aN1_b1_xsX_ysX_Tld,
"CSR_MatMult_v1_a1_b1_xsX_ysX_Tld", &CSR_MatMult_v1_a1_b1_xsX_ysX_Tld,
"CSR_MatTransMult_v1_a1_b1_xsX_ysX_Tld", &CSR_MatTransMult_v1_a1_b1_xsX_ysX_Tld,
"CSR_MatMult_v1_aX_b1_xsX_ysX_Tld", &CSR_MatMult_v1_aX_b1_xsX_ysX_Tld,
"CSR_MatTransMult_v1_aX_b1_xsX_ys1_Tld", &CSR_MatTransMult_v1_aX_b1_xsX_ys1_Tld,
"CSR_MatMult_v1_a1_b1_xs1_ysX_Tld", &CSR_MatMult_v1_a1_b1_xs1_ysX_Tld,
"CSR_MatMult_v1_aX_b1_xs1_ysX_Tld", &CSR_MatMult_v1_aX_b1_xs1_ysX_Tld,
"CSR_MatTransMult_v1_a1_b1_xsX_ys1_Tld", &CSR_MatTransMult_v1_a1_b1_xsX_ys1_Tld,
"CSR_MatTransMult_v1_aX_b1_xsX_ysX_Tld", &CSR_MatTransMult_v1_aX_b1_xsX_ysX_Tld,
"CSR_MatMult_v1_aN1_b1_xs1_ysX_Tld", &CSR_MatMult_v1_aN1_b1_xs1_ysX_Tld,
"CSR_MatTransMult_v1_aN1_b1_xsX_ys1_Tld", &CSR_MatTransMult_v1_aN1_b1_xsX_ys1_Tld,
"CSR_MatMult_v1_aN1_b1_xsX_ysX_Tld", &CSR_MatMult_v1_aN1_b1_xsX_ysX_Tld,
"CSR_SymmMatMult_v1_a1_b1_xs1_ysX_Tld", &CSR_SymmMatMult_v1_a1_b1_xs1_ysX_Tld,
"CSR_SymmMatMult_v1_aX_b1_xs1_ys1_Tld", &CSR_SymmMatMult_v1_aX_b1_xs1_ys1_Tld,
"CSR_SymmMatMult_v1_aN1_b1_xsX_ysX_Tld", &CSR_SymmMatMult_v1_aN1_b1_xsX_ysX_Tld,
"CSR_SymmMatMult_v1_aN1_b1_xsX_ys1_Tld", &CSR_SymmMatMult_v1_aN1_b1_xsX_ys1_Tld,
"CSR_SymmMatMult_v1_aX_b1_xsX_ys1_Tld", &CSR_SymmMatMult_v1_aX_b1_xsX_ys1_Tld,
"CSR_SymmMatMult_v1_a1_b1_xsX_ys1_Tld", &CSR_SymmMatMult_v1_a1_b1_xsX_ys1_Tld,
"CSR_SymmMatMult_v1_aN1_b1_xs1_ys1_Tld", &CSR_SymmMatMult_v1_aN1_b1_xs1_ys1_Tld,
"CSR_SymmMatMult_v1_aN1_b1_xs1_ysX_Tld", &CSR_SymmMatMult_v1_aN1_b1_xs1_ysX_Tld,
"CSR_SymmMatMult_v1_aX_b1_xs1_ysX_Tld", &CSR_SymmMatMult_v1_aX_b1_xs1_ysX_Tld,
"CSR_SymmMatMult_v1_a1_b1_xsX_ysX_Tld", &CSR_SymmMatMult_v1_a1_b1_xsX_ysX_Tld,
"CSR_SymmMatMult_v1_aX_b1_xsX_ysX_Tld", &CSR_SymmMatMult_v1_aX_b1_xsX_ysX_Tld,
"CSR_SymmMatMult_v1_a1_b1_xs1_ys1_Tld", &CSR_SymmMatMult_v1_a1_b1_xs1_ys1_Tld,
"CSR_MatTrisolveLower_Tld", &CSR_MatTrisolveLower_Tld,
"CSR_MatTransTrisolveUpper_Tld", &CSR_MatTransTrisolveUpper_Tld,
"CSR_MatConjTrisolveLower_Tld", &CSR_MatConjTrisolveLower_Tld,
"CSR_MatTrisolveUpper_Tld", &CSR_MatTrisolveUpper_Tld,
"CSR_MatConjTransTrisolveUpper_Tld", &CSR_MatConjTransTrisolveUpper_Tld,
"CSR_MatConjTrisolveUpper_Tld", &CSR_MatConjTrisolveUpper_Tld,
"CSR_MatTransTrisolveLower_Tld", &CSR_MatTransTrisolveLower_Tld,
"CSR_MatConjTransTrisolveLower_Tld", &CSR_MatConjTransTrisolveLower_Tld,
"liboski_mat_CSC_Tld.a", 0x0,
"liboski_mat_CSC_Tld_LTX_oski_CloseModule", &liboski_mat_CSC_Tld_LTX_oski_CloseModule,
"liboski_mat_CSC_Tld_LTX_oski_ConvertMatReprToCSR", &liboski_mat_CSC_Tld_LTX_oski_ConvertMatReprToCSR,
"liboski_mat_CSC_Tld_LTX_oski_CopyMatRepr", &liboski_mat_CSC_Tld_LTX_oski_CopyMatRepr,
"liboski_mat_CSC_Tld_LTX_oski_CreateLuaMatReprFromCSR", &liboski_mat_CSC_Tld_LTX_oski_CreateLuaMatReprFromCSR,
"liboski_mat_CSC_Tld_LTX_oski_CreateMatReprFromCSR", &liboski_mat_CSC_Tld_LTX_oski_CreateMatReprFromCSR,
"liboski_mat_CSC_Tld_LTX_oski_DestroyMatRepr", &liboski_mat_CSC_Tld_LTX_oski_DestroyMatRepr,
"liboski_mat_CSC_Tld_LTX_oski_GetLongDesc", &liboski_mat_CSC_Tld_LTX_oski_GetLongDesc,
"liboski_mat_CSC_Tld_LTX_oski_GetShortDesc", &liboski_mat_CSC_Tld_LTX_oski_GetShortDesc,
"liboski_mat_CSC_Tld_LTX_oski_InitModule", &liboski_mat_CSC_Tld_LTX_oski_InitModule,
"liboski_mat_CSC_Tld_LTX_oski_WrapCSC", &liboski_mat_CSC_Tld_LTX_oski_WrapCSC,
"liboski_mat_CSC_Tld_LTX_oski_MatReprMult", &liboski_mat_CSC_Tld_LTX_oski_MatReprMult,
"liboski_mat_CSC_Tld_LTX_oski_GetMatReprEntry", &liboski_mat_CSC_Tld_LTX_oski_GetMatReprEntry,
"liboski_mat_CSC_Tld_LTX_oski_SetMatReprEntry", &liboski_mat_CSC_Tld_LTX_oski_SetMatReprEntry,
"liboski_mat_CSC_Tld_LTX_oski_MatReprTrisolve", &liboski_mat_CSC_Tld_LTX_oski_MatReprTrisolve,
0x0, 0x0};
/* eof */
