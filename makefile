OBJ = enigma.o	plugboard.o	reflector.o	rotor.o	main.o
EXE = enigma
CXX = g++
CXXFLAGS = -Wall	-g	-MMD

$(EXE): $(OBJ)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^

-include $(OBJ:.o=.d)

clean:
	rm -f *.o enigma *.d *.h.gch
