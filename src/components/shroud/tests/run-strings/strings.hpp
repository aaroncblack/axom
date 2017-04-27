//
// strings.hpp - wrapped routines
//

#ifndef STRINGS_HPP
#define STRINGS_HPP

#include <string>

void passChar(char status);
char returnChar();

void passCharPtr(char * dest, const char *src);

const char * getChar1();
const char * getChar2();
const char * getChar3();

const std::string& getString1();
const std::string& getString2();
const std::string& getString3();
const std::string& getString2_empty();

void acceptName_instance(std::string arg1);

void acceptStringConstReference(const std::string & arg1);

void acceptStringReference(std::string & arg1);

void acceptStringPointer(std::string * arg1);

void explicit1(char * name);
void explicit2(char * name);


#endif // STRINGS_HPP
