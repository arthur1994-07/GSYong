SQUIRREL= ..


OUT= $(SQUIRREL)/lib/libsqstdlib.a
INCZ= -I$(SQUIRREL)/include -I. -Iinclude

SRCS= \
	sqstdblob.cpp \
	sqstdio.cpp \
	sqstdstream.cpp \
	sqstdmath.cpp \
	sqstdsystem.cpp \
	sqstdstring.cpp \
	sqstdaux.cpp \
	sqstdrex.cpp
	
	
sq32:
	gcc -O2  -fno-rtti -Wall -fno-strict-aliasing -c $(SRCS) $(INCZ)
	ar rc $(OUT) *.o
	rm *.o

sqprof:
	gcc -O2 -pg -fno-rtti -pie -gstabs -g3 -Wall -fno-strict-aliasing -c $(SRCS) $(INCZ)
	ar rc $(OUT) *.o
	rm *.o
sq64:
	gcc -O2 -m64 -D_SQ64 -fno-rtti -Wall -fno-strict-aliasing -c $(SRCS) $(INCZ)
	ar rc $(OUT) *.o
	rm *.o
