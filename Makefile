SRC=main.cpp ymp.cpp miscCalc.cpp
OBJ=$(SRC:.cpp=.o)

TARGET=main
CXXFLAGS=-Wall -Wextra -O3 -std=c++17 -DNDEBUG
LDFLAGS=-lgmpxx -lgmp

BENCH=ymp_benchmark

all: $(TARGET)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $?

.PHONY: clean benchmark

clean:
	$(RM) -rf $(OBJ) $(TARGET) $(BENCH)

benchmark: benchmark.o ymp.o miscCalc.o
	$(CXX) -o ymp_benchmark $? $(LDFLAGS)
	./ymp_benchmark
