#include<iostream>
#include<fstream>
#include<cstring>
#include<string.h>
#include<cstdlib>
#include"enigma.h"
#include"errors.h"

using namespace std;

//THIS FILE DEFINES MEMBER FUNCTIONS SPECIFIC TO THE CLASS ENIGMA.

Enigma::Enigma(char** argv, int argc) :
plugboard(argv, argc, getFileTypeIndex(argv, argc, ".pb")),
reflector(argv, argc, getFileTypeIndex(argv, argc, ".rf"))
{
  int starting_pos;
  int index_first_rotor = getFileTypeIndex(argv, argc, ".rot");
  int index_rotor_position = getFileTypeIndex(argv, argc, ".pos");
  ifstream in_stream;

  number_of_rotors = getNumberOfRotors(argv, argc);
  rotors_array = new Rotor*[number_of_rotors];

  //initialise rotor_array pointers to null incase deconstructor is called
  //early in case of exception
  for (int i = 0; i < number_of_rotors; i++)
  {
    rotors_array[i] = nullptr;
  }
  
  //Rest of function creates each rotor whilst checking for errors in rotor
  //config files
  if (index_rotor_position) //check if .pos file exists
  {
    if (!index_first_rotor) //check if .rot file does not exist
    {
      cerr << "Position files exists but no rotor files." << endl;
      throw(INSUFFICIENT_NUMBER_OF_PARAMETERS);
    }

    in_stream.open(argv[index_rotor_position]); //open .pos file

    if (in_stream.fail()) //check for failure opening file
    {
      cerr << "Error: file " << argv[index_rotor_position];
      cerr << " could not be opened." << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }

    for (int i = 0; i < number_of_rotors + 1; i++) //for each rotor set position
    {
      in_stream >> starting_pos;

      if (i == number_of_rotors) //if true: all rotor positions set already
      {
	if (!in_stream.eof()) //eof flag has not been set
	{
	  if (in_stream.bad()) //check for badbit
	  {
	    cerr << "Error reading file " << argv[index_rotor_position] << endl;
	    throw(ERROR_OPENING_CONFIGURATION_FILE);
	  }
	  if (in_stream.fail()) //check for failbit
	  {
	    cerr << "Non-numeric character in rotor positions file ";
      cerr << argv[index_rotor_position];
	    cerr << endl;
	    throw(NON_NUMERIC_CHARACTER);
	  }
	  cerr << "Too many parameters in rotor position file ";
    cerr << argv[index_rotor_position] << ", only " << number_of_rotors;
    cerr << " rotors exist." << endl;
	  throw(TOO_MANY_PARAMETERS);
	}
	in_stream.close();
	return; //return if all rotor positions set and eof flag has been set
      }

      if (in_stream.eof()) //if not enough parameters
      {
	cerr << "No starting position for rotor " << i << " in rotor position file: ";
	cerr << argv[index_rotor_position] << endl;
	throw(NO_ROTOR_STARTING_POSITION);
      }

      if (in_stream.bad()) //check for badbit
      {
	cerr << "Error reading file " << argv[index_rotor_position] << endl;
	throw(ERROR_OPENING_CONFIGURATION_FILE);
      }
      if (in_stream.fail()) //check for failbit
      {
	cerr << "Non-numeric character in rotor positions file ";
  cerr << argv[index_rotor_position] << endl;
	throw(NON_NUMERIC_CHARACTER);
      }
      //if no error from file: create a rotor on stack and pass stating_pos
      //to Rotor constructor
      rotors_array[i] = new Rotor(argv[i + index_first_rotor], starting_pos);
    }
  }
}

/*Enigma class destructor*/

Enigma::~Enigma()
{
  for (int i = 0; i < number_of_rotors; i++)
  {
    delete rotors_array[i]; //delete each rotor
  }
  delete [] rotors_array; //delete array of pointers
}


void Enigma::runEnigma(int argc)
{
  char letter;

  cin >> ws;
  cin >> letter;

  while(!cin.fail()) //while characters remain
  {
    if (letter < 65 || letter > 90) //check for invalid input character
    {
      cerr << letter;
      cerr << " is not a valid input character (input characters must be ";
      cerr << "upper case letters A-Z)!" << endl;
      throw(INVALID_INPUT_CHARACTER);
    }

    //turn far right rotor anticlockwise once then cascade turning to the left
    //if any notch aligns
    for (int j = 0; j < number_of_rotors; j++)
    {
      if (!rotors_array[number_of_rotors - 1 - j]->turnRotor())
	break;
    }

    getOutput(letter, argc); //encript/decrypt letter
    cout << letter;
    cin >> letter;
  }
}

int Enigma::getNumberOfRotors(char** argv, int argc)
{
  int count = 0;
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], ".rot"))
      count++;
  }
  return count;
}

  int Enigma::getFileTypeIndex(char** argv, int argc, const char* extension)
  {
    for (int i = 0; i < argc; i++)
    {
      if (strstr(argv[i], extension))
        return i;
    }
    return 0;
  }


void Enigma::getOutput(char& letter, int argc)
{
  static const int ascii_shift = 'A';
  int number = letter - ascii_shift;

  plugboard.getOutput(number); //pass letter forward through plugboard

  for (int i = 0; i < number_of_rotors; i++)  //pass letter forward through each rotor
  {
    rotors_array[number_of_rotors-1-i]->getOutputForwards(number);
  }

  reflector.getOutput(number); //pass letter through reflector

  for (int i = 0; i < number_of_rotors; i++) //pass letter backward through each rotor
  {
    rotors_array[i]->getOutputBackwards(number);
  }

  plugboard.getOutput(number); //pass letter backward through plugboard

  letter = number + ascii_shift;
}
