
# // g++ xxx.m.cpp -lmysqlcppconn8 -std=c++20  // fedora41, mysql-connector-c++ 9, use -lmysqlcppconnx ...


# make
# ./asd.out
# make clean


CXX = g++
CXXFLAGS = -std=c++20
TARGET = asd.out
SOURCES = 01_server_epoll.cpp
MYFLAGS = -lev

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(MYFLAGS) -o $(TARGET) $(SOURCES)


clean:
	rm -f $(TARGET)

.PHONY: clean

