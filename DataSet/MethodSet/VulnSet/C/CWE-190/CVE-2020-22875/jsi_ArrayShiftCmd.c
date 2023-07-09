static Jsi_RC jsi_ArrayShiftCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,Jsi_Value **ret, Jsi_Func *funcPtr) {
    if (_this->vt != JSI_VT_OBJECT || !Jsi_ObjIsArray(interp, _this->d.obj))
        return Jsi_LogError("expected array object");
    Jsi_Value *v;
    Jsi_Obj *obj = _this->d.obj;
    Jsi_ObjListifyArray(interp, obj);
    uint n = Jsi_ObjGetLength(interp, obj);
    assert(n <= obj->arrCnt);
    if (n<=0) {
        Jsi_ValueMakeUndef(interp, ret);
    } else {
        n--;
        v = obj->arr[0];
        memmove(obj->arr, obj->arr+1, n*sizeof(Jsi_Value*));
        obj->arr[n] = NULL;
        Jsi_ValueDup2(interp, ret, v);
        Jsi_DecrRefCount(interp, v);
        Jsi_ObjSetLength(interp, obj, n);
    }
    return JSI_OK;
}