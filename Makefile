all: create_build tarea_corta child-test

create_build:
	mkdir -p build

tarea_corta: create_build
	gcc -o build/tarea_corta rastreador/main.c

child-test: create_build
	gcc -o build/child-test prog/child-test.c