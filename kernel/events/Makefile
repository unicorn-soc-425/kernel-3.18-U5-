ifdef CONFIG_FUNCTION_TRACER
<<<<<<< HEAD
CFLAGS_REMOVE_core.o = -pg
=======
CFLAGS_REMOVE_core.o = $(CC_FLAGS_FTRACE)
>>>>>>> v4.9.227
endif

obj-y := core.o ring_buffer.o callchain.o

obj-$(CONFIG_HAVE_HW_BREAKPOINT) += hw_breakpoint.o
obj-$(CONFIG_UPROBES) += uprobes.o

