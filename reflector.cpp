#include<iostream>
#include<fstream>
#include<cstring>
#include<string.h>
#include<cstdlib>
#include"reflector.h"
#include"errors.h"

using namespace std;

//THIS FILE DEFINES MEMBER FUNCTIONS SPECIFIC TO THE CLASS REFLECTOR.

Reflector::Reflector(char** argv, int argc, int index_reflector)
{
  char* config_file_name = argv[index_reflector];
  ifstream in_stream;
  int pair[2]; //pair of contacts attempting to connect
  int loop_count = 0;
  bool already_paired[26] = {}; //keep track of contacts already paired

  if (!index_reflector) //check if .rf file does not exist
  {
    cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>* ";
    cerr << "rotor-positions)?" << endl;
    throw(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }

  in_stream.open(config_file_name); //open file .rf file

  if (in_stream.fail()) //check for error opening file
  {
    cerr << "Error: file " << config_file_name << " could not be opened.";
    cerr << endl;
    throw(ERROR_OPENING_CONFIGURATION_FILE);
  }

  while(true)
  {
    loop_count++;

    in_stream >> pair[0]; //take in first integer

    if (in_stream.eof()) //check for end of file
    {
      if (loop_count != 14) //check if not enough parameters in file
      {
	cerr << "Insufficient number of mappings in reflector file: ";
  cerr << config_file_name << endl;
	throw(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
      }
      break; //break loop if finished pairing contacts
    }

    if (loop_count == 14) //check for too many parameters in file
    {
      cerr << "Incorrect (odd) number of parameters in reflector file ";
      cerr << config_file_name << endl;
      throw(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }

    if (in_stream.fail()) //check for non-numeric character
    {
      cerr << "Non-numeric character in reflector file " << config_file_name;
      cerr << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    in_stream >> pair[1]; //take in second integer

    if (in_stream.eof()) //check for odd number of parameters
    {
      cerr << "Incorrect (odd) number of parameters in reflector file ";
      cerr << config_file_name << endl;
      throw(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }

    if (in_stream.fail()) //check for non-numeric character
    {
      cerr << "Non-numeric character in reflector file " << config_file_name;
      cerr << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    if (pair[0] == pair[1]) //check attempted mapping of contact to itself
    {
      cerr << "Attempted connection of contact to itself in plugboard file ";
      cerr << config_file_name << endl;
      throw(INVALID_REFLECTOR_MAPPING);
    }

    //check for out of range character
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25)
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name;
      cerr << endl;
      throw(INVALID_INDEX);
    }

    //check if previously paired
    if (already_paired[pair[0]] == 1 || already_paired[pair[1]] == 1)
    {
      cerr << "Attempted connection of a contact with more than one other ";
      cerr << "contact in file " << config_file_name << endl;
      throw(INVALID_REFLECTOR_MAPPING);
    }

    mapping[pair[0]] = pair[1]; //no exceptions: pair contacts
    mapping[pair[1]] = pair[0];
    already_paired[pair[0]] = 1; //set contacts to already paired
    already_paired[pair[1]] = 1;
  }
  in_stream.close();
}


void Reflector::getOutput(int& input)
{
  input = mapping[input];
  return;
}

int Reflector::getFileIndex(char** argv, int argc)
{
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], ".rf"))
      return i;
  }
  return 0;
}
