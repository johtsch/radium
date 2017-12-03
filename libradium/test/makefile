CC=g++
SRC=test.cpp
EXE=test.out
LIB=-ltins
INCL= -I ../ ../libradium.a
STD=-std=c++11

new: clear
	$(CC) -o $(EXE) $(SRC) $(INCL) $(LIB) $(STD)
	
	@echo "----------------------------------------"
	@echo "	>Ausführung folgt<"
	@echo "----------------------------------------"
	@echo ""
	@echo ""
	./$(EXE)
	
clear:
	rm -f $(EXE)
