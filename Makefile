obj-m := led.o
led-objs:=ledmodule.o utils.o
KDIR := /home/silvianecorreia/buildroot/buildroot-2021.02.5/output/build/linux-custom
ARCH ?= arm64
CROSS_COMPILE ?=/home/silvianecorreia/buildroot/buildroot-2021.02.5/output/host/bin/aarch64-buildroot-linux-gnu-

export ARCH CROSS_COMPILE


KBUILD_TARGETS	:= clean help modules modules_install

.PHONY: all $(KBUILD_TARGETS)

all: modules
$(KBUILD_TARGETS):
	#$(MAKE) -C $(KDIR) M=$(shell pwd) $@ EXTRA_CFLAGS="$(MY_CFLAGS)"
	$(MAKE) -C $(KDIR) M=$(shell pwd) $@
