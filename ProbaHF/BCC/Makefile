SRC = ..\*.cpp 
INCLUDES = ..\*.h 
LIBS = 
INCLUDEPATH = C:\BC31\include;..\;
LIBSPATH = C:\BC31\lib;
TARGET = ProbaHF.exe
$(TARGET):	$(SRC) $(INCLUDES) configuration.xmi
	bcc.exe -mh -v -e$(TARGET) -I$(INCLUDEPATH) -L$(LIBSPATH) $(SRC) $(LIBS)
all:	$(TARGET)
clean:
	-del *.obj
	-del $(TARGET)
	-del *.asm
configuration.xmi:	
	configuration.xmi