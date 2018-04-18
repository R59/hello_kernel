obj-m += hello_kernel.o

ccflags-y += -std=gnu99 -Wno-declaration-after-statement

all:
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) modules

clean:
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) clean
