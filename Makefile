


default: hoff3.c
	gcc -Wall hoff3.c argparser.c -o"hoff3"

test: testing.c
	gcc -Wall testing.c -o"testing"

argparser: argparser.c
	gcc -Wall argparser.c -o argparser

