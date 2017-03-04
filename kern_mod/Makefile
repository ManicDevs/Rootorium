PROJECT = rootorium

obj-m += $(PROJECT).o

ifeq ($(wildcard &(/etc/os-release)),)
product:
		KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none" make -C /lib/modules/$(shell uname -r)/build M=$$PWD modules
else
product:
		KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none" make -C /usr/src/kernels/$(shell uname -r)/tools/build M=$$PWD modules
endif

ifeq ($(wildcard &(/etc/os-release)),)
debug:
	@[ -f /etc/os-release ]
		KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none -DDEBUG" make -C /lib/modules/$(shell uname -r)/build M=$$PWD modules
else
debug:
		KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none -DDEBUG" make -C /usr/src/kernels/$(shell uname -r)/tools/build M=$$PWD modules
endif

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$$PWD clean
