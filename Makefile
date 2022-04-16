CC=m68k-amigaos-gcc
VASM=vasmm68k_mot
VASMFLAGS=-Faout -devpac 
CFLAGS = -D__far="" -DDEMO_DEBUG -Wall -O2 -I. -m68000
LDFLAGS = -noixemul 
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=rotation-demo.exe

all: c2p.o p2c.o $(SOURCES) $(EXECUTABLE) 

c2p.o: chunkyconverter/c2p.s
	$(VASM) $(VASMFLAGS) -o c2p.o chunkyconverter/c2p.s

p2c.o: chunkyconverter/p2c.s
	$(VASM) $(VASMFLAGS) -o p2c.o chunkyconverter/p2c.s

$(EXECUTABLE): $(OBJECTS) p2c.o c2p.o
	$(CC) $(LDFLAGS) $(OBJECTS) p2c.o c2p.o -o $@
	
clean: 
	rm *.o *.lnk *.info *.uaem $(EXECUTABLE)
