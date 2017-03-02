PROJECT = rootorium

obj-m += $(PROJECT).o

product:
	KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none" make -C /lib/modules/$(shell uname -r)/build M=$$PWD modules

debug:
	KCPPFLAGS="-std=gnu99 -O3 -Wall -Wno-incompatible-pointer-types -Wno-format-contains-nul -Wno-unused-result -Wl,--build-id=none -DDEBUG" make -C /lib/modules/$(shell uname -r)/build M=$$PWD modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$$PWD clean
