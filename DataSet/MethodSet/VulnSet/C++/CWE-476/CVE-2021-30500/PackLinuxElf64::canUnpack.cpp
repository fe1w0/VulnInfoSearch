int PackLinuxElf64::canUnpack()
{
    if (super::canUnpack()) {
        return true;
    }
    if (Elf64_Ehdr::ET_DYN==get_te16(&ehdri.e_type)) {
        PackLinuxElf64help1(fi);
        Elf64_Phdr const *phdr = phdri, *last_LOAD = nullptr;
        for (unsigned j = 0; j < e_phnum; ++phdr, ++j)
            if (Elf64_Phdr::PT_LOAD==get_te32(&phdr->p_type)) {
                last_LOAD = phdr;
            }
        if (!last_LOAD)
            return false;
        off_t offset = get_te64(&last_LOAD->p_offset);
        unsigned filesz = get_te64(&last_LOAD->p_filesz);
        fi->seek(filesz+offset, SEEK_SET);
        MemBuffer buf(32 + sizeof(overlay_offset));
        fi->readx(buf, buf.getSize());
        return PackUnix::find_overlay_offset(buf);
    }
    return false;
}