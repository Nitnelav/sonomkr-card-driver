snd-soc-sonomkr-card-objs := sonomkr-card.o
snd-soc-wm8782-objs := wm8782.o

obj-m += snd-soc-sonomkr-card.o
obj-m += snd-soc-wm8782.o

KDIR = /lib/modules/$(shell uname -r)

all:
	make -C $(KDIR)/build M=$(CURDIR) modules_install

install:
	cp $(CURDIR)/snd-soc-wm8782.ko $(KDIR)/
	cp $(CURDIR)/snd-soc-sonomkr-card.ko $(KDIR)/
	modprobe snd-soc-wm8782.ko
	modprobe snd-soc-sonomkr-card.ko

clean:
	make -C $(KDIR)/build M=$(CURDIR) clean
