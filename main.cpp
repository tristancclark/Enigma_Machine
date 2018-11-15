#include<iostream>
#include<fstream>
#include<cstring>
#include"enigma.h"
#include"errors.h"

using namespace std;

int main(int argc, char** argv) {

  try
  {
    Enigma enigma(argv, argc); //set up enigma machine
 
    enigma.runEnigma(argc); //run enigma machine
  }
  
  catch(int error_code)
  {
    return error_code; //check for errors
  }
  
  return NO_ERROR;
}
