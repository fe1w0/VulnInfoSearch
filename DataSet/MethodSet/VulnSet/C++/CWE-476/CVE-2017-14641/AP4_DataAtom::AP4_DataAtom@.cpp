AP4_DataAtom::AP4_DataAtom(AP4_UI32 size, AP4_ByteStream& stream) :
    AP4_Atom(AP4_ATOM_TYPE_DATA, size)
{
    if (size < AP4_ATOM_HEADER_SIZE+8) return;

    AP4_UI32 i;
    stream.ReadUI32(i); m_DataType = (DataType)i;
    stream.ReadUI32(i); m_DataLang = (DataLang)i;

    // the stream for the data is a substream of this source
    AP4_Position data_offset;
    stream.Tell(data_offset);
    AP4_Size data_size = size-AP4_ATOM_HEADER_SIZE-8;
    m_Source = new AP4_SubStream(stream, data_offset, data_size);
}