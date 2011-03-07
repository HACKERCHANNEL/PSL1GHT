CFLAGS	+=	-I/opt/local/include -I/opt/local
LDFLAGS	+=	-L/opt/local/lib -lgmp -lcrypto -lz
POSTFIX	:=	

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
	@echo clean...
	@rm -f $(TARGETS) *.exe

.PHONY: install clean
