CODE_DIR=./code
BUILD_DIR=./build
C_FLAGS= -c -g

a.out: $(BUILD_DIR)/execute.o $(BUILD_DIR)/demo.o $(BUILD_DIR)/ir_debug.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/inputbuf.o 
	g++ -o a.out $(BUILD_DIR)/execute.o $(BUILD_DIR)/demo.o $(BUILD_DIR)/ir_debug.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/inputbuf.o

$(BUILD_DIR)/demo.o: $(CODE_DIR)/demo.h
	g++ $(C_FLAGS) $(CODE_DIR)/demo.cc -o $(BUILD_DIR)/demo.o

$(BUILD_DIR)/execute.o: $(CODE_DIR)/execute.h
	g++ $(C_FLAGS) $(CODE_DIR)/execute.cc -o $(BUILD_DIR)/execute.o

$(BUILD_DIR)/ir_debug.o: $(CODE_DIR)/ir_debug.h
	g++ $(C_FLAGS) $(CODE_DIR)/ir_debug.cc -o $(BUILD_DIR)/ir_debug.o 

$(BUILD_DIR)/lexer.o: $(CODE_DIR)/lexer.h
	g++ $(C_FLAGS) $(CODE_DIR)/lexer.cc -o $(BUILD_DIR)/lexer.o 

$(BUILD_DIR)/inputbuf.o: $(CODE_DIR)/inputbuf.h 
	g++ $(C_FLAGS) $(CODE_DIR)/inputbuf.cc -o $(BUILD_DIR)/inputbuf.o

clean:
	rm $(BUILD_DIR)/*.o