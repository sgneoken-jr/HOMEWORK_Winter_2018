IDIR=../include
CC=gcc
CFLAGS=-I$(IDIR) -Wall

ODIR=obj
LDIR=../lib
DDIR=debugging

LIBS= -lpthread -lrt

_DEPS = config.h myTypes.h interface.h model.h viewer.h controller.h list.h myFunctions.h globVar.h timing.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_SRC = main.c interface.c model.c viewer.c controller.c list.c myFunctions.c globVar.c timing.c

_DBG = main.o interface.o model.o viewer.o controller.o list.o myFunctions.o globVar.o timing.o
DBG = $(patsubst %,$(DDIR)/%,$(_DBG))

_OBJ = main.o interface.o model.o viewer.o controller.o list.o myFunctions.o globVar.o timing.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

haptic: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(DDIR)/%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

debug_haptic: $(DBG)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	#$(CC) $(CFLAGS) -g -o $(DBG) $(_SRC) $(DEPS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
