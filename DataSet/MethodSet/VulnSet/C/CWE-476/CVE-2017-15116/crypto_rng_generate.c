static inline int crypto_rng_generate(struct crypto_rng *tfm,
				      const u8 *src, unsigned int slen,
				      u8 *dst, unsigned int dlen)
{
	return tfm->generate(tfm, src, slen, dst, dlen);
}