AP4_DataAtom::AP4_DataAtom(const AP4_MetaData::Value& value) :
    AP4_Atom(AP4_ATOM_TYPE_DATA, AP4_ATOM_HEADER_SIZE),
    m_DataType(DATA_TYPE_BINARY)
{
    AP4_MemoryByteStream* memory = new AP4_MemoryByteStream();
    AP4_Size payload_size = 8;
    m_Source = memory;
    
    switch (value.GetType()) {
        case AP4_MetaData::Value::TYPE_STRING_UTF_8: {
            m_DataType = DATA_TYPE_STRING_UTF_8;
            AP4_String string_value = value.ToString();
            if (string_value.GetLength()) {
                memory->Write(string_value.GetChars(), string_value.GetLength());
            }
            payload_size += string_value.GetLength();
            break;
        }
         
        case AP4_MetaData::Value::TYPE_INT_08_BE: {
            m_DataType = DATA_TYPE_SIGNED_INT_BE;
            AP4_UI08 int_value = (AP4_UI08)value.ToInteger();
            memory->Write(&int_value, 1);
            payload_size += 1;
            break;
        }
            
        case AP4_MetaData::Value::TYPE_INT_16_BE: {
            m_DataType = DATA_TYPE_SIGNED_INT_BE;
            AP4_UI16 int_value = (AP4_UI16)value.ToInteger();
            memory->Write(&int_value, 2);
            payload_size += 2;
            break;
        }

        case AP4_MetaData::Value::TYPE_INT_32_BE: {
            m_DataType = DATA_TYPE_SIGNED_INT_BE;
            AP4_UI32 int_value = (AP4_UI32)value.ToInteger();
            memory->Write(&int_value, 4);
            payload_size += 4;
            break;
        }

        case AP4_MetaData::Value::TYPE_JPEG:
            m_DataType = DATA_TYPE_JPEG;
            // FALLTHROUGH
        case AP4_MetaData::Value::TYPE_GIF: 
            if (m_DataType == DATA_TYPE_BINARY) m_DataType = DATA_TYPE_GIF;
            // FALLTHROUGH
        case AP4_MetaData::Value::TYPE_BINARY: {
            AP4_DataBuffer buffer;
            value.ToBytes(buffer);
            if (buffer.GetDataSize()) {
                memory->Write(buffer.GetData(), buffer.GetDataSize());
            }
            payload_size += buffer.GetDataSize();
            break;
        }

        default:
            break;
    }
    
    const AP4_String& language = value.GetLanguage();
    if (language == "en") {
        m_DataLang = LANGUAGE_ENGLISH;
    } else {
        // default
        m_DataLang = LANGUAGE_ENGLISH;
    }
    
    m_Size32 += payload_size;
}