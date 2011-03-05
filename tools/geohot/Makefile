UNAME	:=	$(shell uname -s)

ifneq (,$(findstring MINGW,$(UNAME)))
CFLAGS	+=	-I"$(PS3DEV)/include"
LDFLAGS	+=	-L"$(PS3DEV)/lib" -lgmp -lcrypto -lz
POSTFIX	:=	
else
CFLAGS	+=	-I/opt/local/include -I/opt/local
LDFLAGS	+=	-L/opt/local/lib -lgmp -lcrypto -lz
POSTFIX	:=	
endif

TARGETS	:=	make_self$(POSTFIX) make_self_npdrm$(POSTFIX) package_finalize$(POSTFIX)

all: $(TARGETS)

make_self$(POSTFIX): make_self.c
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@

make_self_npdrm$(POSTFIX): make_self.c
	$(CC) $< $(CFLAGS) $(LDFLAGS) -DNPDRM -o $@

package_finalize: package_finalize.c
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@

install: $(TARGETS)
	@install -m 755 $(TARGETS) $(PSL1GHT)/host/bin/

clean:
	@rm -f $(TARGETS)

.PHONY: install clean
