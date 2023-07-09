Jsi_Value *jsi_ValueObjKeyAssign(Jsi_Interp *interp, Jsi_Value *target, Jsi_Value *keyval, Jsi_Value *value, int flag)
{
    int arrayindex = -1;

    if (keyval->vt == JSI_VT_NUMBER && Jsi_NumberIsInteger(keyval->d.num) && keyval->d.num >= 0) {
        arrayindex = (int)keyval->d.num;
    }
    /* TODO: array["1"] also extern the length of array */
    
    if (arrayindex >= 0 && (uint)arrayindex < interp->maxArrayList &&
        target->vt == JSI_VT_OBJECT && target->d.obj->arr) {
        return jsi_ObjArraySetDup(interp, target->d.obj, value, arrayindex);
    }
    const char *kstr = Jsi_ValueToString(interp, keyval, NULL);
    
#if (defined(JSI_HAS___PROTO__) && JSI_HAS___PROTO__==2)
    if (Jsi_Strcmp(kstr, "__proto__")==0) {
        Jsi_Obj *obj = target->d.obj;
        obj->__proto__ = Jsi_ValueDup(interp, value);
        //obj->clearProto = 1;
        return obj->__proto__;
    }
#endif
    Jsi_Value *v = Jsi_ValueNew1(interp);
    if (value)
        Jsi_ValueCopy(interp, v, value);

    jsi_ValueObjSet(interp, target, kstr, v, flag, (Jsi_ValueIsStringKey(interp, keyval)? JSI_OM_ISSTRKEY:0));
    Jsi_DecrRefCount(interp, v);
    return v;
}