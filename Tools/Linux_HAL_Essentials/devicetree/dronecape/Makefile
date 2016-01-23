all: BB-DRONECAPE-00A0.dtbo

%.dtbo: %.dts
	dtc -O dtb -o $@ -b 0 -@ $<

install:
	sudo cp *.dtbo /lib/firmware

clean:
	rm -f *.dtbo
