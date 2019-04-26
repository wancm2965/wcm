TARGET		= ../lib/libvidecmp4.so
OBJPATH 	= ./
SOURCES		= $(wildcard \
			./libplatform/io/File.cpp \
			./libplatform/io/File_posix.cpp \
			./libplatform/io/FileSystem.cpp \
			./libplatform/io/FileSystem_posix.cpp \
			./libplatform/number/random_posix.cpp \
			./libplatform/process/process_posix.cpp \
			./libplatform/prog/option.cpp \
			./libplatform/sys/error.cpp \
			./libplatform/time/time.cpp \
			./libplatform/time/time.cpp \
			./libplatform/time/time_posix.cpp \
			./libplatform/time/MP4V2_time.cpp \
			./libutil/*.cpp \
			./src/bmff/*.cpp \
			./src/itmf/*.cpp \
			./src/qtff/*.cpp \
			./src/*.cpp \
			./VIDECMP4/*.cpp )
			
CPPOBJS := $(SOURCES:.cpp=.o)  
#OBJS    	= $(patsubst %.cpp,%.o,$(SOURCES))
FULLOBJS 	= $(patsubst %.cpp,$(OBJPATH)/%.o,$(SOURCES))
INCPATH		= -I. -I./include -I../include -I../
LIBPATH		= 
LFLAGS		= -w -shared
CFLAGS		= -w -c
CXX     	= g++

all:$(TARGET)

$(TARGET):$(CPPOBJS)
	$(CXX) $(LFLAGS) $(FULLOBJS) -o $(TARGET) $(LIBPATH)

$(CPPOBJS):$(SOURCES)
	$(CXX) $(CFLAGS) $*.cpp -o $(OBJPATH)/$@ $(INCPATH)

#$(CPPOBJS) : %.o: %.cpp  
#	$(CPP) -c $< -o $@ $(INCS) $(DEFINC) $(CPPFLAGS)  
	
clean:            
	-rm $(TARGET) $(CPPOBJS)  
	
#clean:
#	rm -f $(OBJPATH)/*.o
#	rm -f $(TARGET)