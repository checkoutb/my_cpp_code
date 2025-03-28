

all: myapp

# compiler
CC = gcc

# where to install
INSTDIR = /usr/local/bin

INCLUDE = .

# option for development
CFLAGS = -g -Wall -ansi

# option for release
# CFLAGS = -O -Wall -ansi

myapp: main.o 2.o 3.o
	$(CC) -o myapp main.o 2.o 3.o

main.o: main.c a.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c main.c

2.o: 2.c a.h b.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c 2.c

3.o: 3.c b.h c.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c 3.c

clean:
	-rm main.o 2.o 3.o

install: myapp
	@if [ -d ${INSTDIR} ]; \
		then \
		cp myapp ${INSTDIR};\
		chmod a+x $(INSTDIR)/myapp;\
		chmod og-w ${INSTDIR}/myapp;\
		echo "installed in ${INSTDIR}";\
	else \
		echo "Sorry, ${INSTDIR} not exist";\
	fi
