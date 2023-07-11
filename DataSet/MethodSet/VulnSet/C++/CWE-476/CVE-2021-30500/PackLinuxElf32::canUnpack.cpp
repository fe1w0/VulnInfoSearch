int PackLinuxElf32::canUnpack()
{
    if (super::canUnpack()) {
        return true;
    }
    if (Elf32_Ehdr::ET_DYN==get_te16(&ehdri.e_type)) {
        PackLinuxElf32help1(fi);
    }
    return false;
}