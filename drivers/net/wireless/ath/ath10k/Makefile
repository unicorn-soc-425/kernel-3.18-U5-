obj-$(CONFIG_ATH10K) += ath10k_core.o
ath10k_core-y += mac.o \
		 debug.o \
		 core.o \
		 htc.o \
		 htt.o \
		 htt_rx.o \
		 htt_tx.o \
		 txrx.o \
		 wmi.o \
<<<<<<< HEAD
		 bmi.o
=======
		 wmi-tlv.o \
		 bmi.o \
		 hw.o \
		 p2p.o \
		 swap.o
>>>>>>> v4.9.227

ath10k_core-$(CONFIG_ATH10K_DEBUGFS) += spectral.o
ath10k_core-$(CONFIG_NL80211_TESTMODE) += testmode.o
ath10k_core-$(CONFIG_ATH10K_TRACING) += trace.o
<<<<<<< HEAD
=======
ath10k_core-$(CONFIG_THERMAL) += thermal.o
ath10k_core-$(CONFIG_MAC80211_DEBUGFS) += debugfs_sta.o
ath10k_core-$(CONFIG_PM) += wow.o
>>>>>>> v4.9.227

obj-$(CONFIG_ATH10K_PCI) += ath10k_pci.o
ath10k_pci-y += pci.o \
		ce.o

<<<<<<< HEAD
=======
ath10k_pci-$(CONFIG_ATH10K_AHB) += ahb.o

>>>>>>> v4.9.227
# for tracing framework to find trace.h
CFLAGS_trace.o := -I$(src)
