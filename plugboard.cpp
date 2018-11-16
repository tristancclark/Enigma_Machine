#include<iostream>
#include<fstream>
#include<cstring>
#include<string.h>
#include<cstdlib>
#include"plugboard.h"
#include"errors.h"

using namespace std;

//THIS FILE DEFINES MEMBER FUNCTIONS SPECIFIC TO THE CLASS PLUGBOARD.

Plugboard::Plugboard(char** argv, int argc, int index_plugboard)
{
  ifstream in_stream;
  int pair[2]; //pair of contacts attemping to connect
  int loop_count = 0;
  bool already_paired[26] = {}; //to keep track of contacts already connected
  char* config_file_name = argv[index_plugboard];

  if (!index_plugboard) //check if .pb file does not exist
  {
    cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>* ";
    cerr << "rotor-positions)?" << endl;
    throw(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }

  for (int i = 0; i < 26; i++) //initialise as one to one mapping
    mapping[i] = i;

  in_stream.open(config_file_name);

  if (in_stream.fail()) //check for failure opening file
  {
    cerr << "Error: file " << config_file_name << " could not be opened.";
    cerr << endl;
    throw(ERROR_OPENING_CONFIGURATION_FILE);
  }

  while(true)
  {
    loop_count++;

    in_stream >> pair[0]; //take in first integer

    if (in_stream.eof())
      break;

    if (loop_count == 14) //check for too many parameters
    {
      cerr << "Incorrect number of parameters in plugboard file ";
      cerr << config_file_name << endl;
      throw(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for failbit
    {
      cerr << "Non-numeric character in plugboard file " << config_file_name;
      cerr << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    in_stream >> pair[1]; //take in second digit

    if (in_stream.eof()) //check for odd number of parameters
    {
      cerr << "Incorrect number of parameters in plugboard file ";
      cerr << config_file_name << endl;
      throw(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for invalid input
    {
      cerr << "Non-numeric character in plugboard file " << config_file_name;
      cerr << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    if (pair[0] == pair[1]) //check for attempted mapping of contact to itself
    {
      cerr << "Attempted connection of contact to itself in plugboard file ";
      cerr << config_file_name << endl;
      throw(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }

    //check for out of range parameter
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25)
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name;
      cerr << endl;
      throw(INVALID_INDEX);
    }

    //check if previously swapped
    if (already_paired[pair[0]] == 1 || already_paired[pair[1]] == 1)
    {
      cerr << "Attempted connection of a contact with more than one other ";
      cerr << "contact in file " << config_file_name << endl;
      throw(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }

    mapping[pair[0]] = pair[1]; //if no exceptions: pair contacts
    mapping[pair[1]] = pair[0];
    already_paired[pair[0]] = 1; //set contacts to already paired
    already_paired[pair[1]] = 1;
  }
  in_stream.close();
}

void Plugboard::getOutput(int& input)
{
  input = mapping[input];
}
