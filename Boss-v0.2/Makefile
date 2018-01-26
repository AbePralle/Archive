ROGUE_LIBRARIES = $(shell find Source/Rogue | grep "\.rogue$$")

all: rogue install

test: testrogue install

clean:
	rm -rf Build

testrogue: rogue
	Build/Rogue/boss Test.boss

rogue: Build/Rogue/boss

Build/Rogue/boss: Build/Rogue $(ROGUE_LIBRARIES)
	roguec Source/Rogue/BossLauncher.rogue --libraries="Source/Rogue" --output=Build/Rogue/boss --compile --debug

Build/Rogue:
	mkdir -p Build/Rogue

install: /usr/local/bin/boss

/usr/local/bin/boss:
	@printf "%s\nexec \"%s/Build/Rogue/boss\" \"%c@\"\n" '#!/bin/sh' `pwd` '$$' > boss.launcher
	cp boss.launcher /usr/local/bin/boss
	chmod a+x /usr/local/bin/boss
	rm boss.launcher

