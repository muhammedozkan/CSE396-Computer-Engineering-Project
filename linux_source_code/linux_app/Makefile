CC              =  g++
CFLAGS          = -Wall -g -O0
LIBS            = -lopencv_core         \
                  -lopencv_imgproc  	\
		   -lopencv_highgui	\
                  -lopencv_ml           \
                  -lopencv_video        \
                  -lopencv_features2d   \
                  -lopencv_calib3d      \
                  -lopencv_objdetect    \
                  -lopencv_flann\
                  -lopencv_imgcodecs\
		   -I/usr/include/opencv4  \
                  -L/usr/local/lib -lopencv_shape -lopencv_videoio

SOURCES         = ballbouncer.cpp 	   \
					camera.cpp	   \
					pid.cpp		   \
					serialib.cpp

OBJECTS         = $(SOURCES:.c=.o)
EXECUTABLE      = ballbouncer


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo "====================================================="
	@echo "Building ballbouncer"
	@echo ""

	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

	@echo ""
	@echo "Success"
	@echo "====================================================="

%.o: %.c
	$(CC) -c -MD $(CFLAGS) -o $@ $<

-include $(wildcard *.d)

valgrind:
	valgrind ./$(EXECUTABLE)

make_clean: all clean

clean:
	rm -f *.o *.d

clean_all:
	rm -f *.o *.d $(EXECUTABLE)



.PHONY: all clean

