all: libs cubescpp

# directorios
HDIR    = src
CPPDIR  = src
ODIR    = obj

MODULOS = window render
#MODULOS = asignatura asistenciavivo clase claseestudiante ctrlasignatura ctrlclase ctrlmensaje 
#MODULOS += ctrlreproduccion ctrlsubscripcion ctrlusuario dicta docente estudiante handlerasignaturas 
#MODULOS += handlerusuarios mensaje monitoreo notificacion practico reloj srespuesta teorico usuario
#MODULOS += dt/dtasignatura dt/dtasignaturaext dt/dtclase dt/dtdocente dt/dtestudiante dt/dtfecha 
#MODULOS += dt/dtmensaje dt/dtnotificacion dt/dtpromasistencia
#MODULOS += dt/dttiempoasignatura dt/dtusuario fabrica dt/dtpreview 

# IMPORTANTE hay que poner como "modulos" todos los archivos de src a compilar

# cadena de archivos, con directorio y extensión
HS   = $(MODULOS:%=$(HDIR)/%.hpp)
CPPS = $(MODULOS:%=$(CPPDIR)/%.cpp)
OS   = $(MODULOS:%=$(ODIR)/%.o)

MAIN=main
BINARY=cubescpp

# compilador
CC = g++
# opciones de compilación
CCFLAGS = -Wall -Werror -O3 -I$(HDIR)
# -DNDEBUG
# se agrega esta opción para que las llamadas a assert no hagan nada.
LIBRARIES = -lglfw
LDFLAGS = $(LIBRARIES) $(ODIR)/glad.o

libs:
	cd lib/glad && $(CC) -o ../../$(ODIR)/glad.o -Iinclude -c src/glad.c

run: all
	./cubescpp

$(ODIR)/$(MAIN).o:$(CPPDIR)/$(MAIN).cpp
	$(CC) $(CCFLAGS) -c $< -o $@

# cada .o depende de su .cpp
# $@ se expande para tranformarse en el objetivo
# $< se expande para tranformarse en la primera dependencia	
$(ODIR)/%.o: $(CPPDIR)/%.cpp $(HDIR)/%.hpp
	$(CC) $(CCFLAGS) -c $< -o $@

# $^ se expande para tranformarse en todas las dependencias
$(BINARY): $(ODIR)/$(MAIN).o $(OS)
	$(CC) $(CCFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	@rm obj/*.o cubescpp
