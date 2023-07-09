static njs_jump_off_t
njs_vmcode_array(njs_vm_t *vm, u_char *pc)
{
    uint32_t            length;
    njs_array_t         *array;
    njs_value_t         *value;
    njs_vmcode_array_t  *code;

    code = (njs_vmcode_array_t *) pc;

    array = njs_array_alloc(vm, 0, code->length, NJS_ARRAY_SPARE);

    if (njs_fast_path(array != NULL)) {

        if (code->ctor) {
            /* Array of the form [,,,], [1,,]. */
            value = array->start;
            length = array->length;

            do {
                njs_set_invalid(value);
                value++;
                length--;
            } while (length != 0);

        } else {
            /* Array of the form [], [,,1], [1,2,3]. */
            array->length = 0;
        }

        njs_set_array(&vm->retval, array);

        return sizeof(njs_vmcode_array_t);
    }

    return NJS_ERROR;
}