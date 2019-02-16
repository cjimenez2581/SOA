# Tarea Corta 1
Interceptador de system calls utilizando `ptrace()`

Este es un repositorio para hacer la tarea corta 1 de Sistemas operativos Avanzados
El reporsitorio esta estructurado de la siguiente forma.

```
.
├── build
│   ├── child-test
│   └── tarea_corta
├── Makefile
├── prog
│   ├── child-test.c
│   └── Makefile
├── rastreador
│   ├── main.c
│   └── Makefile
└── README.md
```

## Uso del repositorio
utilizar el comando `make` en el directorio root para generar el ejecutable `tarea_corta`
(yo hice un makefile simple y borre el que fue generado por netbeans, me parece que esto
es lo mejor dado que el profesor quiere que el `Makefile` se envie junto con los .c, o sea
creo que el quiere que el `Makefile` sea hecho por nosotros y no por un IDE)

El `Makefile` crea el directorio `build` en caso de no existir y exporta 
ahi el ejecutable. (yo agregue build en .gitignore, para que git lo ignore
en el momento de que suban cualquier cambio)

El `Makefile` tambien compila `child-test` para usarlo para pruebas y tambien
lo pone en `build` para que asi quede todo en el mismo lugar que `tarea_corta`
pueda ser probado.

(aqui deberian de agregarse las funcionalidades/parametros de tarea corta)
(Un tip es que si van a editar este archivo prueben primero los cambios en dillinger.io
este es un interprete online de markdown)
(Es para que quede bonito =))
