# if linux or macosx
CC=gcc
FLAGS = -Wall -Wextra -Werror -std=c11 --coverage
TST_LIBS = -lcheck
ifeq ($(shell uname), Linux)
	TST_LIBS += -lpthread -lrt -lsubunit -lm
endif
SRCS=$(wildcard parser/s21_*.c)
OBJS=$(SRCS:.c=.o)
PROJECT_NAME = 3D_Viewer
QMAKE_DIR = Qmake
DOC_DIR = documentation


LIB_NAME = s21_parser
TEST_FILES = $(wildcard test*.c) $(wildcard tests/*.c)

FILES_TO_FORMAT = $(wildcard **/*.c) $(wildcard **/*.cpp) $(wildcard **/**/*.c) $(wildcard **/**/*.cpp)

ifeq ($(OS),Windows_NT)
	EXE_PATH = release/3D_Viewer
	OPEN = start
else
	UNAME = $(shell uname -s)
	ifeq ($(UNAME), Darwin)
			EXE_PATH = $(PROJECT_NAME).app/Contents/MacOS/$(PROJECT_NAME)
			OPEN = open
	else ifeq ($(UNAME), Linux)
		EXE_PATH = $(PROJECT_NAME)
		OPEN = xdg-open
	endif
endif

all: uninstall clean gcov_report install dvi dist run
#all: uninstall install run

install:
	@mkdir -p $(QMAKE_DIR)
	cd $(QMAKE_DIR) && qmake ../$(PROJECT_NAME).pro && make

uninstall:
	rm -rf $(QMAKE_DIR)

clean:
	rm -rf parser/*.o test *.a *.gcov *.html *.css gcov_report/ coverage.info
	rm -rf $(QMAKE_DIR) *.pro.user
	rm -rf ../build*
	rm -rf html latex documentation
	rm -rf *.out *.app *.exe
	rm -rf test
	rm -rf **/*.gcno **/*.gcda **/**/*.gcno **/**/*.gcda
	rm -rf *.tar.gz

run:
	./$(QMAKE_DIR)/$(EXE_PATH)

dvi:
	rm -rf $(DOC_DIR)
	@doxygen dox_conf
	@$(OPEN) $(DOC_DIR)/html/index.html

dist: install
	@rm -rf dist.tar.gz
	@tar -cvzf dist.tar.gz ./$(QMAKE_DIR)/$(EXE_PATH)

clang-format:
	clang-format -style=Google -n $(FILES_TO_FORMAT)
	clang-format -style=Google -i $(FILES_TO_FORMAT)

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

$(LIB_NAME).a: $(OBJS)
	ar rc $(LIB_NAME).a $^
	ranlib $(LIB_NAME).a
	rm -rf *.o

test: $(LIB_NAME).a
	$(CC) $(FLAGS) $(TEST_FILES) -L. $(LIB_NAME).a $(TST_LIBS) -o test
	./test

gcov_report: test
#	gcov -b -l -p -c s21_*.gcno
#	gcovr -o gcov_report.html --html --html-details

	rm test*.gcda test*.gcno
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory gcov_report
	$(OPEN) gcov_report/index.html
