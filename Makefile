.PHONY=clean

memory-capper: main.c
	cc $? -o $@

clean:
	rm memory-capper

install:

