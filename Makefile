all:
	make -C bios_patch
	make -C src
	head -n -1 bios_patch/SYS_ALT.HEX > ALT_FULL.HEX
	cat src/ALTBIOS.HEX >> ALT_FULL.HEX

clean:
	make -C bios_patch clean
	make -C src clean
	rm -f ALT_FULL.HEX

deploy: ALT_FULL.HEX
	mount /mnt/dso
	cp $< /mnt/dso
	sync
	umount /mnt/dso

VERSION = $(shell git describe | tr -d '.-' | head -c 3)

publish:
	make clean all
	make -C src api
	cp ALT_FULL.HEX ALT_$(VERSION).HEX
	
	rm -f alterbios-api-$(VERSION).zip
	cd api; zip -r ../alterbios-api-$(VERSION).zip *
