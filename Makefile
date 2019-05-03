-include make.config

PROG     ?= carve
BUILDDIR ?= build
SRCDIR   ?= src

CFLAGS += -g -O0 -std=c99 -Wall -W -pedantic -fsanitize=address
LDFLAGS += -fsanitize=address

Q ?= @

SRCS := $(sort $(wildcard $(SRCDIR)/*.c))
DEPS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%.d, $(basename $(SRCS)))
OBJS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%.o, $(basename $(SRCS)))
DIRS := $(sort $(dir $(OBJS)))

# create build directories
DUMMY := $(shell mkdir -p $(DIRS))

.PHONY: all clean

all: $(BUILDDIR)/$(PROG)

-include $(DEPS)

$(BUILDDIR)/$(PROG): $(OBJS)
	@echo "===> LD $@"
	$(Q)$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "===> CC $<"
	$(Q)$(CC) $(CFLAGS) -MMD -c -o $@ $<

clean:
	@echo "===> CLEAN"
	$(Q)rm -fr $(BUILDDIR)
