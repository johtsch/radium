CC=g++
SRC=lang.cpp vartypes.cpp lfilehandler.cpp lstep.cpp
STD=-std=c++11
OBJ=lang.o vartypes.o lfilehandler.o lstep.o
LIBNAME=libradium.a

new: 
	$(CC) -c $(SRC) -O3 $(STD) -lpthread -ltins
	ar crs $(LIBNAME) $(OBJ)
	ranlib $(LIBNAME)