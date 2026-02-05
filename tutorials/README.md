# HALCON-Tutorials

This repository is part of [HALCON](https://gitlab.com/hawk-dsp) and contains ***tutorials*** explaining how to develop HALCON Core-based simulators.

# Primeros pasos

## Contenido

[TOC]

## ¿Que tengo que instalar?

Los requerimientos más importantes (al menos para el tutorial) son [Docker](https://www.youtube.com/watch?v=Uu49ID0NBRQ), [Git](https://www.youtube.com/watch?v=2mxh3tgx71c) y algún entorno de Linux. Obviamente es necesario tener instalado un editor de texto y en el caso de utilizar Windows, también hay que instalar WSL (Windows Subsystem for Linux).

1. Instalar WSL:
   - Ubuntu: no hace falta 😊
   - [Windows](https://learn.microsoft.com/es-es/windows/wsl/install)

2. Instalar Git:
   - Ubuntu: no hace falta 😊
   - [Windows](https://git-scm.com/download/win): aunque es opcional, se puede hacer desde WSL

3. Instalar Docker:
   - Ubuntu: 
      ```bash
      curl -fsSL https://get.docker.com -o get-docker.sh
      sudo sh get-docker.sh
      sudo docker run hello-world
      ```
   - [Windows](https://learn.microsoft.com/es-es/windows/wsl/tutorials/wsl-containers): también hay que activar el uso de Docker en WSL

Es importante señalar que correr los simuladores en Windows con WSL es mucho más lento que correr los simuladores en Ubuntu nativo. Por lo tanto, ***se recomienda Ubuntu nativo para simulaciones largas o barridos de parámetros***.

En Windows hay que tener corriendo **Docker Desktop** para que WSL lo levante. El comando para verificar que Docker está funcionando, y todos los comandos de aquí en adelante, se corren en una terminal WSL2 con ***Ubuntu (o cualquier otra distro de Linux)***. Para definir WSL2 por defecto ejecutar el siguiente comando en una terminal de Windows (Power Shell) y reiniciar la cómputadora:

```bash
wsl --set-default-version 2
```

Después de la instalación de Docker Desktop, podemos verificar que está funcionando con el siguiente comando (en WSL2):

```bash
docker --version
```

## ¿Que repositorios clonar?

No es necesario clonar todos los repositorios ya que están enlazados entre sí mediante sub-módulos de Git. Es decir, cada repositorio incluye a los repositorios que requiere para funcionar.

Para este tutorial, es suficiente con el repositorio [Examples](https://gitlab.com/hawk-dsp/examples):

```bash
cd <algún_path>
mkdir halcon
cd halcon/
git clone https://gitlab.com/hawk-dsp/examples.git
cd examples/
git submodule update --init --recursive
```

Sin embargo, si queres clonar los principales repositorios de HALCON para participar en el desarrollo del proyecto, los comandos son los siguientes:

```bash
cd <algún_path>
mkdir halcon
cd halcon/
git clone https://gitlab.com/hawk-dsp/core.git
git clone https://gitlab.com/hawk-dsp/release.git
git clone https://gitlab.com/hawk-dsp/modules.git
git clone https://gitlab.com/hawk-dsp/examples.git
git clone https://gitlab.com/hawk-dsp/tools.git
git clone https://gitlab.com/hawk-dsp/tutorials.git
cd examples/
git submodule update --init --recursive
cd ../tutorials/
git submodule update --init --recursive
```

***Importante***: con el fin de simplificar las cosas, de aquí en adelante y a lo largo de todo el tutorial se va a hacer referencia a los directorios del proyecto con las siguientes macros:

- `{HALCON} = <algún_path>/halcon`
- `{CORE} = <algún_path>/halcon/core`
- `{RELEASE} = <algún_path>/halcon/release`
- `{MODULES} = <algún_path>/halcon/modules`
- `{EXAMPLES} = <algún_path>/halcon/examples`
- `{TOOLS} = <algún_path>/halcon/tools`

Por ejemplo, el comando `cd {HALCON}` implica cambiar al directorio `<algun_path>/halcon`.

## ¿Como lanzar el contenedor?

Antes de lanzar el contenedor hay que ubicar al archivo `dockerfile` en el repositorio y **construir el contenedor**. La construcción se hace **una única vez** a menos que se hagan modificaciones en el `dockerfile`, en tal caso hay que volver a construirlo.

1. Ubicar el archivo `dockerfile`: si clonaste Tools, el `dockerfile` está en `{HALCON}/tools/docker`. Si solo clonaste Examples, el archivo está en `{HALCON}/examples/lib/tools/docker`. Si clonaste Tutorials, el `dockerfile` está en `{TUTORIALS}/lib/tools/docker`. Para no perder generalidad en el tutorial, nos referiremos a este path como `{DOCKER}`.

2. Construir el contenedor (tarda bastante y depende de tu velocidad de internet, unos 30 minutos a 1 hora :neutral_face:):

```bash
cd {DOCKER}
sh build.sh
```

***Importante***: el contenedor **siempre** se debe construir en el directorio `{DOCKER}` porque es donde se encuentra el archivo `dockerfile`.

3. Lanzar el contenedor. Para ello, hay que ubicarse en el directorio raiz del repositorio de interés (`{EXAMPLES}`  en este caso) y ejecutar el siguiente comando:

```bash
cd {EXAMPLES}
sh ./{DOCKER}/run.sh
```

***Importante***: el contenedor siempre se debe lanzar en la raiz de un repositorio de HALCON. De este modo, siempre se mapea el directorio `/app` del contenedor la raiz del directorio que puede ser `{TUTORIALS}`, `{EXAMPLES}`, `{MODULES}`, etc.

Algunos comentarios:

1. De aquí en adelante todos los comandos se ejecutan ***dentro del contenedor*** a menos que se indique lo contrario.

2. El archivo `dockerfile` describe todo lo que el contenedor tiene instalado por defecto con sus respectivas versiones. Por ejemplo, el contenedor de este proyecto ya tiene Git, Python3, Doxygen y gcc13. Además, tiene algunas librerías de C++ y algunos módulos de Python. Para más información leer el `dockerfile`.
   
3. Todo lo que se instale dentro del contenedor luego de lanzarlo con el script `run.sh` (y no a través del `dockerfile`) se pierde al cerrar el mismo. Por lo tanto, en caso de necesitar instalar algo, hacerlo a través del `dockerfile` **y volver a construir el contenedor**.

4. Es una buena practica ser explicito con las versiones de las librerías de las que depende el proyecto (más que todo en C++). Por lo tanto, ***siempre poner las versiones de lo que se instale en el dockerfile*** porque descubrir que un bug se debe al cambio de versión de una dependencia es una tarea titánica 😬.

## ¿Como se estructura un simulador?

Para entender el árbol de directorios de un proyecto HALCON, nos ubicamos en Examples y listamos los directorios:

```bash
cd {EXAMPLES}
tree --dirsfirst -CL 2
```

El resultado será algo así:

```
|-- lib
|   |-- core
|   |-- modules
|   `-- tools
|-- low_pass_filter_sim
|   |-- build
|   |-- conf
|   |-- run
|   |-- src
|   |-- tests
|   |-- CMakeLists.txt
|   `-- main.py
|-- qam_basic_sim
|   (...)
`-- README.md
```

Cada directorio tiene una función especifica:

- `lib/`: es el directorio de dependencias del repositorio, en este se instancian los demás repositorios de los que dependen los simuladores:

  - `core/`: sub-módulo de git apuntando a [Release](https://gitlab.com/hawk-dsp/release). Esta es la versión estable de HALCON Core. En este repositorio se encuentran las librerías de C++ que implementan las funcionalidades básicas de HALCON.

  - `modules/`: sub-módulo de git apuntando a [Modules](https://gitlab.com/hawk-dsp/modules). 

  - `tools/`: sub-módulo de git apuntando a [Tools](https://gitlab.com/hawk-dsp/tools).

- `low_pass_filter_sim`: es un simulador de ejemplo. En su interior:
  
  - `build/`: directorio de compilación. ***Nunca pushear el contenido de este directorio***.
  
  - `conf/`: directorio donde se encuentra el archivo de configuración `settings.yaml` y el archivo de comandos `command.cmd`. Además puede contener un archivo extra llamado `hierarchy.txt` que es exportado por el propio simulador y es un resumen de todas las jerarquías y bloques visibles dentro del simulador. Más adelante entenderemos mejor de que se trata.
  
  - `run/`: directorio de ejecución. ***Nunca pushear el contenido de este directorio***.

  - `src/`: aquí se encuentra el archivo `main.cpp` y todos los módulos del proyecto. Este directorio es analizado en profundidad más adelante.

  - `tests/`: directorio con tests (en Python). Aquí generalmente se hacen scripts con plots de las señales que se loggean del simulador.

  - `CMakeLists.txt`: instrucciones de compilación de CMake.

  - `main.py`: script que automatiza la compilación y ejecución del simulador, entre otras cosas.

- `qam_basic_sim`: otro simulador de ejemplo. 

Es importante resaltar que en este caso `lib/` está a la misma altura que `low_pass_filter_sim/` y `qam_basic_sim` (los simuladores). Esto no siempre es asi, en este caso se trata de un repositorio donde hay multiples simuladores y, por lo tanto, las dependencias comunes se instancian una única vez. Sin embargo, cuando el repositorio contiene un único simulador, `lib/` se encuentra dentro del directorio del simulador como se observa en el repositorio.

## ¿Como correr un simulador?

Llegó el momento de correr el simulador `low_pass_filter_sim`. En este se implementan dos generadores de tonos sinusoidales, `u_sin_low` y `u_sin_high`, cuyas salidas se suman en `u_adder` y el resultado pasa a través de un filtro FIR pasa bajos, `u_filter`, que elimina uno de los tonos. Por último, la salida del filtro se exporta a un archivo. El diagrama en bloques del simulador es el siguiente:

![connection](img/example_connection.png){width=80%}

El proceso de compilación y ejecución está automatizado en el script `main.py`. Sin embargo, vamos a ejecutar **por única vez** todo a mano para entender que sucede en todo el proceso 😊.

Para compilar el proyecto nos ubicamos en `build/` y ejecutamos los siguientes comandos:

```bash
cd {EXAMPLES}/low_pass_filter_sim/build
cmake ..             # Construye el Makefile
make -j8             # Ejecuta el Makefile
make install         # Copia el binario en ../run/
```

Al correr el último comando, el resultado debería tener esta pinta:

```
[ 66%] Built target local
[100%] Built target lpf_sim
Install the project...
-- Install configuration: "Release"
-- Installing: /app/examples/low_pass_filter_sim/run/lpf_sim
```

Simulador compilado. Para correrlo, nos movemos al directorio `run/` y ejecutamos el binario:

```bash
cd {EXAMPLES}/low_pass_filter_sim/run
./lpf_sim -h         # Lista opciones (opcional)
./lpf_sim            # Ejecuta el simulador
```

Debemos tener una salida como esta:

```
-- Parsing options and flags ...
-- Options and flags were loaded succesfully

Simulation Iters: 0/1000
Simulation Iters: 10/1000
...
Simulation Iters: 1000/1000
```

Simulador ejecutado. Ahora vamos a levantar señales y hacer gráficas. Para ello, nos movemos al directorio `tests/` y ejecutamos el script `plot.py`:

```bash
cd {EXAMPLES}/low_pass_filter_sim/tests
python3 plot.py            # Ejecuta el script de procesamiento
```

Este script levanta las señales loggeadas que se encuentran por defecto en `run/logs` y hace algunas gráficas. Por ultimo, las exporta a un archivo `.png` que tiene esta pinta:

![plots](img/plots_low_pass_filter.png "Plots"){width=80%}

Claramente hacer todo este proceso a mano es una perdida de tiempo. El script `main.py` automatiza la compilación, ejecución del simulador y ejecución de tests en una sola linea:

```bash
cd {EXAMPLES}/low_pass_filter_sim/
python3 main.py -h         # Lista opciones (opcional)
python3 main.py -f -r -t   # Compila (-f), ejecuta (-r) y corre plot.py (-t) 
```

***Importante***: el script `main.py` tiene dos flags de compilación diferentes, `-p` y `-f`. La primera opción compila el proyecto ***solo simulando los flancos positivos de los clocks*** mientras que la segunda compila una versión donde ***ambos flancos son simulados***. Si la aplicación no demanda simular los flancos negativos de los relojes, se recomienda compilar con `-p` ya que en este modo el tiempo de ejecución del binario es la mitad que para el caso compilado con `-f`. Sin embargo, la versión `-f` está más debugeada asi que por ahora usamos esa :smile:.

## ¿Como cambio las configuraciones?

El archivo de configuración `settings.yaml` se encuentran en `conf/`. No es necesario construir este archivo a mano, el mismo simulador es capaz de hacerlo desde cero y actualizarlo a medida que se hacen cambios en él. Para ello, luego de compilar el simulador, ejecutar el binario con la flag `-e` (esto si hacemos las cosas a mano, si usamos el `main.py` no es necesario):

```bash
cd {EXAMPLES}/low_pass_filter_sim/run
./lpf_sim -e         # Exporta o actualiza el settings.yaml
```

Si el archivo `settings.yaml` no existe, el simulador lo crea y completa cada parámetro con el valor **por defecto** de las variables internas del simulador. En caso de existir el archivo, lo actualiza agregando posibles nuevas variables con sus respectivos valores por defecto pero ***no modifica*** las variables previamente definidas en el `settings.yaml` ya existente.

Para este simulador de ejemplo `low_pass_filter_sim/`, el `settings.yaml` es el siguiente:

```yaml
root:
  enable_log: 1
  fs_clk: 10000000.0
  logger_buffer_size: 1000
  n_iterations: 10000
  u_filter:
    coeffs: [1, 1, 1, 1, 1]
  u_sin_high:
    amplitude_v: 1
    frequency_hz: 1500000.0
    phase_deg: 0
  u_sin_low:
    amplitude_v: 1
    frequency_hz: 1000000.0
    phase_deg: 0
```

Por el momento, nos enfoquemos en correr el simulador variando los parámetros de los módulos `u_sin_low`, `u_sin_high` y `u_filter` (en este último no se puede cambiar la cantidad de taps, solo se puede variar el valor de los mismos). Más adelante entenderemos que hacen las demás configuraciones.

También podemos probar con borrar algunas configuraciones, o el archivo completo, y ejecutar `./lpf_sim -e` para ver como el simulador lo exporta nuevamente con sus valores por defecto.

## ¿Que comandos están soportados?

El archivo de comandos `command.cmd`, que también se encuentra en `conf/`, soporta tres tipos de comandos clasificados según su funcionalidad: loggeo de señales , `LOG`, seteo de señales, `SET` y loggeo de señales al final de la simulación `FLOG`. Además, el archivo soporta comentarios con `#` al inicio de la linea.

**Importante**: nuevamente, esto está automatizado con el script `main.py`. Esta sección es meramente informativa para entender que está pasando y que significan los parámetros que estamos configurando desde Python. Continuamos ...

Los comandos `LOG` permiten maestrear señales del simulador con un clock y flanco específicos, durante una ventana de tiempo determinada y con un diezmado (paso) arbitrario. Luego las señales se exportan a archivos `.bin` o `.txt`. La sintaxis de estos comandos es la siguiente:

```html
LOG -s <signal> -c <clock> -e <edge> -b <begin> -p <step> -d <end> -t <file_type> -n <file_name> -f <format>
```

Las funcionalidades de parámetros que definen al comando `LOG` son:

- `-s | --SIGNAL`: es la señal a loggear y se define de forma jerárquica. Se parte desde root y de ahí continúa hasta la variable requerida, utilizando los nombres de las instancias para formar la jerarquía.
- `-c | --CLOCK`: es el clock con el que se loggea la señal.
- `-e | --EDGE`: flanco del clock con el que se loggea `[p:posedge | n:negedge]`.
- `-b | --BEGIN`: número de tick del ***clock de referencia*** en donde empieza el loggeo.
- `-p | --STEP`: paso o diezmado, es decir, cada cuántos ticks del clock se toma una muestra (por ejemplo, `-p 1` toma todas, `-p 2` toma una cada dos).
- `-d | --END`: número de tick del ***clock de referencia*** en donde termina el loggeo.
- `-t | --FILETYPE`: tipo de archivo de salida, `[t:txt | b:bin]`.
- `-n | --FILENAME`: formato del nombre del archivo de salida `[l:long | s:short]`
- `-f | --FORMAT`: formato de los datos en el archivo de salida (HEX, FLOAT, etc).

Por ejemplo el siguiente comando hace un `LOG` de la variable `root.u_sin_low.amplitude` en los flancos positivos del clock `root.clk` desde el tick `0` al tick `100` con paso `1` del ***clock de referencia*** `root.clk_cmd_handler` y exporta las muestras a un archivo `txt`:

```
LOG -s root.u_sin_low.amplitude -c root.clk -e p -b 0 -p 1 -d 100 -t txt
```

Además, se puede colocar el valor `-d | --END` en `0`, si se quiere hacer un `LOG` de una señal durante toda la simulación. Este es el único caso, donde el valor de inicio del `LOG` puede ser mayor al del final:

```
LOG -s root.u_sin_low.amplitude -c root.clk -e p -b 0 -p 1 -d 0 -t txt
```

Los comandos `SET` permiten definir el valor de señales del simulador con un clock y flanco específicos y durante una ventana de tiempo determinada. La sintaxis es similar a la de los comandos `LOG`:

```html
SET -v <value> -s <signal_name> -c <clock_name> -e <edge_type> -b <first_tick> -d <last_tick>
SET -f <txt_file> -s <signal_name> -c <clock_name> -e <edge_type> -b <first_tick> -d <last_tick>
```

Los parametros de los comandos `SET` repetidos con los comandos `LOG` tienen el mismo significado. Los parametros restantes son:

- `-v | --VALUE`: Valor a settear en la variable, puede ser un número o un array del tipo `"[1, 2, 3, ... ]"` si la variable destino es de este tipo.
- `-f | --FILE`: si la variable destino es un array, se la puede cargar directamente desde un archivo. Esto es útil cuando tenemos arrays grandes, o generamos el archivo con una herramienta externa como MATLAB o Python. El formato del archivo es `"[1, 2, 3, ... ]"`.

Por ejemplo el siguiente comando hace un `SET` del arreglo `root.u_filter.taps` en los flancos positivos del clock `root.clk` desde el tick `0` al tick `100` del ***clock de referencia*** `root.clk_cmd_handler`:

```
SET -v "[0, 1, 0, 0]" -s root.u_filter.taps -c root.clk -e p -b 0 -p 1 -d 100
```

***Importante***: tenga en cuenta que **la ventana de tiempo** (`-b` y `-d`) de los comandos está definida en ticks del clock de referencia `root.clk_cmd_handler` y no del clock con el que se está loggeando la señal. La frecuencia de este clock es configurable y define que tan rápido se pueden hacer los LOGs y SETs del sistema.

## ¿Que señales son loggeables o seteables?

La estructura del simulador (todas las jerarquías) son exportadas automáticamente por el simulador al archivo `conf/hierarchy.txt` cuando se ejecuta el binario con la opción `-e`:

```bash
cd {EXAMPLES}/low_pass_filter_sim/run
./lpf_sim -e         # Exporta hierarchy.txt en conf/

cd {EXAMPLES}/low_pass_filter_sim/conf
cat hierarchy.txt
```

El resultado del último comando es:

```
-module                       root
+clock                        root.clk
*variable_unsigned_long       root.clk.i_division_factor_den
*variable_unsigned_long       root.clk.i_division_factor_num
*variable_long_double         root.clk.i_frequency_hz
*variable_long_double         root.clk.i_phase_deg
*variable_long_double         root.clk.last_edge_time
*variable_unsigned_long_long  root.clk.n_edges
*variable_unsigned_long_long  root.clk.n_ticks
*variable_long_double         root.clk.next_edge_time
*variable_char                root.clk.next_edge_type
*variable_bool                root.clk.state
+clock                        root.clk_cmd_handler
*variable_unsigned_long       root.clk_cmd_handler.i_division_factor_den
*variable_unsigned_long       root.clk_cmd_handler.i_division_factor_num
*variable_long_double         root.clk_cmd_handler.i_frequency_hz
*variable_long_double         root.clk_cmd_handler.i_phase_deg
*variable_long_double         root.clk_cmd_handler.last_edge_time
*variable_unsigned_long_long  root.clk_cmd_handler.n_edges
*variable_unsigned_long_long  root.clk_cmd_handler.n_ticks
*variable_long_double         root.clk_cmd_handler.next_edge_time
*variable_char                root.clk_cmd_handler.next_edge_type
*variable_bool                root.clk_cmd_handler.state
*variable_bool                root.enable_log
*variable_long_double         root.fs_clk
*variable_unsigned_long       root.logger_buffer_size
*variable_unsigned_long       root.n_iterations
-module                       root.u_adder
+port                         root.u_adder.i_high
+port                         root.u_adder.i_low
+port                         root.u_adder.o_sum
-module                       root.u_filter
*variable_double              root.u_filter.coeffs
+port                         root.u_filter.i_signal
+port                         root.u_filter.o_shift_reg
+port                         root.u_filter.o_signal
-register                     root.u_filter.r_out
*variable_double              root.u_filter.r_out.i
*variable_double              root.u_filter.r_out.o
-register                     root.u_filter.r_shift_reg
*variable_double              root.u_filter.r_shift_reg.i
*variable_double              root.u_filter.r_shift_reg.o
-module                       root.u_sin_high
*variable_double              root.u_sin_high.amplitude_v
*variable_double              root.u_sin_high.frequency_hz
+port                         root.u_sin_high.o_sin
*variable_double              root.u_sin_high.phase_deg
-register                     root.u_sin_high.r_out
*variable_double              root.u_sin_high.r_out.i
*variable_double              root.u_sin_high.r_out.o
-module                       root.u_sin_low
*variable_double              root.u_sin_low.amplitude_v
*variable_double              root.u_sin_low.frequency_hz
+port                         root.u_sin_low.o_sin
*variable_double              root.u_sin_low.phase_deg
-register                     root.u_sin_low.r_out
*variable_double              root.u_sin_low.r_out.i
*variable_double              root.u_sin_low.r_out.o

```

Cada linea inicia con alguno de los caracteres `-`, `+` y `*`. Estos especifican que se puede hacer con el modulo/clock/variable/puerto que se indica al final de la linea:

- `-`: `LOG` y `SET` no son soportados
- `+`: solo los comandos `LOG` son soportados
- `*`: ambos comandos son soportados

## ¿Como se construye un simulador?

Para entender como está construido `low_pass_filter_sim` analizaremos todos los archivo de C++ que componen su directorio fuente. Para ello, nos ubicamos en `src/` y listamos su contenido:

```bash
cd {EXAMPLES}/low_pass_filter_sim/src
tree -CL 2
```
Obteniendo:

```
|-- main.cpp
|-- adder
|   |-- adder.cpp
|   `-- adder.hpp
|-- filter
|   |-- filter.cpp
|   `-- filter.hpp
|-- root
|   |-- root.cpp
|   `-- root.hpp
`-- sin_generator
    |-- sin_generator.cpp
    `-- sin_generator.hpp
```

En el directorio `src/` ***siempre*** encontraremos al archivo `main.cpp` y, dependiendo del proyecto, un listado de directorios. Cada directorio representa a un ***módulo*** del proyecto y en él se encuentran todos los archivos fuente que describen la funcionalidad del mismo.

Este simulador está formado por cuatro módulos: `adder`, `filter`, `sin_generator` y por último, y más importante, `root`. La funcionalidad de los primeros tres es predecible, pero ***¿que implementa root?***. La respuesta es muy simple, ***root es la descripción del simulador propiamente dicho***. Es el módulo que instancia a los demás módulos, los conecta entre sí, lleva adelante la simulación, entre otras cosas.

Si `root` es el simulador, ¿que hace `main.cpp`?

## ¿Que hace el main.cpp?

El archivo `main.cpp` es igual en todos los simuladores y su principal funcionalidad es instanciar un objeto de tipo `Root` y llamar a algunos de sus métodos. A continuación, el `main.cpp` de `low_pass_filter_sim`:

```cpp
#include "root.hpp"

int main(int argc, char *argv[])
{
    Root root;
    root.ProcessOptions(argc, argv);
    root.Run();
    return 0;
}
```

***Importante***: en este caso el módulo donde se describe al simulador se llama `root` pero esto no necesariamente tiene que ser así. El diseñador puede reemplazar este nombre por otro nombre sin problemas. Sin embargo, en el archivo de configuración y en el loggeo y seteo de señales, ***las jerarquías siempre inician con root***, aún cuando el bloque principal tenga otro nombre. Por lo tanto, para ser consistentes, se recomienda utilizar el nombre `root`.

## ¿Que es Root?

Todos los módulos de `src/`, incluido `root`, los escribe el usuario. Sin embargo, los módulos comunes (o simplemente módulos) y el módulo `root` son diferentes. Este último consiste en una clase derivada de `Simulator` cuyos atributos son otros bloques constructivos del simulador (tales como `SinGenerator`, `Adder`, `Filter`) y sus métodos son funciones preestablecidas de HALCON que el usuario ***está obligado a definir***. El `root.hpp` de `low_pass_filter_sim` es el siguiente:

```cpp
#include "halcon.hpp"
#include "sin_generator.hpp"
#include "adder.hpp"
#include "filter.hpp"

class Root : public Simulator {
public:
   Root();
private:
   /* Pre-established methods */
   void Init() override;
   void Connect() override;
   void Iteration() override;
   bool ContinueRunning() override;
   void Terminate() override;
   
   /* Other modules */
   Clock clk;
   SinGenerator u_sin_low;
   SinGenerator u_sin_high;
   Adder u_adder;
   Filter u_filter;
   
   /* Variables */
   unsigned int n_iterations { 100 };
};
```

Note que en `main.cpp` se utilizan los métodos `Run()`, `ParseOptions()` y `ExportFiles()` que el usuario no define en la clase `Root`. Estos métodos son propios de HALCON y son heredados de `Simulator`, clase de la que ***obligatoriamente debe heredar*** `Root`.

Además, la clase `Simulator` tiene bloques predefinidos que llevan a cabo tareas concretas que le brindan al usuario algunas de las funcionalidades ya mencionadas, como por ejemplo la posibilidad de loggear y setear señales automáticamente. Los bloques listados a continuación, en formato `ClassName | object_name`, son algunos de los bloques más importantes definidos en `Simulator`:

- `Scheduler | scheduler`: es el encargado de ordenar todos los clocks del sistema en función del tiempo de su próximo flanco para que estos se ejecuten en el orden correspondiente. ***Todos los clocks son registrados en el scheduler automáticamente cuando se crean***.
  
- `CommandHandler | command_handler`: destinado a procesar el archivo `command.cmd` y enviar a los objetos `logger` y `setter` los comandos tipo `LOG`, `FLOG` y `SET` en el instante de tiempo que correspondan.
  
- `Logger | logger`: ejecuta los comandos tipo `LOG` que recibe del `command_handler`. Es el encargado de crear los archivos, mantenerlos abiertos y cerrarlos cuando corresponda.
  
- `Setter | setter`: ejecuta los comandos tipo `SET` que recibe del `command_handler`.

Al archivo `root.cpp` de `low_pass_filter_sim` lo analizaremos por partes, iniciando con el constructor de la clase `Root` que tiene la particular tarea de ***reflejar*** el contenido del bloque:

```cpp
Root::Root() {
    /* Clocks */
    REFLECT(clk);
    
    /* Modules */
    REFLECT(u_sin_low);
    REFLECT(u_sin_high);
    REFLECT(u_adder);
    REFLECT(u_filter);
    
    /* Settings YAML */
    REFLECT_YAML(n_iterations);
}
```

En pocas palabras, la reflexión es una técnica de programación que le permite a un programa conocer información de sí mismo. Información como las variables que tiene definidas, el nombre de las mismas, donde se ubican en memoria, etc. En este caso, HALCON necesita conocer qué se define adentro de cada bloque constructivo del simulador, para poder construir el árbol de jerarquía de bloques que luego utiliza para inicializar el simulador desde el `settings.yaml`, loggear señales y setearlas.

***Importante***: en todos los constructores de las clases, se deben ***reflejar obligatoriamente*** todos los ***sub-módulos*** (ej: `u_sin_low`, `u_filter`, etc), ***puertos***, ***registros***, ***clocks*** y ***variables***. En el caso de las variables, solo se deben reflejar las que se deseen loggear, setear o configurar desde el `settings.yaml`. La reflexión se implementa mediante la macro `REFLECT()`. Y el caso de `REFLECT_YAML()` es un caso especialdonde se agrega la posibilidad de exportar esa variable al `settings.yaml` automáticamente.

A esta altura el simulador tiene algunos bloques creados pero estos no están conectados entre sí. El método `Connect()` describe como se conectan los bloques internos a través de sus puertos con el operador `<<`:

```cpp
void Root::Connect()
{
    /* Clock */
    clk.i_frequency_hz << fs_clk;
    clk.i_phase_deg.SetData(0);
    clk.i_division_factor_num.SetData(1);
    clk.i_division_factor_den.SetData(1);

    /* Modules */
    u_sin_low.i_clock << clk;
    u_sin_high.i_clock << clk;
    
    u_adder.i_low << u_sin_low.o_sin;
    u_adder.i_high << u_sin_high.o_sin;

    u_filter.i_clock << clk;
    u_filter.i_signal << u_adder.o_sum;
};
```

Aunque aún no sabemos como están implementados los bloques `Clock` o `SinGenerator` en su interior, el simulador (o `Root` para ser más precisos) ya los tiene instanciados, reflejados y conectados entre sí. El proceso de descripción funcional del simulador termina aquí.

Los métodos restantes de `Root`, `Init()`, `Iteration()`, `ContinueRunning()` y `Terminate()`, son métodos de ***control de flujo*** de simulación. Es decir, son funciones que se ejecutan en regiones específicas del proceso de ejecución del simulador que le permiten al usuario agregar código a medida para evaluar aspectos del simulador. Si tuviéramos que simplificar al máximo el proceso de ejecución del simulador sería algo así:

```cpp
Root();
Connect();
Init();
iteration_counter = 0;
do {
   
   /* HALCON tasks ... */
   
   Iteration();
   iteration_counter++;
} while( ContinueRunning() )
Terminate();
```

A partir de este pseudo-código podemos deducir la funcionalidad de cada uno de los métodos:

- `Init()`: una vez finalizada la etapa de inicialización del simulador, esta es la primer función que se ejecuta. Se utiliza para mostrar advertencias o el resultado de la inicialización.

- `Iteration()`: el proceso de ejecución del simulador es un loop en donde se ejecutan tareas, la última en ejecutarse en cada iteración es una llamada a esta función. Es decir, esta es llamada de forma continua durante la simulación, por lo tanto, el código en su interior tiene que ser lo más óptimo posible para no afectar el rendimiento del simulador.

- `ContinueRunning()`: es la condición de finalización del loop que mencionamos anteriormente. Si retorna `False` la simulación finaliza.

- `Terminate()`: una vez finalizada la simulación se ejecuta este método. En este podemos imprimir algún resultado por pantalla o guardar un reporte en un archivo.

Por ejemplo, para el simulador `low_pass_filter_sim` estos métodos se definen como sigue:

```cpp
void Root::Init() {
   std::cout << "Inicialización terminada" << std:endl;
}

void Root::Iteration() {
   std::cout << GetIterationCounter() << "/" << n_iterations << std:endl;
}

bool Root::ContinueRunning(){
    return GetIterationCounter() <= n_iterations;
}

void Root::Terminate() {
   std::cout << "Simulación terminada" << std:endl;
}
```

## ¿Y los módulos, registros y puertos?

Los módulos son clases que heredan de la clase `Module` tal como `Root` hereda de `Simulator`. La clase `Module` obliga al usuario a definir algunos métodos y por defecto tiene algunas funcionalidades implementadas. Los módulos pueden contener a otros módulos (sub-módulos), registros, puertos, clocks y variables.


`SinGenerator`, `Adder` y `Filter` son módulos y todos siguen la misma estructura. Por lo tanto, nos centraremos en entender como se diseñó `Filter` cuyo diagrama en bloques es el siguiente:

![FIRFilter](img/fir_filter.png "FIR Filter"){width=80%}

Para empezar, analicemos algunas lineas del archivo `filter.hpp`:

```cpp
#include <array>
#include "halcon.hpp"

#define NTAPS 5

class Filter : public Module
{
private:

   /* (1) */

    /* Internal vars */
    std::array<double, NTAPS> coeffs {1, 1, 1, 1, 1};
    double sum { 0 };

public:
    Filter();
    
    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;
    
    /* (2) */
};
```

`Filter` es una clase desarrollada por el usuario que obligatoriamente debe heredar de `Module`. Esta última es una clase que, como `Simulator`, establece algunos métodos que el usuario tiene que definir para integrar el bloque en HALCON. Estos son:

- `Filter()` (constructor): al igual que en `Root`, en el constructor de la clase que define al módulo se debe usar la macro `REFLECT()` para reflejar sub-módulos, registros, puertos, variables y clocks.

- `Init()`: luego del proceso de configuración de los módulos, donde se levantan los parámetros del archivo `settings.yaml`, se llama a la función `Init()` de cada módulo en la cual el usuario puede computar, o realizar, configuraciones que dependen de algún parámetro configurado a través del `settings.yaml`.

- `Connect()`: al igual que en `Root`, esta función se utiliza para conectar sub-módulos, clocks, registros y variables dentro de un módulo. Se realiza de la misma manera que en `Root`, utilizando el operador `<<`.

- `RunClockMaster()`: si se imagina una señal de clock, esta está constituida por dos flancos, uno positivo y uno negativo, y dos intervalos de tiempo de medio periodo entre los flancos. Durante estos intervalos, en un sistema real, es donde las señales eléctricas se propagan y se establecen los nuevos niveles en las entradas a los registros. Ese intervalo de tiempo entre flancos, donde se propagan las señales, es lo que modela este método. Por lo tanto, esta función es ejecutada por el core de HALCON dos oportunidades por periodo de clock, una antes de un flanco positivo y otra antes de un flanco negativo.
  

Antes de ver la implementación de estos métodos, estudiaremos como modelar los registros requeridos para implementar el filtro y los puertos a través de los cuales este se va a conectar con otros módulos. Definir esto antes de profundizar en la implementación de los métodos nos permitirá entender como se utilizan las clases `Register` y `Port`.

Empecemos con la clase `Register`. En `/* (1) */` (en el último segmento de código) podemos definir un arreglo de registros y un registro individual para la salida del módulo como:

```cpp

   /* Registers */
   Register<double, NTAPS - 1> r_shift_reg { 0 };  /* Arreglo de registros */
   Register<double> r_out { 0 };                   /* Registro individual */
```

La clase `Register` es parte de HALCON y modela un registro con dos atributos públicos `i` y `o` que representan la entrada y la salida, respectivamente, y un método `RunClock()` que realiza la transferencia de información de `i` a `o`. Si el registro responde a flancos positivos o flancos negativos del clock depende de si, como se verá en el método `Connect()`, se utiliza la función del clock `RegisterOnPositiveEdge()` o de `RegisterOnPositiveEdge()`, respectivamente.

Para definir los puertos del módulo `Filter` hacemos uso de la clase `Port` de HALCON. En `/* (2) */` agregamos el siguiente código:

```cpp
   /* Ports */
   Input<Clock> i_clock;                           /* Entrada de tipo Clock */
   Input<double> i_signal;                         /* Entrada de señal */
   Output<double> o_signal;                        /* Salida de señal */
   Output<double, NTAPS> o_shift_reg;              /* Salida de registros (array) */
```

Una forma simple de entender el concepto de puertos es mediante punteros. Todos los puertos al fin del día implementan un puntero a una variable. De este modo un módulo puede leer la salida de otro sin tener que hacer copias de las señales. Además, como la conexión se realiza a través de la clase `Port` y no a través de punteros tradicionales de C++, la clase inhabilita la escritura de la variable a la que apunta el puerto, evitando así que los módulos puedan interferirse entre sí.

Con estos dos conceptos en mente (registros y puertos) veamos la implementación de los métodos de `Filter` empezando con el constructor que hace `REFLECT()` de registros, puertos y variables configurables desde el `settings.yaml`:

```cpp
Filter::Filter() {
    /* Registers */
    REFLECT(r_shift_reg);
    REFLECT(r_out);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);
    REFLECT(o_shift_reg);

    /* Settings YAML */
    REFLECT_YAML(coeffs);
}
```

Continuamos con los métodos `Init()` y `Connect()`. En este último, además de las conexiones implementadas con `<<`, se registra a `Filter` en el `Clock` con el método `RegisterOnPositiveEdge()`. Esto le envía un puntero que le indica el módulo que se está utilizando, es decir `Filter` para este caso, y le indica cuál es el registro que se desea incluir en ese flanco de `Clock`.

```cpp
void Filter::Init() {
   /* Pass */
}

void Filter::Connect() {
   /* Sensibilidad de los registros */
   i_clock->RegisterOnPositiveEdge(this, r_shift_reg);
   i_clock->RegisterOnPositiveEdge(this, r_out);

   /* Puerto de salida */
   o_signal << r_out.o;

   /* Puerto de registros */
   for(size_t i { 1 }; i < NTAPS; ++i)
   {
      o_shift_reg[i] << r_shift_reg.o[i - 1];
   }

   /* Puerto de entrada */
   o_shift_reg[0] << i_signal;
}
```

Observe que los arreglos de puertos (ya sea de entrada o salida) se indexan directamente con el operador `[]` mientras que para indexar arreglos de registros, primero se debe especificar si es la entrada o la salida (`i` u `o`) y luego se indexa con el operador `[]`.

Por último, las clases asociadas a las etapas del clock. En `RunClockMaster()` se definen todas las entradas de los registros:

```cpp
void Filter::RunClockMaster() {
    /* Shift register */
    for(size_t i { 1 }; i < NTAPS - 1; ++i)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = i_signal.GetData();

    /* Products */
    sum = i_signal.GetData() * coeffs[0];
    for(size_t i { 1 }; i < NTAPS; ++i)
    {
        sum += r_shift_reg.o[i - 1] * coeffs[i];
    }
    r_out.i = sum;
}
```

## ¿Como funcionan los clocks y el scheduler?

El `Clock` es el bloque encargado de modelar el reloj. Una señal de reloj es una onda cuadrada que se repite cada `T` o con una frecuencia de `1/T`. Este modela tanto el flanco positivo como negativo de la señal. Todos los bloques de hardware que diseñemos y que tengan lógica secuencial deben estar registrados en un clock, ya que el modelo de clock tiene listas de módulos asociados. Además, debe especificarse al menos un flanco de reloj (positivo o negativo) del cual depende la lógica secuencial del bloque.  Para esto, el clock tiene dos métodos que podemos utilizar dentro del método `Connect()` del bloque en cuestión, estos son:
    
- `RegisterOnPositiveEdge(Module *module_ptr, AbstractRegister &reg_ref)`: registra un registro (`reg_ref`) que se encuentra en el módulo (`module_ptr`) para que sea ejecutado en el flanco positivo del reloj.

- `RegisterOnNegativeEdge(Module *module_ptr, AbstractRegister &reg_ref)`: registra un registro (`reg_ref`) que se encuentra en el módulo (`module_ptr`) para que sea ejecutado en el flanco negativo del reloj.

El `Scheduler` del simulador es el componente encargado de ejecutar el pulso del reloj cuyo flanco, ya sea ascendente o descendente, se encuentra temporalmente más próximo al instante actual. Observe el siguiente diagrama:

![Clocks](img/scheduler.png "Clocks"){width=80%}

Este es un diagrama temporal de un scheduler que tiene asociado 3 clocks (`CLOCK_1`, `CLOCK_2`, `CLOCK_3`). El `CLOCK_1` y `CLOCK_2` tienen la misma frecuencia pero diferente fase inicial, el `CLOCK_3` tiene un tercio de la frecuencia de los otros dos.

En la tabla al costado de la imagen vemos que flanco se ejecuta en cada evento. En el evento `#1` el scheduler le indicara al `CLOCK_1` que ejecute el método `RegisterOnNegativeEdge()` de todos los módulos que tenga asociados a esta condición, y hará la mismo con el `CLOCK_3` pero diciéndole que ejecute el método `RegisterOnPositiveEdge()`. Cada uno de estos eventos representa una iteración del bucle principal del simulador.

## ¿Hay reglas para programar?

Si y es sumamente importante respetarlas para mantener la legibilidad de los simuladores. Estas se listan a continuación:

- Formatos:
   - Directorios y archivos en `snake_case`
   - Archivos README.md en mayúscula
   - Archivos CmakeLists.txt en `PascalCase`
   - Atributos y variables en `snake_case`
   - Métodos y funciones en `PascalCase`
   
- Comentarios:
   - Solo con `/*  */` y nunca en lineas de código
   - La linea de comentario es de comentario y la linea de código es de código

- Nombres de:
   - Puertos de entrada inician con `i_` y el resto en `snake_case`
   - Puertos de salida inician con `o_` y el resto en `snake_case`
   - Registros inician con `r_` y el resto en `snake_case`
   - Clocks inician con `clk_` y el resto en `snake_case`
   - Variables ***descriptivos***

- Simbolos y operadores:
  - `+`, `-`, `*`, `%`, `/`, `==`, `!=`, etc separados con espacio
  - `()`, `[]`, `->`, `::`, `;` pegados al texto que los precede

- Inicialización de variables:
  - Siempre que se pueda con `{ }` y separados con espacio del texto que los precede

- `if`, `for`, `while`, etc:
  - Siempre con `{}`, aún cuando el cuerpo tenga una línea
  - Las llaves van en lineas independientes (no en la linea del `if`, `for`, `while`, etc)

- Iteradores e indexación:
  - Para iteradores utilizar `it`
  - Para indices siempre usar el tipo `size_t` inicializado con `{ }` (`size_t i { 0 }`)
  - Para indices utilizar `i`, `j`, `k`

- Números complejos:
  - La variable imaginaria con `j` (evitar `i`)

- Librerías:
  - Usar librerías estándar dentro de lo posible
  - Intentar utilizar librerías ya instaladas en el contenedor del proyecto

# Ejercicios

- [TP1 - Registro de desplazamiento](tp1/README.md)
- [TP2 - Registro de desplazamiento en punto fijo](tp2/README.md)
- [TP3 - Filtro FIR promediador](tp3/README.md)
- [TP4 - Filtro FIR con templates](tp4/README.md)
- [TP5 - Filtro FIR coseno realzado](tp5/README.md)
- [TP6 - Filtro FIR raiz coseno realzado](tp6/README.md)
- [TP7 - Filtro IIR](tp7/README.md)