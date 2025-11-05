build:
	rm -f ./a.out
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	g++ -std=c++20 -g -Wall -pedantic -Werror main.c ram.c tests.c -lgtest -lm -lpthread -Wno-unused-variable -Wno-unused-function -Wno-write-strings

buildcc:
	rm -f ./a.out
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	g++ -std=c++20 -g -Wall -pedantic -Werror main.c ram.c tests.c -lgtest -lm -lpthread --coverage -Wno-unused-variable -Wno-unused-function -Wno-write-strings

run:
	rm -f *.gcda
	./a.out


valgrind:
	rm -f ./a.out
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	g++ -std=c++20 -g -Wall -pedantic -Werror main.c ram.c tests.c -lgtest -lm -lpthread -Wno-unused-variable -Wno-unused-function -Wno-write-strings
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./a.out


clean:
	rm -f ./a.out
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov


submit-ram:
	/gradescope/gs submit 1130317 7088793 ram.c


submit-tests:
	/gradescope/gs submit 1130317 7088794 tests.c


commit:
	git add .
	git commit -m "$(msg)"


push:
	git push origin main


