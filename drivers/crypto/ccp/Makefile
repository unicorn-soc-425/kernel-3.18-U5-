obj-$(CONFIG_CRYPTO_DEV_CCP_DD) += ccp.o
<<<<<<< HEAD
ccp-objs := ccp-dev.o ccp-ops.o
ifdef CONFIG_X86
ccp-objs += ccp-pci.o
endif
ifdef CONFIG_ARM64
ccp-objs += ccp-platform.o
endif
=======
ccp-objs := ccp-dev.o \
	    ccp-ops.o \
	    ccp-dev-v3.o \
	    ccp-dev-v5.o \
	    ccp-platform.o \
	    ccp-dmaengine.o
ccp-$(CONFIG_PCI) += ccp-pci.o
>>>>>>> v4.9.227

obj-$(CONFIG_CRYPTO_DEV_CCP_CRYPTO) += ccp-crypto.o
ccp-crypto-objs := ccp-crypto-main.o \
		   ccp-crypto-aes.o \
		   ccp-crypto-aes-cmac.o \
		   ccp-crypto-aes-xts.o \
		   ccp-crypto-sha.o
