#
# Makefile for the CAAM backend and dependent components
#
ifeq ($(CONFIG_CRYPTO_DEV_FSL_CAAM_DEBUG), y)
<<<<<<< HEAD
	EXTRA_CFLAGS := -DDEBUG
=======
	ccflags-y := -DDEBUG
>>>>>>> v4.9.227
endif

obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM) += caam.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_JR) += caam_jr.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_CRYPTO_API) += caamalg.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_AHASH_API) += caamhash.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_RNG_API) += caamrng.o
<<<<<<< HEAD

caam-objs := ctrl.o
caam_jr-objs := jr.o key_gen.o error.o
=======
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_PKC_API) += caam_pkc.o

caam-objs := ctrl.o
caam_jr-objs := jr.o key_gen.o error.o
caam_pkc-y := caampkc.o pkc_desc.o
>>>>>>> v4.9.227
