SRC = ..\*.cpp 
INCLUDES = 
LIBS = 
INCLUDEPATH = C:\bc31\include;
LIBSPATH = C:\bc31\lib;
TARGET = ProbaTH.exe
$(TARGET):	$(SRC) $(INCLUDES) configuration.xmi
	bcc.exe -mh -v -e$(TARGET) -I$(INCLUDEPATH) -L$(LIBSPATH) $(SRC) $(LIBS)
all:	$(TARGET)
clean:
	-del *.obj
	-del $(TARGET)
	-del *.asm
configuration.xmi:	
	configuration.xmi