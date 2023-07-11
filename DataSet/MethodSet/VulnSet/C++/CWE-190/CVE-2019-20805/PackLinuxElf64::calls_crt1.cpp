bool PackLinuxElf64::calls_crt1(Elf64_Rela const *rela, int sz)
{
    if (!dynsym || !dynstr) {
        return false;
    }
    for (unsigned relnum= 0; 0 < sz; (sz -= sizeof(Elf64_Rela)), ++rela, ++relnum) {
        unsigned const symnum = get_te64(&rela->r_info) >> 32;
        char const *const symnam = get_dynsym_name(symnum, relnum);
        if (0==strcmp(symnam, "__libc_start_main")  // glibc
        ||  0==strcmp(symnam, "__libc_init")  // Android
        ||  0==strcmp(symnam, "__uClibc_main")
        ||  0==strcmp(symnam, "__uClibc_start_main"))
            return true;
    }
    return false;
}