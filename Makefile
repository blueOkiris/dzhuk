# Project settings

VERS :=			25.5.1

## Compiler
ifndef WINDOWS
CC :=			gcc
else
CC :=			x86_64-w64-mingw32-gcc
endif
ifdef RELEASE
ifndef WINDOWS
CFLAGS :=		-O2 -Wall -Werror -Wno-unused-function \
				-DVERS=$(VERS) \
				-Iinclude
else
CFLAGS :=		-O2 -Wall -Werror -Wno-unused-function -mwindows \
				-DWIN32 -DVERS=$(VERS) \
				-Iinclude
endif
else
CFLAGS :=		-g -Wall -Werror -Wno-unused-function \
				-DVERS=$(VERS) \
				-Iinclude
endif
ifndef WINDOWS
LD :=			gcc
else
LD :=			x86_64-w64-mingw32-gc
endif
ifdef RELEASE
ifndef WINDOWS
LDFLAGS :=		-lm
else
LDFLAGS :=		-lm
endif
else
LDFLAGS :=		-lm
endif

## Project

OBJNAME :=		dzhuk-$(VERS).bin
SRC :=			$(wildcard src/*.c)
OBJS =			$(SRC:src/%.c=obj/%.o)
HFILES =		$(wildcard include/*.h)

# Targets

## Helpers

ifdef RELEASE
ifndef WINDOWS
.PHONY: all
all: /realworld/$(OBJNAME).AppImage
else
.PHONY: all
all: /realworld/$(OBJNAME).exe
endif
else
.PHONY: all
all: $(OBJNAME)
endif

.PHONY: clean
clean:
	rm -rf obj/
	rm -rf *.d
	rm -rf $(OBJNAME)
	-rm -rf $(APPDIR)
	-rm -rf *.exe

obj/%.o: src/%.c $(HFILES)
	mkdir -p obj/
	$(CC) -o $@ $(CFLAGS) -c $<

-include $(OBJS:.o=.d)

## Main

$(foreach file,$(SRC),$(eval $(call compile_obj,$(basename $(notdir $(file))))))

ifndef WINDOWS
$(OBJNAME): $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)
else
$(OBJNAME).exe: $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)
endif
