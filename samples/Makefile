MAKEFILES	:=	$(wildcard **/Makefile)
SUBDIRS		:=	$(dir $(MAKEFILES))

.PHONY: all pkg clean

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

pkg:
	@for dir in $(SUBDIRS); do \
		$(MAKE) pkg -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir; \
	done
