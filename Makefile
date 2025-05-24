APP_URI := http://lv2plug.in/plugins/eg-amp
INSTALL_DIR := ~/.lv2

CC := gcc
CFLAGS := $(shell pkg-config --cflags lv2) -DAPP_URI=\"$(APP_URI)\"
LDFLAGS := $(shell pkg-config --libs lv2) --shared

.PHONY: all compile clean uninstall install run

all: compile install

compile: build/lv2_test.lv2/lv2_test.so

build/lv2_test.lv2/lv2_test.so: build/main.o | build/lv2_test.lv2/
	# Replace templating with actual file name
	sed -e "s|@LIB_EXT@|.so|g" -e "s|@APP_URI@|$(APP_URI)|g" manifest.ttl > build/lv2_test.lv2/manifest.ttl 
	sed -e "s|@LIB_EXT@|.so|g" -e "s|@APP_URI@|$(APP_URI)|g" lv2_test.ttl > build/lv2_test.lv2/lv2_test.ttl 
	$(CC) $(LDFLAGS) -o $@ $?

build/main.o: build src/main.c
	$(CC) $(CFLAGS) -o $@ -c $^

build/lv2_test.lv2/: build
	mkdir -p build/lv2_test.lv2

build:
	mkdir -p build

clean:
	rm -r build

uninstall:
	rm -rf $(INSTALL_DIR)/lv2_test.lv2

install: compile
	rm -rf $(INSTALL_DIR)/lv2_test.lv2
	cp -r build/lv2_test.lv2 $(INSTALL_DIR)/lv2_test.lv2

run: install
	jalv $(APP_URI)
