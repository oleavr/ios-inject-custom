host_arch := arm64
host_machine := arm64

CC := $(shell xcrun --sdk iphoneos -f clang) -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CFLAGS := -Wall -pipe -Os
LDFLAGS := -Wl,-dead_strip
STRIP := $(shell xcrun --sdk iphoneos -f strip) -Sx
CODESIGN := $(shell xcrun --sdk iphoneos -f codesign) -f -s "iPhone Developer"
LIPO := $(shell xcrun --sdk iphoneos -f lipo)

frida_version := 12.9.4
frida_os_arch := ios-$(host_arch)

all: bin/inject bin/agent.dylib bin/victim

clean:
	$(RM) -r bin/ obj/

deploy: bin/inject bin/agent.dylib bin/victim
	ssh iphone "rm -rf /usr/local/ios-inject-example"
	scp -r bin iphone:/usr/local/ios-inject-example

bin/inject: obj/arm64/inject obj/arm64e/inject
	@mkdir -p $(@D)
	$(LIPO) $^ -create -output $@

bin/agent.dylib: obj/arm64/agent.dylib obj/arm64e/agent.dylib
	@mkdir -p $(@D)
	$(LIPO) $^ -create -output $@

bin/victim: obj/arm64/victim obj/arm64e/victim
	@mkdir -p $(@D)
	$(LIPO) $^ -create -output $@

obj/%/inject: inject.c obj/%/frida-core/.stamp
	@mkdir -p $(@D)
	$(CC) -arch $* $(CFLAGS) -I$(@D)/frida-core inject.c -o $@ -L$(@D)/frida-core -lfrida-core -Wl,-framework,Foundation,-framework,UIKit -lresolv $(LDFLAGS)
	$(STRIP) $@
	$(CODESIGN) --entitlements inject.xcent $@

obj/%/agent.dylib: agent.c obj/%/frida-gum/.stamp
	@mkdir -p $(@D)
	$(CC) -arch $* -shared -Wl,-exported_symbol,_example_agent_main $(CFLAGS) -I$(@D)/frida-gum agent.c -o $@ -L$(@D)/frida-gum -lfrida-gum $(LDFLAGS)
	$(STRIP) $@
	$(CODESIGN) $@

obj/%/victim: victim.c
	@mkdir -p $(@D)
	$(CC) -arch $* $(CFLAGS) victim.c -o $@ $(LDFLAGS)
	$(STRIP) $@
	$(CODESIGN) $@

obj/%/frida-core/.stamp:
	@mkdir -p $(@D)
	@$(RM) $(@D)/*
	curl -Ls https://github.com/frida/frida/releases/download/$(frida_version)/frida-core-devkit-$(frida_version)-ios-$*.tar.xz | xz -d | tar -C $(@D) -xf -
	@touch $@

obj/%/frida-gum/.stamp:
	@mkdir -p $(@D)
	@$(RM) $(@D)/*
	curl -Ls https://github.com/frida/frida/releases/download/$(frida_version)/frida-gum-devkit-$(frida_version)-ios-$*.tar.xz | xz -d | tar -C $(@D) -xf -
	@touch $@

.PHONY: all clean deploy
.PRECIOUS: obj/%/frida-core/.stamp obj/%/frida-gum/.stamp
