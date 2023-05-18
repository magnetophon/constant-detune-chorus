SHELL := /usr/bin/env bash
DSP_FILES := $(wildcard *.dsp)
JAQT_TARGETS := $(basename $(DSP_FILES))
LV2_TARGETS := $(addsuffix .lv2, $(basename $(DSP_FILES)))
DESTDIR ?=
PREFIX ?= /usr/local
BINDIR ?= $(DESTDIR)$(PREFIX)/bin
LIBDIR ?= $(DESTDIR)$(PREFIX)/lib
LV2_URI_PREFIX := https://magnetophon.nl

# Get the value of XDG_CONFIG_HOME or fallback to ~/.config
XDG_CONFIG_HOME := $(shell echo $$XDG_CONFIG_HOME)
ifeq ($(XDG_CONFIG_HOME),)
	XDG_CONFIG_HOME := $(HOME)/.config
endif

.PHONY: all jaqt lv2 install install-jaqt install-lv2 clean

all: jaqt lv2

jaqt: $(JAQT_TARGETS)

lv2: $(LV2_TARGETS)

%: %.dsp
	PLUGIN_PRESETS_DIR="$(XDG_CONFIG_HOME)/$(basename $<)/"; \
	mkdir -p "$$PLUGIN_PRESETS_DIR" || true; \
	faust2jaqt -time -vec -t -1 -preset "$$PLUGIN_PRESETS_DIR" $<

%.lv2: %.dsp
	faust2lv2 -uri-prefix $(LV2_URI_PREFIX) -time -vec -gui -t -1 $<

install-jaqt: $(JAQT_TARGETS)
	install -d $(BINDIR)
	$(foreach f, $(JAQT_TARGETS), install -m 755 $(f) $(BINDIR)/$(notdir $(f));)

install-lv2: $(LV2_TARGETS)
	install -d $(LIBDIR)/lv2
	$(foreach p,$(LV2_TARGETS),cp -rfd $(p) $(LIBDIR)/lv2;)

install: install-jaqt install-lv2

clean:
	rm -rf $(JAQT_TARGETS) $(LV2_TARGETS)
