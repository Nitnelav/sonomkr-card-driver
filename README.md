# sonomkr-card-driver

## how to add driver to rpi kernel source :

### 1. add the device tree overlay source file :

*arch/arm/boot/dts/overlays/sonomkr-card-overlay.dts*


### 2. add the resulting dtbo to the Makefile :

*arch/arm/boot/dts/overlays/Makefile*

- add the following line (for 4.9 kernel) :

```
sonomkr-card.dtbo \
```

- or the following line (for 4.4 kernel) :

```
dtbo-$(RPI_DT_OVERLAYS) += sonomkr-card.dtbo
```

### 3. add sonomkr as a vendor in :

*Documentation/devicetree/bindings/vendor-prefixes.txt*

- add the following line :

```
sonomkr        SonoMKR Project Team
```


### 4. add the new bcm sound driver :

*sound/soc/bcm/sonomkr-card.c*


### 5. add the resulting .o file to the Makefile :

*sound/soc/bcm/Makefile*

- add these two lines at the right place :

```
snd-soc-sonomkr-card-objs := sonomkr-card.o
```

```
obj-$(CONFIG_SND_SONOMKR_CARD) += snd-soc-sonomkr-card.o
```


### 6. add the associated config element in Kconfig :

*sound/soc/bcm/Kconfig*

- add the following lines:

```
config SND_SONOMKR_CARD
	tristate "Support for the SonoMKR Audio Card"
	depends on SND_BCM2708_SOC_I2S || SND_BCM2835_SOC_I2S
	select SND_SOC_WM8782
	help
	  Say Y or M if you want to add support for SonoMKR Audio Card.
```


### 7. edit both bcm2709 and bcmrpi defconfig :

*arch/arm/configs/bcm2709_defconfig*

*arch/arm/configs/bcmrpi_defconfig*

- add the following line to both files

```
CONFIG_SND_SONOMKR_CARD=m
```
