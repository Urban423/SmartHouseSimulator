Target = Strategy
build = ./build/
platform = Windows
include ../../makefilePatterns/${platform}.mk

AppDir = ../../App/
MathDir = ../../Math/
IOSystemDir = ../../IOSystem/
WindowsDir = ../../IOSystem/Windows/
GraphicsEngineDir = ../../GraphicsEngine/
LogicDir = ../../Logic/


Libs += -L../../libs
Libs += -l App
Libs += -l Logic
Libs += -l IOSystem
Libs += -l GraphicsEngine
Libs += -l Math
Libs += -lz

IFlags += -I ${AppDir}
IFlags += -I ${MathDir}
IFlags += -I ${IOSystemDir}
IFlags += -I ${WindowsDir}
IFlags += -I ${GraphicsEngineDir}
IFlags += -I ${LogicDir}


LFlags += -lgdi32
LFlags += -lopengl32

build: buildApp
	$(build)$(Target)

include ../../makefilePatterns/pattern.mk