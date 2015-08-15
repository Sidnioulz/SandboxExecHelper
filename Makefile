SOURCE_OBJS_LIB = src/lib.c src/common.c src/slist.c src/list.c src/hash.c src/realpath.c
SOURCE_OBJS_TEST = tests/test.c
TARGET_LIB = lib.so
INSTALL_LIB = libExecHelper.so
TARGET_TEST = exec-helper-test
CFLAGS ?= -O0 -DDEBUGLVL=1 -g
#CFLAGS ?= -O2 -DDEBUGLVL=0
CFLAGS_LIB = -Wall -fPIC -DPIC -shared -ldl
CFLAGS_TEST = -lrt

all: lib

test-run: test lib
	LD_PRELOAD=$(DESTDIR)/usr/lib/$(INSTALL_LIB):$(LD_PRELOAD) ./$(TARGET_TEST)

lib:
	gcc $(CFLAGS_LIB) -o $(TARGET_LIB) $(SOURCE_OBJS_LIB) $(CFLAGS)

glib:
	gcc $(CFLAGS_LIB) -o $(TARGET_LIB) $(SOURCE_OBJS_LIB) $(CFLAGS) -lglib-2.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include 

test:
	gcc $(CFLAGS_TEST) -o $(TARGET_TEST) $(SOURCE_OBJS_TEST) $(CFLAGS)

clean:
	rm *~ $(TARGET_TEST) $(TARGET_LIB) -f

install: lib
	mkdir $(DESTDIR)/usr/lib/ -p
	cp $(TARGET_LIB) $(DESTDIR)/usr/lib/$(INSTALL_LIB).0.9
	ln -fs $(DESTDIR)/usr/lib/$(INSTALL_LIB).0.9 $(DESTDIR)/usr/lib/$(INSTALL_LIB).0
	ln -fs $(DESTDIR)/usr/lib/$(INSTALL_LIB).0 $(DESTDIR)/usr/lib/$(INSTALL_LIB)
	mkdir $(DESTDIR)/etc/security/ -p
#	echo "LD_PRELOAD      DEFAULT=\"$(DESTDIR)/usr/lib/$(INSTALL_LIB)\"" >> $(DESTDIR)/etc/security/pam_env.conf


uninstall:
#	sed -i '\@ExecHelper.so@d' $(DESTDIR)/etc/security/pam_env.conf
	rm $(DESTDIR)/usr/lib/$(INSTALL_LIB) -f
	rm $(DESTDIR)/usr/lib/$(INSTALL_LIB).0 -f
	rm $(DESTDIR)/usr/lib/$(INSTALL_LIB).0.9 -f

