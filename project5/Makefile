# Source Files
SRCS = project5.cpp CS3113/cs3113.cpp CS3113/Entity.cpp CS3113/Map.cpp CS3113/Scene.cpp CS3113/ShaderProgram.cpp

# Target
TARGET = project5_app

# OS detection
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Default values
CXX = g++
CXXFLAGS = -std=c++11

ifeq ($(UNAME_S), Darwin)
    CXXFLAGS += -arch $(UNAME_M)
    LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    EXEC = ./$(TARGET)
else ifneq (,$(findstring MINGW,$(UNAME_S)))
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = $(TARGET)
else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
endif

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TARGET).exe" ]; then rm -f $(TARGET).exe; fi

run: $(TARGET)
	$(EXEC)
