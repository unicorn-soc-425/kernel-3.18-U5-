#
# Makefile for the industrial I/O core.
#

obj-$(CONFIG_IIO) += industrialio.o
industrialio-y := industrialio-core.o industrialio-event.o inkern.o
industrialio-$(CONFIG_IIO_BUFFER) += industrialio-buffer.o
industrialio-$(CONFIG_IIO_TRIGGER) += industrialio-trigger.o
<<<<<<< HEAD
industrialio-$(CONFIG_IIO_BUFFER_CB) += buffer_cb.o

obj-$(CONFIG_IIO_TRIGGERED_BUFFER) += industrialio-triggered-buffer.o
obj-$(CONFIG_IIO_KFIFO_BUF) += kfifo_buf.o
=======

obj-$(CONFIG_IIO_CONFIGFS) += industrialio-configfs.o
obj-$(CONFIG_IIO_SW_DEVICE) += industrialio-sw-device.o
obj-$(CONFIG_IIO_SW_TRIGGER) += industrialio-sw-trigger.o
obj-$(CONFIG_IIO_TRIGGERED_EVENT) += industrialio-triggered-event.o
>>>>>>> v4.9.227

obj-y += accel/
obj-y += adc/
obj-y += amplifiers/
<<<<<<< HEAD
obj-y += common/
obj-y += dac/
obj-y += gyro/
obj-y += frequency/
=======
obj-y += buffer/
obj-y += chemical/
obj-y += common/
obj-y += dac/
obj-y += dummy/
obj-y += gyro/
obj-y += frequency/
obj-y += health/
>>>>>>> v4.9.227
obj-y += humidity/
obj-y += imu/
obj-y += light/
obj-y += magnetometer/
obj-y += orientation/
<<<<<<< HEAD
=======
obj-y += potentiometer/
>>>>>>> v4.9.227
obj-y += pressure/
obj-y += proximity/
obj-y += temperature/
obj-y += trigger/
