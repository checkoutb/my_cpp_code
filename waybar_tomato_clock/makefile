
# make
# ./asd.out
# make clean

CXX = g++
CXXFLAGS = -std=c++20
TARGET = qwe.out
RELEASE_TARGET = waybar_tomato_clock.out
SOURCES = common.h waybar_tomato_clock.m.cpp
MYFLAGS =

CLICKER_SOURCES = common.h waybar_tomato_clock_clicker.m.cpp
CLICKER_TARGET = zxc.out
RELEASE_CLICKER_TARGET = waybar_tomato_clock_clicker.out


$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(MYFLAGS) -o $(TARGET) $(SOURCES)

r:  # release
	$(CXX) -o2 $(CXXFLAGS) $(MYFLAGS) -o $(RELEASE_TARGET) $(SOURCES)
	$(CXX) -o2 $(CXXFLAGS) $(MYFLAGS) -o $(RELEASE_CLICKER_TARGET) $(CLICKER_SOURCES)

clicker:
	$(CXX) $(CXXFLAGS) $(MYFLAGS) -o $(CLICKER_TARGET) $(CLICKER_SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean

