OBJECT := pfc.o
OBJECT += filelist.o
OBJECT += identicalfile.o

ifneq ($(CC), gcc)
CC := gcc
endif
CFLAGS := -std=gnu99

.PHONY: all clean

all: clean pfc

clean:
	-rm $(OBJECT) pfc
 
pfc: $(OBJECT)
	@echo
	@echo 'make pfc'
	$(CC) $(CFLAGS) $^ -o $@

