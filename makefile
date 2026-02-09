CXX = g++
BISON = bison
FLEX = flex
CXXFLAGS = -Iinclude

TARGET = compiler

TEST_DIR = Tests
TEST_FILES = $(wildcard $(TEST_DIR)/*.np)

QUAD_DIR = Quads
ASM_DIR = Asm

all:
	$(BISON) -d parser.y
	$(FLEX) -o lex.yy.c lexer.l
	$(CXX) $(CXXFLAGS) -o $(TARGET) parser.tab.c lex.yy.c src/*.cpp

clean:
	rm -f $(TARGET) lex.yy.c parser.tab.c parser.tab.h $(ASM_DIR)/* $(QUAD_DIR)/*

test:
	@index=1; \
	for file in $(TEST_FILES); do \
		./$(TARGET) < "$$file" "$$index"; \
		index=`expr $$index + 1`; \
	done