NDS-ROM-Trimmer
===============

A ROM trimmer from Nintento DS.

When to use
-----------

Generally, the size of NDS roms occupies 2^n bytes. For instance, if a ROM itself occupies 18 MB, the ROM is feeded with empty blank space at the end to make it 32 MB. This is space it's not necessary and thus can be removed.

Compile
-------

$ make

Run
---

$ bin/nds-rom-trimmer <nds.rom>

Author
------

Diego Pino <pinowsky@gmail.com>
