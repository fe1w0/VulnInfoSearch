GlyphCache::GlyphCache(const Face & face, const uint32 face_options)
: _glyph_loader(new Loader(face, bool(face_options & gr_face_dumbRendering))),
  _glyphs(_glyph_loader && *_glyph_loader && _glyph_loader->num_glyphs()
        ? grzeroalloc<const GlyphFace *>(_glyph_loader->num_glyphs()) : 0),
  _boxes(_glyph_loader && _glyph_loader->has_boxes() && _glyph_loader->num_glyphs()
        ? grzeroalloc<GlyphBox *>(_glyph_loader->num_glyphs()) : 0),
  _num_glyphs(_glyphs ? _glyph_loader->num_glyphs() : 0),
  _num_attrs(_glyphs ? _glyph_loader->num_attrs() : 0),
  _upem(_glyphs ? _glyph_loader->units_per_em() : 0)
{
    if ((face_options & gr_face_preloadGlyphs) && _glyph_loader && _glyphs)
    {
        int numsubs = 0;
        GlyphFace * const glyphs = new GlyphFace [_num_glyphs];
        if (!glyphs)
            return;

        // The 0 glyph is definately required.
        _glyphs[0] = _glyph_loader->read_glyph(0, glyphs[0], &numsubs);

        // glyphs[0] has the same address as the glyphs array just allocated,
        //  thus assigning the &glyphs[0] to _glyphs[0] means _glyphs[0] points
        //  to the entire array.
        const GlyphFace * loaded = _glyphs[0];
        for (uint16 gid = 1; loaded && gid != _num_glyphs; ++gid)
            _glyphs[gid] = loaded = _glyph_loader->read_glyph(gid, glyphs[gid], &numsubs);

        if (!loaded)
        {
            _glyphs[0] = 0;
            delete [] glyphs;
        }
        else if (numsubs > 0 && _boxes)
        {
            GlyphBox * boxes = (GlyphBox *)gralloc<char>(_num_glyphs * sizeof(GlyphBox) + numsubs * 8 * sizeof(float));
            GlyphBox * currbox = boxes;

            for (uint16 gid = 0; currbox && gid != _num_glyphs; ++gid)
            {
                _boxes[gid] = currbox;
                currbox = _glyph_loader->read_box(gid, currbox, *_glyphs[gid]);
            }
            if (!currbox)
            {
                free(boxes);
                _boxes[0] = 0;
            }
        }
        delete _glyph_loader;
        _glyph_loader = 0;
    }

    if (_glyphs && glyph(0) == 0)
    {
        free(_glyphs);
        _glyphs = 0;
        if (_boxes)
        {
            free(_boxes);
            _boxes = 0;
        }
        _num_glyphs = _num_attrs = _upem = 0;
    }
}