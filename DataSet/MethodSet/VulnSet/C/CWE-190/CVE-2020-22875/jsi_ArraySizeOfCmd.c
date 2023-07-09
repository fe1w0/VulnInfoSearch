static Jsi_RC jsi_ArraySizeOfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,Jsi_Value **ret, Jsi_Func *funcPtr) {
    if (_this->vt != JSI_VT_OBJECT || !Jsi_ObjIsArray(interp, _this->d.obj))
        return Jsi_LogError("expected array object");
    int i = Jsi_ObjGetLength(interp, _this->d.obj);
    Jsi_ValueMakeNumber(interp, ret, i);
    return JSI_OK;
}