all: create_build tarea_corta child-test

create_build:
	mkdir -p build
	cp systemCalls.csv build/systemCalls.csv

tarea_corta: create_build
	gcc -o build/rastreador rastreador/main.c

child-test: create_build
	gcc -o build/test prog/child-test.c