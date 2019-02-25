# New Document# Tarea Corta 1 -- Rastreador de "System Calls"
Este programa rastrea System Calls generados en otro proceso y crea un reporte de los mismos en dos formatos: salida de consola y un archivo .txt cuyo nombre es de la forma **Report YYYY-MM-DD HH:mm:ss.txt**. Para este fin utilizamos `ptrace()` que es un system call de Linux que permite observar y controlar la ejecución de un proceso "rastreado" desde otro proceso "rastreador".

El proyecto está estructurado de la siguiente forma:
```
├── Makefile
├── rastreador
│   ├── main.c
│── systemCalls.csv
└── README.md
```
La compilación del proyecto esta organizada por el archivo `Makefile`. Este crea un folder con nombre `build` en el cual se genera el ejecutable de rastreador y donde, además, se copia el archivo **systemCalls.csv** que contiene datos de los System Calls de Linux organizados en tres columnas: Id, Nombre y Parametros.
## Uso.
1.Dentro del folder principal del proyecto, ejecutar
```
make Makefile
```
2.Mover el programa (de ahora en adelante "prog") que va a ser rastreado al folder `build`

3.Ejecutar el programa rastreador con la siguiente sintáxis


```
./rastreador [opciones de rastreador] ./prog [opciones de prog]
```
donde `[opciones de rastreador]` son:

&nbsp;&nbsp;&nbsp;a.`-v` desplegará continuamente un mensaje cada vez que detecte un System Call (opción por defecto)

&nbsp;&nbsp;&nbsp;b.`-V` desplegará uno a uno los mensajes, precedido por presionar una tecla.

4.Una vez que todos los mensajes han sido desplegados en pantalla, el programa solicita al usuario presionar una tecla para generar el reporte. Esta acción mostrara una tabla formateada con los detalles de los System Calls y a su vez escribe la misma salida a un achivo .txt dentro del folder `build`.
## Limitaciones.
1.El programa falla si se pasa mas de una opcion a `rastreador`. El siguiente ejemplo resulta en error:
```
./rastreador -v -g ./prog
```

2.Si no se pasa '-v' o '-V' a como parametro a `rastreador` y se pasa un commando de Linux como el programa a rastrear nuestro codigo responde con el error `invalid option --`. Ejemplo:

```
./rastreador ls -a
```

3.Al ejecutar en una arquitectura de 32 bits la salida no incluye los detalles adicionales de los System Calls (nombre y parámetros) 

## Autores.
*Marco Acuña Vargas.*

*Ricardo Alfaro Villalobos.*

*Randall Jiménez Morales.*

*Christopher Jiménez Valverde.*

*Renán Morera Salazar.*
