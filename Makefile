TARGETS	:=	crt libpsl1ght libreality sprx tools

all:
	@for dir in $(TARGETS); do \
		$(MAKE) --no-print-directory -C $$dir; \
	done

clean:
	@for dir in $(TARGETS); do \
		$(MAKE) --no-print-directory -C $$dir clean; \
	done

install: install-headers install-rules
	@mkdir -p $(PSL1GHT)/host/bin
	@for dir in $(TARGETS); do \
		$(MAKE) --no-print-directory -C $$dir install; \
	done

install-headers:
	@mkdir -p $(PSL1GHT)/target/include
	@cp -r include/* $(PSL1GHT)/target/include/

install-rules:
	@mkdir -p $(PSL1GHT)/host
	@cp rules/base.mk rules/ppu.mk rules/spu.mk $(PSL1GHT)/host/
	@ln -sf $(PSL1GHT)/host/ppu.mk $(PSL1GHT)/Makefile.base
	@mkdir -p $(PS3DEV)/host/ppu/ppu/lib
	@#cp rules/lv2.ld $(PS3DEV)/host/ppu/ppu/lib/
	@sed -e 's\$$PSL1GHT\$(PSL1GHT)\' -e 's\$$PS3DEV\$(PS3DEV)\' rules/lv2.ld > $(PS3DEV)/host/ppu/ppu/lib/lv2.ld

.PHONY: all clean install install-headers
