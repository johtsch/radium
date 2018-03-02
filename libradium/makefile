CC=g++
SRC=lang.cpp vartypes.cpp lfilehandler.cpp lstep.cpp lproto.cpp lassembler.cpp lfilter.cpp lreaction.cpp vtdata.cpp
STD=-std=c++11
OBJ=lang.o vartypes.o lfilehandler.o lstep.o lproto.o lassembler.o lfilter.o lreaction.o vtdata.o
LIBNAME=libradium.a

new: 
	$(CC) -c $(SRC) $(STD) -lpthread -ltins
	ar crs $(LIBNAME) $(OBJ)
	ranlib $(LIBNAME)
