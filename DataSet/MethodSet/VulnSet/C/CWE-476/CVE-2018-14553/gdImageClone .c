BGD_DECLARE(gdImagePtr) gdImageClone (gdImagePtr src) {
	gdImagePtr dst;
	register int i, x;

	if (src->trueColor) {
		dst = gdImageCreateTrueColor(src->sx , src->sy);
	} else {
		dst = gdImageCreate(src->sx , src->sy);
	}

	if (dst == NULL) {
		return NULL;
	}

	if (src->trueColor == 0) {
		dst->colorsTotal = src->colorsTotal;
		for (i = 0; i < gdMaxColors; i++) {
			dst->red[i]   = src->red[i];
			dst->green[i] = src->green[i];
			dst->blue[i]  = src->blue[i];
			dst->alpha[i] = src->alpha[i];
			dst->open[i]  = src->open[i];
		}
		for (i = 0; i < src->sy; i++) {
			for (x = 0; x < src->sx; x++) {
				dst->pixels[i][x] = src->pixels[i][x];
			}
		}
	} else {
		for (i = 0; i < src->sy; i++) {
			for (x = 0; x < src->sx; x++) {
				dst->tpixels[i][x] = src->tpixels[i][x];
			}
		}
	}

	if (src->styleLength > 0) {
		dst->styleLength = src->styleLength;
		dst->stylePos    = src->stylePos;
		for (i = 0; i < src->styleLength; i++) {
			dst->style[i] = src->style[i];
		}
	}

	dst->interlace   = src->interlace;

	dst->alphaBlendingFlag = src->alphaBlendingFlag;
	dst->saveAlphaFlag     = src->saveAlphaFlag;
	dst->AA                = src->AA;
	dst->AA_color          = src->AA_color;
	dst->AA_dont_blend     = src->AA_dont_blend;

	dst->cx1 = src->cx1;
	dst->cy1 = src->cy1;
	dst->cx2 = src->cx2;
	dst->cy2 = src->cy2;

	dst->res_x = src->res_x;
	dst->res_y = src->res_y;

	dst->paletteQuantizationMethod     = src->paletteQuantizationMethod;
	dst->paletteQuantizationSpeed      = src->paletteQuantizationSpeed;
	dst->paletteQuantizationMinQuality = src->paletteQuantizationMinQuality;
	dst->paletteQuantizationMinQuality = src->paletteQuantizationMinQuality;

	dst->interpolation_id = src->interpolation_id;
	dst->interpolation    = src->interpolation;

	if (src->brush) {
		dst->brush = gdImageClone(src->brush);
	}

	if (src->tile) {
		dst->tile = gdImageClone(src->tile);
	}

	if (src->style) {
		gdImageSetStyle(dst, src->style, src->styleLength);
	}

	for (i = 0; i < gdMaxColors; i++) {
		dst->brushColorMap[i] = src->brushColorMap[i];
		dst->tileColorMap[i] = src->tileColorMap[i];
	}

	if (src->polyAllocated > 0) {
		dst->polyAllocated = src->polyAllocated;
		for (i = 0; i < src->polyAllocated; i++) {
			dst->polyInts[i] = src->polyInts[i];
		}
	}

	return dst;
}