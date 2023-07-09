static Jsi_RC ObjListifyCallback(Jsi_Tree *tree, Jsi_TreeEntry *hPtr, void *data)
{
    Jsi_Interp *interp = tree->opts.interp;
    Jsi_Obj *obj = (Jsi_Obj*)data;
    int n;
    if (!hPtr->f.bits.dontenum) {
        char *ep = NULL, *cp = (char*)Jsi_TreeKeyGet(hPtr);
        if (!cp || !isdigit(*cp))
            return JSI_OK;
        n = (int)strtol(cp, &ep, 0);
        if (n<0 || (uint)n >= interp->maxArrayList)
            return JSI_OK;
        hPtr->f.bits.isarrlist = 1;
        if (Jsi_ObjArraySizer(interp, obj, n) <= 0) 
            return Jsi_LogError("too long");
        obj->arr[n] = (Jsi_Value*)Jsi_TreeValueGet(hPtr);
       // obj->arrCnt++;
    }
    return JSI_OK;
}