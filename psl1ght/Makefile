
-include Makefile.base

CFILES		:= $(wildcard stub/*.c)
SFILES		:= $(wildcard stub/*.S)

STUBS		:= $(SFILES:.S=.o) $(CFILES:.c=.o)

all: $(STUBS)
	@ppu-ld -r $(STUBS) -o stub/crt1.o
	@make -C tools/raw2h --no-print-directory
	@make -C tools/ps3load --no-print-directory
	@make -C tools/sprxlinker --no-print-directory
	@make -f Makefile.lib --no-print-directory
	@make -C libc-glue-ppu --no-print-directory
	@make -C sprx/liblv2 --no-print-directory
	@make -C sprx/libsysutil --no-print-directory
	@make -C sprx/libgcm_sys --no-print-directory
	@make -C sprx/libio --no-print-directory
	@make -C sprx/libcamera --no-print-directory
	@make -C sprx/libsysmodule --no-print-directory
	@make -C sprx/libaudio --no-print-directory
	@make -C sprx/libpngdec --no-print-directory
	@make -C lib/libreality --no-print-directory

clean:
	rm -f $(STUBS) stub/crt1.o stub.a
	@make -C tools/raw2h clean --no-print-directory
	@make -C tools/ps3load clean --no-print-directory
	@make -C tools/sprxlinker clean --no-print-directory
	@make -f Makefile.lib clean --no-print-directory
	@make -C libc-glue-ppu clean --no-print-directory
	@make -C sprx/liblv2 clean --no-print-directory
	@make -C sprx/libsysutil clean --no-print-directory
	@make -C sprx/libgcm_sys clean --no-print-directory
	@make -C sprx/libio clean --no-print-directory
	@make -C sprx/libcamera clean --no-print-directory
	@make -C sprx/libsysmodule clean --no-print-directory
	@make -C sprx/libaudio clean --no-print-directory
	@make -C sprx/libpngdec clean --no-print-directory
	@make -C lib/libreality clean --no-print-directory

install: all
	@mkdir -p $(PSL1GHT)/include $(PSL1GHT)/lib $(PSL1GHT)/bin
	@cp linker.x libpsl1ght.a libc-glue-ppu/libc-glue-ppu.a stub/crt1.o sprx/*/*.a lib/*/*.a $(PSL1GHT)/lib/
	@cp -r include $(PSL1GHT)/
	@cp Makefile.base $(PSL1GHT)/
	@cp tools/raw2h/raw2h tools/ps3load/ps3load tools/sprxlinker/sprxlinker $(PSL1GHT)/bin/
	@cp tools/PS3Py/fself.py $(PSL1GHT)/bin/
	@cp tools/PS3Py/Struct.py $(PSL1GHT)/bin/
	@cp tools/PS3Py/sfo.py $(PSL1GHT)/bin/
	@cp tools/PS3Py/sfo.xml $(PSL1GHT)/bin/
	@cp tools/PS3Py/pkg.py $(PSL1GHT)/bin/
	@cp tools/PS3Py/ICON0.PNG $(PSL1GHT)/bin/

%.o: %.c
	$(CC) -Iinclude -c $< -o $@

%.o: %.S
	$(CC) -Iinclude -c $< -o $@

.PHONY: all clean install
