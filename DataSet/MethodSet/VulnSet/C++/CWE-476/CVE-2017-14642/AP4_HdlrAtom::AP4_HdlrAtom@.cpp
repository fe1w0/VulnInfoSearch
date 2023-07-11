AP4_HdlrAtom::AP4_HdlrAtom(AP4_UI32        size, 
                           AP4_UI08        version,
                           AP4_UI32        flags,
                           AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_HDLR, size, version, flags)
{
    AP4_UI32 predefined;
    stream.ReadUI32(predefined);
    stream.ReadUI32(m_HandlerType);
    stream.ReadUI32(m_Reserved[0]);
    stream.ReadUI32(m_Reserved[1]);
    stream.ReadUI32(m_Reserved[2]);
    
    // read the name unless it is empty
    int name_size = size-(AP4_FULL_ATOM_HEADER_SIZE+20);
    if (name_size == 0) return;
    char* name = new char[name_size+1];
    stream.Read(name, name_size);
    name[name_size] = '\0'; // force a null termination
    // handle a special case: the Quicktime files have a pascal
    // string here, but ISO MP4 files have a C string.
    // we try to detect a pascal encoding and correct it.
    if (name[0] == name_size-1) {
        m_HandlerName = name+1;
    } else {
        m_HandlerName = name;
    }
    delete[] name;
}