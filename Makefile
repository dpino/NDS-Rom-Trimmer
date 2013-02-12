all: nds-rom-trimmer

nds-rom-trimmer: 
	mkdir bin/
	gcc src/nds_rom_trimmer.c -o bin/nds-rom-trimmer

clean:
	rm -Rf bin/
