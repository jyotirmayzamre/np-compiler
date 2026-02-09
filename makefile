CXX = g++
BISON = bison
FLEX = flex
CXXFLAGS = -Iinclude

BASE = Jyotirmay_Zamre_PA4
TARGET = compiler

TEST_DIR = Tests
TEST_FILES = $(wildcard $(TEST_DIR)/*.np)

QUAD_DIR = Quads
ASM_DIR = Asm

all:
	$(BISON) -d $(BASE).y
	$(FLEX) -o lex.yy.c $(BASE).l
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(BASE).tab.c lex.yy.c src/*.cpp

clean:
	rm -f $(TARGET) lex.yy.c $(BASE).tab.c $(BASE).tab.h $(ASM_DIR)/* $(QUAD_DIR)/*

test:
	@index=1; \
	for file in $(TEST_FILES); do \
		./$(TARGET) < "$$file" "$$index"; \
		index=`expr $$index + 1`; \
	done