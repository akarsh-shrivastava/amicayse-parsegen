TARGET    =     libparsegen.so

CX        =     g++
LD        =     g++

INCLUDES  =     include
CXOBJ     =     .obj
CXSRC     =     src

HEADERS   =     $(INCLUDES)/*.h
CXSOURCE  =     $(CXSRC)/*.cpp
CXOBJECT  =     $(CXOBJ)/parsegen.o \

CXFLAGS    =    -c -I$(INCLUDES) -fPIC -O3 --std=c++17
LDFLAGS    =    -shared

INSTALL_INCLUDE= "$(HOME)/.amicayse/include/amicayse"
INSTALL_LIB    = "$(HOME)/.amicayse/lib"

$(TARGET): $(CXOBJECT)
	$(LD) $^ $(LDFLAGS) -o $@ 

$(CXOBJ)/%.o: $(CXSRC)/%.cpp $(HEADERS)
	$(CX) $(CXFLAGS) $< -o $@

install: $(TARGET)
	cp $(TARGET) $(INSTALL_LIB)
	cp $(HEADERS) $(INSTALL_INCLUDE)