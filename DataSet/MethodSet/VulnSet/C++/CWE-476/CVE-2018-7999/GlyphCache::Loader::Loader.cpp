GlyphCache::Loader::Loader(const Face & face, const bool dumb_font)
: _head(face, Tag::head),
  _hhea(face, Tag::hhea),
  _hmtx(face, Tag::hmtx),
  _glyf(face, Tag::glyf),
  _loca(face, Tag::loca),
  _long_fmt(false),
  _has_boxes(false),
  _num_glyphs_graphics(0),
  _num_glyphs_attributes(0),
  _num_attrs(0)
{
    if (!operator bool())
        return;

    const Face::Table maxp = Face::Table(face, Tag::maxp);
    if (!maxp) { _head = Face::Table(); return; }

    _num_glyphs_graphics = TtfUtil::GlyphCount(maxp);
    // This will fail if the number of glyphs is wildly out of range.
    if (_glyf && TtfUtil::LocaLookup(_num_glyphs_graphics-1, _loca, _loca.size(), _head) == size_t(-2))
    {
        _head = Face::Table();
        return;
    }

    if (!dumb_font)
    {
        if ((m_pGlat = Face::Table(face, Tag::Glat, 0x00030000)) == NULL
            || (m_pGloc = Face::Table(face, Tag::Gloc)) == NULL
            || m_pGloc.size() < 8)
        {
            _head = Face::Table();
            return;
        }
        const byte    * p = m_pGloc;
        int       version = be::read<uint32>(p);
        const uint16    flags = be::read<uint16>(p);
        _num_attrs = be::read<uint16>(p);
        // We can accurately calculate the number of attributed glyphs by
        //  subtracting the length of the attribids array (numAttribs long if present)
        //  and dividing by either 2 or 4 depending on shor or lonf format
        _long_fmt              = flags & 1;
        int tmpnumgattrs       = (m_pGloc.size()
                                   - (p - m_pGloc)
                                   - sizeof(uint16)*(flags & 0x2 ? _num_attrs : 0))
                                       / (_long_fmt ? sizeof(uint32) : sizeof(uint16)) - 1;

        if (version >= 0x00020000 || tmpnumgattrs < 0 || tmpnumgattrs > 65535
            || _num_attrs == 0 || _num_attrs > 0x3000  // is this hard limit appropriate?
            || _num_glyphs_graphics > tmpnumgattrs
            || m_pGlat.size() < 4)
        {
            _head = Face::Table();
            return;
        }

        _num_glyphs_attributes = static_cast<unsigned short>(tmpnumgattrs);
        p = m_pGlat;
        version = be::read<uint32>(p);
        if (version >= 0x00040000 || (version >= 0x00030000 && m_pGlat.size() < 8))       // reject Glat tables that are too new
        {
            _head = Face::Table();
            return;
        }
        else if (version >= 0x00030000)
        {
            unsigned int glatflags = be::read<uint32>(p);
            _has_boxes = glatflags & 1;
            // delete this once the compiler is fixed
            _has_boxes = true;
        }
    }
}