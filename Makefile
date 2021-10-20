
.PHONY: all clean

PROJECT_NAME ?= planet
BUILD_MODE ?= DEBUG
RELEASE_PATH ?= src
CC ?= gcc

# No uname.exe on MinGW!, but OS=Windows_NT on Windows!
# ifeq ($(UNAME),Msys) -> Windows
ifeq ($(OS),Windows_NT)
    PLATFORM_OS=WINDOWS
else
    PLATFORM_OS=LINUX
endif

# SETUP FLAGS
CFLAGS += -Wall -std=c99 -Wno-missing-braces -Wno-unused-parameter 
#CFLAGS += -Wno-unused-function

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -s -O1
endif

ifeq ($(PLATFORM_OS),WINDOWS)
    # resource file contains windows executable icon and properties
    # -Wl,--subsystem,windows hides the console window
    # CFLAGS += -Wl,--subsystem,windows
else
    CFLAGS += -D_DEFAULT_SOURCE
endif

INCLUDE_PATHS = -I./external -I./src
LDFLAGS = -Lexternal
# // KANSKE INTE STÄMMER
LDLIBS = external/libcglm.a 

# Define any libraries required on linking
# if you want to link libraries (libname.so or libname.a), use the -lname
ifeq ($(PLATFORM_OS),WINDOWS)
    # Libraries for Windows desktop compilation
    # NOTE: WinMM library required to set high-res timer resolution
    LDLIBS +=  external/libglew32.a
    LDLIBS += -lglfw3 -lopengl32 -lgdi32 -lwinmm -lm
    # Required for physac examples
    #LDLIBS += -static -lpthread
else
    # Libraries for Debian GNU/Linux desktop compiling
    # NOTE: Required packages: libegl1-mesa-dev
    LDLIBS += -lglfw -lGL -lm -lpthread -ldl -lrt
    
    # On X11 requires also below libraries
    LDLIBS += -lX11
    # NOTE: It seems additional libraries are not required any more, latest GLFW just dlopen them
    #LDLIBS += -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor
    
endif

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SRC_DIR = ./src
OBJ_DIR = ./obj

SRC = $(call rwildcard, *.c)
OBJS ?= main.c ./external/*.c ./external/GLAD/glad.c ./src/*.c ./src/*/*.c
#OBJS += $(SRC:$(SRC_DIR)%.c=$(OBJ_DIR)%.o)

all:
	make $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) -o $(PROJECT_NAME) $(EXT) $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(BUILD_MODE) -DLOG_USE_COLOR
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(BUILD_MODE) -DLOG_USE_COLOR

clean:
    ifeq ($(PLATFORM_OS),WINDOWS)
		rm *.o *.exe /s
    else
		find -type f -executable | xargs file -i | grep -E 'x-object|x-archive|x-sharedlib|x-executable' | rev | cut -d ':' -f 2- | rev | xargs rm -fv
    endif
