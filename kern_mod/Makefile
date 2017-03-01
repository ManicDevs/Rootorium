PROJECT = rootorium

obj-m += $(PROJECT).o

product:
	KCPPFLAGS="" make -C /lib/modules/$(shell uname -r)/build M=$$PWD modules

debug:
	KCPPFLAGS="-DDEBUG" make -C /lib/modules/$(shell uname -r)/build M=$$PWD modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$$PWD clean
