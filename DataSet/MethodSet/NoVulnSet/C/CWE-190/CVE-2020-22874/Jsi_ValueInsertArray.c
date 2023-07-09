Jsi_RC Jsi_ValueInsertArray(Jsi_Interp *interp, Jsi_Value *target, int key, Jsi_Value *val, int flags)
{
    if (target->vt != JSI_VT_OBJECT) {
        if (interp->strict)
            Jsi_LogWarn("Target is not object");
        return JSI_ERROR;
    }
    Jsi_Obj *obj = target->d.obj;
    
    if (obj->isarrlist) {
        if (key >= 0 && (uint)key < interp->maxArrayList) {
            Jsi_ObjArraySet(interp, obj, val, key);
            return JSI_OK;
        }
        return JSI_ERROR;
    }
    char unibuf[JSI_MAX_NUMBER_STRING];
    Jsi_NumberItoA10(key, unibuf, sizeof(unibuf));
    Jsi_ObjInsert(interp, obj, unibuf, val, flags);
    return JSI_OK;
}