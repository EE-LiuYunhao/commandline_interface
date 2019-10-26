SRC := src
OBJ := obj
BIN := bin
TEST_SRC := tester_src
TEST_OBJ := tester_obj
TEST_BIN := tester_bin

myShell: $(OBJ)/*.o $(SRC)/*.h
	make clean
	g++ $^ -o $(BIN)/$@

debug: $(TEST_OBJ)/cmd.o $(TEST_OBJ)/parser.o $(TEST_OBJ)/main.o
	g++ $^ -o $(TEST_BIN)/$@ -g -Wall

parsertester: $(TEST_OBJ)/parsertester.o $(TEST_OBJ)/parser.o $(TEST_OBJ)/cmd.o
	g++ $^ -o $(TEST_BIN)/$@ -g -Wall

cmdtester: $(TEST_OBJ)/cmdtester.o $(TEST_OBJ)/cmd.o
	g++ $^ -o $(TEST_BIN)/$@ -g -Wall

$(OBJ)/cmd.o: $(SRC)/cmd.cpp $(SRC)/cmd.h $(SRC)/main.h
	g++ $< -c -o $@

$(OBJ)/parser.o: $(SRC)/parser.cpp $(SRC)/parser.h $(SRC)/cmd.h
	g++ $< -c -o $@

$(OBJ)/main.o: $(SRC)/main.cpp $(SRC)/parser.h $(SRC)/cmd.h $(SRC)/main.h
	g++ $< -c -o $@

$(TEST_OBJ)/parsertester.o: $(TEST_SRC)/parsertester.cpp $(SRC)/parser.h $(SRC)/cmd.h
	g++ $< -c -o $@ -g -Wall

$(TEST_OBJ)/cmdtester.o: $(TEST_SRC)/cmdtester.cpp $(SRC)/cmd.h
	g++ $< -c -o $@ -g -Wall

$(TEST_OBJ)/cmd.o: $(SRC)/cmd.cpp $(SRC)/cmd.h $(SRC)/main.h
	g++ $< -c -o $@ -g -Wall

$(TEST_OBJ)/parser.o: $(SRC)/parser.cpp $(SRC)/parser.h $(SRC)/cmd.h
	g++ $< -c -o $@ -g -Wall

$(TEST_OBJ)/main.o: $(SRC)/main.cpp $(SRC)/parser.h $(SRC)/cmd.h $(SRC)/main.h
	g++ $< -c -o $@ -g -Wall

clean: 
	rm -rf $(OBJ)/*.o
	rm -rf $(BIN)/*
	rm -rf $(TEST_OBJ)/*.o
	rm -rf $(TEST_BIN)/*

run: 
	./$(BIN)/myShell

.PHONY: clean run
