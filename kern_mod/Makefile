PROJECT = rootorium
obj-m += $(PROJECT).o

ifneq ("$(test -f /etc/os-release && echo deb)","deb")
KERN_BUILD := /lib/modules/$(shell uname -r)/build
else ifneq ("$(test -f /etc/os-release && test -f /etc/redhat-release echo rhel)","rhel")
KERN_BUILD := /usr/src/kernels/$(shell uname -r)/tools/build
endif

product:
	KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none" make -C $(KERN_BUILD) M=$$PWD modules

debug:
	KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none -DDEBUG" make -C $(KERN_BUILD) M=$$PWD modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$$PWD clean
