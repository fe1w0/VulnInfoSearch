static inline int crypto_rng_seedsize(struct crypto_rng *tfm)
{
	return tfm->seedsize;
}