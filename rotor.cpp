#include<iostream>
#include<fstream>
#include<cstring>
#include<string.h>
#include<cstdlib>
#include"rotor.h"
#include"errors.h"

using namespace std;

//THIS FILE DEFINES MEMBER FUNCTIONS SPECIFIC TO THE CLASS ROTOR.

Rotor::Rotor(char* config_file_name, int starting_position)
{
  ifstream in_stream;
  int number, loop_count = 0;
  bool already_mapped[26] = {};

  relative_position = starting_position;
  in_stream.open(config_file_name);

  if (in_stream.fail()) //check for failure opening file
  {
    cerr << "Error: file " << config_file_name << " could not be opened.";
    cerr << endl;
    throw(ERROR_OPENING_CONFIGURATION_FILE);
  }

  in_stream >> number; //take in first integer

  for (int i = 0; i < 26; i++) //loop to construct data member mapping array
  {
    if (in_stream.eof()) //check if not enough parameters
    {
      cerr << "Not all inputs mapped in rotor file: " << config_file_name;
      cerr << endl;
      throw(INVALID_ROTOR_MAPPING);
    }

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for failbit
    {
      cerr << "Non-numeric character for mapping in rotor file ";
      cerr << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    if (number > 25 || number < 0) //check for out of range parameter
    {
      cerr << "Parameter out of range in rotor file " << config_file_name;
      cerr << endl;
      throw(INVALID_INDEX);
    }

    if (already_mapped[number] == 1) //check if contact already mapped
    {
      cerr << "Invalid mapping of input " << i << " to output " << number;
      cerr << " (output " << number << " is already mapped to from input ";
      cerr << endl;

      for (int j = 0; j < 26; j++) //find contact already mapped to
      {
	if (mapping[j] == number)
	{
	  cerr << j << ") in rotor file " << config_file_name << endl;
	  throw(INVALID_ROTOR_MAPPING);
	}
      }
    }

    mapping[i] = number; //no exceptions: set contact mapping
    already_mapped[number] = 1; //set contact to already mapped
    in_stream >> number;
  }


  while (!in_stream.eof()) //loop to set data member notches array
  {
    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for failbit
    {
      cerr << "Non-numeric character for mapping in rotor file ";
      cerr << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    if (loop_count > 26) //check for too many parameters
    {
      cerr << "Too many input parameters for rotor file " << config_file_name;
      cerr << endl;
      throw(TOO_MANY_PARAMETERS);
    }

    notches[number] = 1; //set notch position to true

    in_stream >> number;
  }

  in_stream.close();
}


void Rotor::getOutputForwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  relative_input = relative_input%26; //guard against rotor overflow
  relative_output = mapping[relative_input];
  relative_output -= relative_position;
  relative_output = (relative_output + 26)%26; //guard against rotor underflow
  input = relative_output;
}

void Rotor::getOutputBackwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  relative_input = relative_input%26; //guard against rotor overflow
  for (int i = 0; i < 26; i++)
  {
    if (mapping[i] == relative_input)
    {
      relative_output = i - relative_position;
      relative_output = (26 + relative_output)%26; //guard against rotor underflow
      input = relative_output;
      return;
    }
  }
}

bool Rotor::turnRotor()
{
  relative_position++;
  relative_position = relative_position%26; //guard against rotor overflow
  if (notches[relative_position] == 1)
    return true;
  return false;
}
