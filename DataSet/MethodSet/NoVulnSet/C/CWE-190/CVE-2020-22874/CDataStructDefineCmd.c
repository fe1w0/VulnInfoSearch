static Jsi_RC CDataStructDefineCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    Jsi_RC rc = JSI_OK;
    Jsi_OptionTypedef *st = NULL;
    jsi_csInitType(interp);
    Jsi_Value *eval = NULL, *val = Jsi_ValueArrayIndex(interp, args, 0),
        *flds = Jsi_ValueArrayIndex(interp, args, 1);
    int vlen, flen, i, argc, clen = 0, arrCnt=0;
    Jsi_DString fStr = {};
    char **argv = NULL, *cmt, *eq, *cp, *ce, *cpval, ocp;
    const char *vstr = Jsi_ValueString(interp, val, &vlen),
        *fstr = Jsi_ValueString(interp, flds, &flen);
    if (vstr) {
        cmt = Jsi_Strstr(fstr, "//");
        eq = Jsi_Strchr(fstr, '=');
        Jsi_DString tStr = {fstr};
        fstr=jsi_TrimStr(Jsi_DSValue(&tStr));
        Jsi_SplitStr(fstr, &argc, &argv, (cmt?"\n":";"), &fStr);
        Jsi_DSFree(&tStr);
        if (!cmt && argc>0 && !argv[argc-1][0])
            argc--;
        if (eq) {
            eval = Jsi_ValueNewArray(interp, NULL, 0);
            Jsi_IncrRefCount(interp, eval);
        }
    } else if (!Jsi_ValueIsArray(interp,flds) || (argc=Jsi_ValueGetLength(interp, flds))<1)
        return Jsi_LogError("arg 2 must be string or non-empty array");
    if (argc>200)
        return Jsi_LogError("too many fields: %d>200", argc);
    Jsi_StructSpec *sl, *sf, recs[argc+2];
    memset(recs, 0, sizeof(recs));
    sl = recs+argc+1;
    sf = recs;
    SIGINIT(sl, OPTS_STRUCT);
    if (vstr) {
        sl->name = Jsi_KeyAdd(interp, vstr);
    } else if (Jsi_OptionsProcess(interp, StructOptions, sl, val, 0) < 0) {
        rc = JSI_ERROR;
        goto bail;
    }
    if (!Jsi_StrIsAlnum(sl->name)) {
        rc = Jsi_LogError("invalid struct name: %s", sl->name);
        goto bail;
    }
    if (jsi_csStructGet(interp, sl->name)) {
        rc = Jsi_LogError("struct already exists: %s", sl->name);
        goto bail;
    }
    for (i = 0; i<argc; i++) {
        Jsi_Number numVal = 0;
        bool bVal, isbitset = 0;
        Jsi_Value *inival = NULL;
        sf = recs+i;
        SIGINIT(sf, OPTS_FIELD);
        if (!argv) {
            val = Jsi_ValueArrayIndex(interp, flds, i);
            if (!val || Jsi_OptionsProcess(interp, StructFieldOptions, sf, val, 0) < 0) {
                rc = JSI_ERROR;
                goto bail;
            }
            if (!Jsi_StrIsAlnum(sf->name) || !sf->type) {
                rc = Jsi_LogError("invalid struct field name: %s", sf->name);
                goto bail;
            }
            if (!Jsi_ValueObjLookup(interp, val, "init", 0) && (sf->type->flags&jsi_CTYP_STRUCT))
                inival = Jsi_ValueNewBoolean(interp, true);
        } else {
            if (cmt) {
                cp = Jsi_Strstr(argv[i], "//"); 
                if (cp) {
                    *cp = 0;
                    cp += 2;
                    cp = jsi_TrimStr(cp);
                    sf->help = Jsi_KeyAdd(interp, cp);
                }
                cp = Jsi_Strchr(argv[i], ';'); 
                if (cp) *cp = 0;
            }
            cp = Jsi_Strchr(argv[i], '='); 
            if (cp) {
                if (!eval)
                    goto bail;
                *cp++ = 0;
                cp = jsi_TrimStr(cp);
                int cplen = Jsi_Strlen(cp);
                if (cplen>1 && (cp[0] == '\'' || cp[0] == '"') && cp[0] == cp[cplen-1]) {
                    cpval = cp+1;
                    cp[cplen-1] = 0;
                    inival = Jsi_ValueNewStringDup(interp, cpval);
                }
                else if (Jsi_GetDouble(interp, cp, &numVal) == JSI_OK)
                    inival = Jsi_ValueNewNumber(interp, numVal);
                else if (Jsi_GetBool(interp, cp, &bVal) == JSI_OK)
                    inival = Jsi_ValueNewBoolean(interp, bVal);
                else
                    inival = Jsi_ValueNewStringDup(interp, cp);
            }
            cp = argv[i];
            while (*cp && isspace(*cp)) cp++;
            ce = cp;
            while (*ce && !isspace(*ce)) ce++;
            ocp = *ce;
            if (!ocp) {
                rc = Jsi_LogError("bad field: %s", cp);
                goto bail;
            }
            *ce = 0;
            sf->type = Jsi_TypeLookup(interp, cp);
            *ce = ocp;
            if (!sf->type) {
                rc = Jsi_LogError("unknown type: %s", argv[i]);
                goto bail;
            }
            if (!inival && (sf->type->flags&jsi_CTYP_STRUCT))
                inival = Jsi_ValueNewBoolean(interp, true);

            cp = ce+1;
            while (*cp && isspace(*cp)) cp++;
            ce = cp;
            while (*ce && (isalnum(*ce) || *ce == '_')) ce++;
            ocp = *ce;
            *ce = 0;
            if (!*cp) {
                rc = Jsi_LogError("missing or bad struct field name");
                goto bail;
            }
            sf->name = Jsi_KeyAdd(interp, cp);
            if (ocp) {
                ce++;
                clen = Jsi_Strlen(ce);
            }
            if (ocp == '@') {
                isbitset = 1;
            } else
            if (ocp == ':') {
                int bsiz = -1;
                if (Jsi_GetInt(interp, ce, &bsiz,10) != JSI_OK || bsiz<=0 || bsiz>64) {
                    rc = Jsi_LogError("bad bit size: %s", ce);
                    goto bail;
                }
                sf->bits = bsiz;
            } else if (ocp == '[' && clen>1 && ce[clen-1]==']') {
                int asiz = -1;
                ce[clen-1] = 0;
                if (Jsi_GetInt(interp, ce, &asiz, 10) != JSI_OK || asiz<=0) {
                    rc = Jsi_LogError("bad size: %s", cp);
                    goto bail;
                }
                sf->arrSize = asiz;
            } else if (ocp) {
                rc = Jsi_LogError("bad field: %s", cp);
                goto bail;
            }
            val = NULL;            
        }
        if (inival) {
            Jsi_Value *sval = Jsi_ValueNewObj(interp, NULL);
            Jsi_ValueInsert(interp, sval, "init", inival, 0);
            Jsi_ValueArraySet(interp, eval, sval, i);
        }
        bool isEnum = (sf->type && (sf->type->flags&jsi_CTYP_ENUM));
        if (isbitset) {
            if (!isEnum) {
                rc = Jsi_LogError("bitset type is not an enum");
                goto bail;
            }
            sf->flags |= JSI_OPT_BITSET_ENUM;
        }
        if (sf->arrSize) {
            if (sf->arrSize>interp->maxArrayList) {
                rc = Jsi_LogError("array size too big: %d >= %d", sf->arrSize, interp->maxArrayList);
                goto bail;
            }
            if (sf->bits || isEnum) {
                rc = Jsi_LogError("array of %s unsupported", (sf->bits?"bits":"enums"));
                goto bail;
            }
            arrCnt++;
        }
    }
    recs[argc].id = JSI_OPTION_END;
    rc = jsi_csSetupStruct(interp, sl, recs, recs, argc, &st, arrCnt);
    if (rc != JSI_OK || !st) {
        rc = JSI_ERROR;
        goto bail;
    }
    sl = jsi_csGetStructSpec(st->extData);
    sf = jsi_csGetFieldSpec(sl->extData);
    if (vstr)
        flds = eval;
    for (i = 0; i<argc && flds; i++) {
        val = Jsi_ValueArrayIndex(interp, flds, i);
        if (val && Jsi_OptionsProcess(interp, StructFieldOptions, sf+i, val, JSI_OPT_PASS2) < 0) {
            rc = JSI_ERROR;
            goto bail;
        }
    }
bail:
    if (eval)
        Jsi_DecrRefCount(interp, eval);
    Jsi_DSFree(&fStr);
    if (rc != JSI_OK && st)
        Jsi_HashEntryDelete(st->hPtr);
    return rc;
}