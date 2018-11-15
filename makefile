OBJ = enigma.o	main.o
EXE = enigma
CXX = g++
CXXFLAGS = -Wall	-g	-MMD

$(EXE): $(OBJ)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^

-include $(OBJ:.o=.d)

clean:
	rm -f *.o enigma *.d

