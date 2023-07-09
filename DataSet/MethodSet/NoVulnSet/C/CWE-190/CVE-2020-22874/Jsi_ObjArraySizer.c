int Jsi_ObjArraySizer(Jsi_Interp *interp, Jsi_Obj *obj, uint len)
{
    uint nsiz = len + 1, mod = ALLOC_MOD_SIZE;
    assert(obj->isarrlist);
    if (mod>1)
        nsiz = nsiz + ((mod-1) - (nsiz + mod - 1)%mod);
    if (len >= interp->maxArrayList || nsiz > interp->maxArrayList) {
        Jsi_LogError("array size too big: %u >= %u", len, interp->maxArrayList);
        return 0;
    }
    if (len >= obj->arrMaxSize) {
        int oldsz = (nsiz-obj->arrMaxSize);
        obj->arr = (Jsi_Value**)Jsi_Realloc(obj->arr, nsiz*sizeof(Jsi_Value*));
        memset(obj->arr+obj->arrMaxSize, 0, oldsz*sizeof(Jsi_Value*));
        obj->arrMaxSize = nsiz;
    }
    if (len>obj->arrCnt)
        obj->arrCnt = len;
    return nsiz;
}