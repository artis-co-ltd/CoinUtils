// Copyright (C) 2006, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#include "CoinUtilsConfig.h"

#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <algorithm>
#ifdef COINUTILS_HAS_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "CoinPragma.hpp"
#include "CoinParam.hpp"
#include "CoinFinite.hpp"

/*
  Constructors and destructors

  There's a generic constructor and one for integer, double, keyword, string,
  and action parameters.
*/

/*
  Default constructor.
*/
CoinParam::CoinParam()
  : type_(paramInvalid)
  , name_()
  , lengthName_(0)
  , lengthMatch_(0)
  , lowerDblValue_(-COIN_DBL_MAX)
  , upperDblValue_(COIN_DBL_MAX)
  , dblValue_(0.0)
  , dblDefaultValue_(0.0)
  , lowerIntValue_(-COIN_INT_MAX)
  , upperIntValue_(COIN_INT_MAX)
  , intValue_(0)
  , intDefaultValue_(0)
  , strValue_("")
  , strDefaultValue_("")
  , definedKwds_()
  , currentMode_(-1)
  , defaultMode_(-1)
  , currentKwd_("")
  , defaultKwd_("")
  , pushFunc_(0)
  , pullFunc_(0)
  , shortHelp_()
  , longHelp_()
  , display_(displayPriorityNone)
{
  /* Nothing to be done here */
}

/*
  Constructor for double parameter
*/
CoinParam::CoinParam(std::string name, std::string help,
                     double lower, double upper, 
                     double defaultValue, std::string longHelp,
                     CoinDisplayPriority displayPriority)
  : type_(paramDbl)
  , name_(name)
  , lengthName_(0)
  , lengthMatch_(0)
  , lowerDblValue_(lower)
  , upperDblValue_(upper)
  , dblValue_(defaultValue)
  , dblDefaultValue_(defaultValue)
  , lowerIntValue_(0)
  , upperIntValue_(0)
  , intValue_(0)
  , intDefaultValue_(0)
  , strValue_("")
  , strDefaultValue_("")
  , definedKwds_()
  , currentMode_(-1)
  , defaultMode_(-1)
  , currentKwd_("")
  , defaultKwd_("")
  , pushFunc_(0)
  , pullFunc_(0)
  , shortHelp_(help)
  , longHelp_(longHelp)
  , display_(displayPriority)
{
  processName();
}

/*
  Constructor for integer parameter
*/
CoinParam::CoinParam(std::string name, std::string help,
                     int lower, int upper, 
                     int defaultValue, std::string longHelp,
                     CoinDisplayPriority displayPriority)
  : type_(paramInt)
  , name_(name)
  , lengthName_(0)
  , lengthMatch_(0)
  , lowerDblValue_(0.0)
  , upperDblValue_(0.0)
  , dblValue_(0.0)
  , dblDefaultValue_(0.0)
  , lowerIntValue_(lower)
  , upperIntValue_(upper)
  , intValue_(defaultValue)
  , intDefaultValue_(defaultValue)
  , strValue_("")
  , strDefaultValue_("")
  , definedKwds_()
  , currentMode_(-1)
  , defaultMode_(-1)
  , currentKwd_("")
  , defaultKwd_("")
  , pushFunc_(0)
  , pullFunc_(0)
  , shortHelp_(help)
  , longHelp_(longHelp)
  , display_(displayPriority)
{
  processName();
}

/*
  Constructor for keyword parameter.
*/
CoinParam::CoinParam(std::string name, std::string help,
                     std::string defaultKwd, int defaultMode,
                     std::string longHelp, CoinDisplayPriority displayPriority)
  : type_(paramKwd)
  , name_(name)
  , lengthName_(0)
  , lengthMatch_(0)
  , lowerDblValue_(0.0)
  , upperDblValue_(0.0)
  , dblValue_(0.0)
  , dblDefaultValue_(0.0)
  , lowerIntValue_(0)
  , upperIntValue_(0)
  , intValue_(0)
  , intDefaultValue_(0)
  , strValue_("")
  , strDefaultValue_("")
  , definedKwds_()
  , currentMode_(defaultMode)
  , defaultMode_(defaultMode)
  , currentKwd_(defaultKwd)
  , defaultKwd_(defaultKwd)
  , pushFunc_(0)
  , pullFunc_(0)
  , shortHelp_(help)
  , longHelp_(longHelp)
  , display_(displayPriority)
{
  processName();
  definedKwds_[defaultKwd] = defaultMode;
}

/*
  Constructor for string parameter.
*/
CoinParam::CoinParam(std::string name, std::string help,
                     std::string defaultValue, std::string longHelp,
                     CoinDisplayPriority displayPriority)
  : type_(paramStr)
  , name_(name)
  , lengthName_(0)
  , lengthMatch_(0)
  , lowerDblValue_(0.0)
  , upperDblValue_(0.0)
  , dblValue_(0.0)
  , dblDefaultValue_(0.0)
  , lowerIntValue_(0)
  , upperIntValue_(0)
  , intValue_(0)
  , intDefaultValue_(0)
  , strValue_(defaultValue)
  , strDefaultValue_(defaultValue)
  , definedKwds_()
  , currentMode_(0)
  , defaultMode_(0)
  , currentKwd_("")
  , defaultKwd_("")
  , pushFunc_(0)
  , pullFunc_(0)
  , shortHelp_(help)
  , longHelp_(longHelp)
  , display_(displayPriority)
{
  processName();
}

/*
  Constructor for action parameter.
*/
CoinParam::CoinParam(std::string name, std::string help,
                     std::string longHelp, CoinDisplayPriority displayPriority)
  : type_(paramAct)
  , name_(name)
  , lengthName_(0)
  , lengthMatch_(0)
  , lowerDblValue_(0.0)
  , upperDblValue_(0.0)
  , dblValue_(0.0)
  , dblDefaultValue_(0.0)
  , lowerIntValue_(0)
  , upperIntValue_(0)
  , intValue_(0)
  , intDefaultValue_(0)
  , strValue_("")
  , strDefaultValue_("")
  , definedKwds_()
  , currentMode_(0)
  , defaultMode_(0)
  , currentKwd_("")
  , defaultKwd_("")
  , pushFunc_(0)
  , pullFunc_(0)
  , shortHelp_(help)
  , longHelp_(longHelp)
  , display_(displayPriority)
{
  processName();
}

/*
  Copy constructor.
*/
CoinParam::CoinParam(const CoinParam &orig)
  : type_(orig.type_)
  , lengthName_(orig.lengthName_)
  , lengthMatch_(orig.lengthMatch_)
  , lowerDblValue_(orig.lowerDblValue_)
  , upperDblValue_(orig.upperDblValue_)
  , dblValue_(orig.dblValue_)
  , dblDefaultValue_(orig.dblDefaultValue_)
  , lowerIntValue_(orig.lowerIntValue_)
  , upperIntValue_(orig.upperIntValue_)
  , intValue_(orig.intValue_)
  , intDefaultValue_(orig.intDefaultValue_)
  , strValue_(orig.strValue_)
  , strDefaultValue_(orig.strDefaultValue_)
  , currentMode_(orig.currentMode_)
  , defaultMode_(orig.defaultMode_)
  , currentKwd_(orig.currentKwd_)
  , defaultKwd_(orig.defaultKwd_)
  , pushFunc_(orig.pushFunc_)
  , pullFunc_(orig.pullFunc_)
  , display_(orig.display_)
{
  name_ = orig.name_;
  strValue_ = orig.strValue_;
  definedKwds_ = orig.definedKwds_;
  shortHelp_ = orig.shortHelp_;
  longHelp_ = orig.longHelp_;
}

/*
  Clone
*/

CoinParam *CoinParam::clone()
{
  return (new CoinParam(*this));
}

CoinParam &CoinParam::operator=(const CoinParam &rhs)
{
  if (this != &rhs) {
    type_ = rhs.type_;
    name_ = rhs.name_;
    lengthName_ = rhs.lengthName_;
    lengthMatch_ = rhs.lengthMatch_;
    lowerDblValue_ = rhs.lowerDblValue_;
    upperDblValue_ = rhs.upperDblValue_;
    dblValue_ = rhs.dblValue_;
    dblDefaultValue_ = rhs.dblDefaultValue_;
    lowerIntValue_ = rhs.lowerIntValue_;
    upperIntValue_ = rhs.upperIntValue_;
    intValue_ = rhs.intValue_;
    intDefaultValue_ = rhs.intDefaultValue_;
    strValue_ = rhs.strValue_;
    strDefaultValue_ = rhs.strDefaultValue_;
    definedKwds_ = rhs.definedKwds_;
    currentMode_ = rhs.currentMode_;
    defaultMode_ = rhs.defaultMode_;
    currentKwd_ = rhs.currentKwd_;
    defaultKwd_ = rhs.defaultKwd_;
    pushFunc_ = rhs.pushFunc_;
    pullFunc_ = rhs.pullFunc_;
    shortHelp_ = rhs.shortHelp_;
    longHelp_ = rhs.longHelp_;
    display_ = rhs.display_;
  }

  return *this;
}

/*
  Destructor
*/
CoinParam::~CoinParam()
{ /* Nothing more to do */
}

/*
  Methods to manipulate a CoinParam object.
*/

/* Methods to initialize a parameter that's already constructed */ 

/* Set up a double parameter */
void CoinParam::setup(std::string name, std::string help,
                      double lower, double upper, std::string longHelp,
                      CoinDisplayPriority display){
   name_ = name;
   processName();
   type_ = paramDbl;
   shortHelp_ = help;
   lowerDblValue_ = lower;
   upperDblValue_ = upper;
   longHelp_ = longHelp;
   display_ = display;
}

/* Set up an integer parameter */
void CoinParam::setup(std::string name, std::string help,
                      int lower, int upper, std::string longHelp,
                      CoinDisplayPriority display){
   name_ = name;
   processName();
   type_ = paramInt;
   shortHelp_ = help;
   lowerIntValue_ = lower;
   upperIntValue_ = upper;
   longHelp_ = longHelp;
   display_ = display;
}

/* Set up a keyword, string, or action parameter */
void CoinParam::setup(std::string name, std::string help,
                      std::string longHelp, CoinDisplayPriority display){
   name_ = name;
   processName();
   shortHelp_ = help;
   longHelp_ = longHelp;
   display_ = display;
}

/*
  Process the parameter name.
  
  Process the name for efficient matching: determine if an `!' is present. If
  so, locate and record the position and remove the `!'.
*/

void CoinParam::processName()

{
  std::string::size_type shriekPos = name_.find('!');
  lengthName_ = name_.length();
  if (shriekPos == std::string::npos) {
    lengthMatch_ = lengthName_;
  } else {
    lengthMatch_ = shriekPos;
    name_ = name_.substr(0, shriekPos) + name_.substr(shriekPos + 1);
    lengthName_--;
  }

  return;
}

/*
  Check an input string to see if it matches the parameter name. The whole
  input string must match, and the length of the match must exceed the
  minimum match length. A match is impossible if the string is longer than
  the name.

  Returns: 0 for no match, 1 for a successful match, 2 if the match is short
*/
int CoinParam::matches(std::string input) const
{
  size_t inputLen = input.length();
  if (inputLen <= lengthName_) {
    size_t i;
    for (i = 0; i < inputLen; i++) {
      if (tolower(name_[i]) != tolower(input[i]))
        break;
    }
    if (i < inputLen) {
      return (0);
    } else if (i >= lengthMatch_) {
      return (1);
    } else {
      return (2);
    }
  }

  return (0);
}

/*
  Return the parameter name, formatted to indicate how it'll be matched.
  E.g., some!Name will come back as some(Name).
*/
std::string CoinParam::matchName() const
{
  if (lengthMatch_ == lengthName_) {
    return name_;
  } else {
    return name_.substr(0, lengthMatch_) + "(" + name_.substr(lengthMatch_) + ")";
  }
}

/*
  Return the length of the formatted paramter name for printing.
*/
int CoinParam::lengthMatchName() const {
  if (lengthName_ == lengthMatch_)
    return lengthName_;
  else
    return lengthName_ + 2;
}

/*
  Print the long help message and a message about appropriate values.
*/
std::string CoinParam::printLongHelp() const
{
  std::ostringstream buffer;
  if (longHelp_ != "") {
    CoinParamUtils::printIt(longHelp_.c_str());
  } else if (shortHelp_ != "") {
    CoinParamUtils::printIt(shortHelp_.c_str());
  } else {
    CoinParamUtils::printIt("No help provided.");
  }

  switch (type_) {
   case paramDbl: {
      buffer << "<Range of values is " << lowerDblValue_ << " to "
             << upperDblValue_ << ";\n\tcurrent " << dblValue_ << ">"
             << std::endl;
      assert(upperDblValue_ > lowerDblValue_);
      break;
   }
   case paramInt: {
      buffer << "<Range of values is " << lowerIntValue_ << " to "
             << upperIntValue_ << ";\n\tcurrent " << intValue_ << ">"
             << std::endl;
      assert(upperIntValue_ > lowerIntValue_);
      break;
   }
   case paramKwd: {
      printKwds();
      break;
   }
   case paramStr: {
      buffer << "<Current value is ";
      if (strValue_ == "") {
         buffer << "(unset)>";
      } else {
         buffer << "`" << strValue_ << "'>";
      }
      buffer << std::endl;
      break;
   }
   case paramAct: {
      break;
   }
   default: {
      buffer << "!! invalid parameter type !!" << std::endl;
      assert(false);
   }
  }
  return buffer.str();
}

/*
  Methods to manipulate the value of a parameter.
*/

/*
  Methods to manipulate the values associated with a keyword parameter.
*/

/* Set value of a parameter of any type */
int CoinParam::setVal(std::string value, std::string *message,
                      ParamPushMode pMode)
{
   switch(type_){
    case paramKwd:
      return setKwdVal(value, message, pMode);
    case paramStr:
      return setStrVal(value, message, pMode);
    case paramAct:
      std::cout << "setVal(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "setVal(): attepting to pass string as an argument, "
                << "to a parameter that is not of type paramStr or "
                << "paramKwd!" << std::endl;
      return 1;
   }
}

int CoinParam::setVal(double value, std::string *message, ParamPushMode pMode)
{
   switch(type_){
    case paramDbl:
      setDblVal(value, message, pMode);
      return 0;
    case paramAct:
      std::cout << "setVal(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "setVal(): attepting to pass double as an argument, "
                << "to a parameter that is not of type paramDbl!"
                << std::endl;
      return 1;
   }
}

int CoinParam::setVal(int value, std::string *message, ParamPushMode pMode)
{
   switch(type_){
    case paramInt:
      setIntVal(value, message, pMode);
      return 0;
    case paramKwd:
      setModeVal(value, message, pMode);
      return 0;
    case paramAct:
      std::cout << "setVal(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "setVal(): attepting to pass integer as an argument, "
                << "to a parameter that is not of type paramInt or ParamKwd!"
                << std::endl;
      return 1;
   }
}

/* Set value of a parameter of any type */
int CoinParam::setDefault(std::string value, std::string *message)
{
   switch(type_){
    case paramKwd:
      return setKwdValDefault(value, message);
    case paramStr:
      return setStrValDefault(value, message);
    case paramAct:
      std::cout << "setDefault(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "setDefault(): attepting to pass string as an argument, "
                << "to a parameter that is not of type paramStr or "
                << "paramKwd!" << std::endl;
      return 1;
   }
}

int CoinParam::setDefault(double value, std::string *message)
{
   switch(type_){
    case paramDbl:
      setDblValDefault(value, message);
      return 0;
    case paramAct:
      std::cout << "setDefault(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "setDefault(): attepting to pass double as an argument, "
                << "to a parameter that is not of type paramDbl!"
                << std::endl;
      return 1;
   }
}

int CoinParam::setDefault(int value, std::string *message)
{
   switch(type_){
    case paramInt:
      setIntValDefault(value, message);
      return 0;
    case paramKwd:
      setModeValDefault(value, message);
      return 0;
    case paramAct:
      std::cout << "setDefault(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "setDefault(): attepting to pass integer as an argument, "
                << "to a parameter that is not of type paramInt or ParamKwd!"
                << std::endl;
      return 1;
   }
}

/* Restore value of a parameter to its default */
int CoinParam::restoreDefault(){
   switch(type_){
    case paramInt:
      intValue_ = intDefaultValue_;
      return 0;
    case paramDbl:
      dblValue_ = dblDefaultValue_;
      return 0;
    case paramStr:
      strValue_ = strDefaultValue_;
      return 0;
    case paramKwd:
      currentMode_ = defaultMode_;
      currentKwd_ = defaultKwd_;
      return 0;
    default:
      std::cout << "restoreDefault() is illegal for parameter of this type!"
                << std::endl;
      return 1;
   }
}

/* Get value of a parameter of any type */
int CoinParam::getVal(std::string &value) const
{
   switch(type_){
    case paramKwd:
      value = currentKwd_;
      return 0;
    case paramStr:
      value = strValue_;
      return 0;
    case paramAct:
      std::cout << "getVal(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "getVal(): argument type doesn't match parameter type!"
                << std::endl;
      return 1;
   }      
}

int CoinParam::getVal(double &value) const
{
   switch(type_){
    case paramDbl:
      value = dblValue_;
      return 0;
    case paramAct:
      std::cout << "getVal(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "getVal(): argument type doesn't match parameter type!"
                << std::endl;
      return 1;
   }
}

int CoinParam::getVal(int &value) const
{
   switch(type_){
    case paramInt:
      value = intValue_;
      return 0;
    case paramAct:
      std::cout << "getVal(): Cannot be called for an action parameter!"
                << std::endl;
      return 1;
    default:
      std::cout << "getVal(): argument type doesn't match parameter type!"
                << std::endl;
      return 1;
   }
}

int CoinParam::readValue(std::deque<std::string> &inputQueue,
                         std::string &value,
                         std::string *message)
{
   value = CoinParamUtils::getNextField(inputQueue);
   
   if (value == "--" || value == "stdin"){
      value = "-";
   } else if (value == "stdin_lp"){
      value = "-lp";
   }

   if (value != ""){
      return 0;
   } else {
      std::ostringstream buffer;
      buffer << name_ << " has value ";
      if (type_ == paramStr){
         buffer << strValue_;
      } else {
         buffer << kwdVal();
      }
      buffer << std::endl;
      *message = buffer.str();
      return 1;
   }
}

// return 0 - okay, 1 bad, 2 not there
int CoinParam::readValue(std::deque<std::string> &inputQueue,
                         int &value,
                         std::string *message)
{
   std::string field = CoinParamUtils::getNextField(inputQueue);

   if (field.empty()){
      return 2;
   }
   
   char c;
   std::stringstream ss(field);
   ss >> value;
   if (!ss.fail() && !ss.get(c)){
      return 0;
   } else {
      std::ostringstream buffer;
      buffer << name_ << " has value " << intValue_ << std::endl;
      *message = buffer.str();
      return 1;
   }
}

int CoinParam::readValue(std::deque<std::string> &inputQueue,
                         double &value,
                         std::string *message)
{
   std::string field = CoinParamUtils::getNextField(inputQueue);

   if (field.empty()){
      return 2;
   }
   
   char c;
   std::stringstream ss(field);
   ss >> value;
   if (!ss.fail() && !ss.get(c)){
      return 0;
   } else {
      std::ostringstream buffer;
      buffer << name_ << " has value " << dblValue_ << std::endl;
      *message = buffer.str();
      return 1;
   }
}

/*
  Add a keyword to the list for a keyword parameter.

  Note, we don't check to make sure the index is not already used.
  The assumption is that we are mapping to an enum and the indices are
  thus automatically deconflicted.
*/
void CoinParam::appendKwd(std::string kwd, int mode)
{
  definedKwds_[kwd] = mode;
}

void CoinParam::appendKwd(std::string kwd)
{
  definedKwds_[kwd] = definedKwds_.size();
}

/*
  Scan the keywords of a keyword parameter and return the integer index of
  the keyword matching the input, or -1 for no match.
*/
int CoinParam::kwdToMode(std::string input) const
{
  assert(type_ == paramKwd);

  std::map<std::string, int>::const_iterator it;
  
  it = definedKwds_.find(input);

  if (it == definedKwds_.end()) {
      return -1;
   }else{
      return it->second;
   }

#if 0

  // This code scans through the list and allows for partial matches,
  // It's not finding much use right now and complicates matters.
  
  int whichItem = -1;
  size_t numberItems = definedKwds_.size();
  if (numberItems > 0) {
    size_t inputLen = input.length();
    /*
  Open a loop to check each keyword against the input string. We don't record
  the match length for keywords, so we need to check each one for an `!' and
  do the necessary preprocessing (record position and elide `!') before
  checking for a match of the required length.
*/
    for (it = definedKwds_.begin(); it != definedKwds_.end(); it++) {
      std::string kwd = definedKwds_[it];
      std::string::size_type shriekPos = kwd.find('!');
      size_t kwdLen = kwd.length();
      size_t matchLen = kwdLen;
      if (shriekPos != std::string::npos) {
        matchLen = shriekPos;
        kwd = kwd.substr(0, shriekPos) + kwd.substr(shriekPos + 1);
        kwdLen = kwd.length();
      }
      /*
  Match is possible only if input is shorter than the keyword. The entire input
  must match and the match must exceed the minimum length.
*/
      if (inputLen <= kwdLen) {
        unsigned int i;
        for (i = 0; i < inputLen; i++) {
          if (tolower(kwd[i]) != tolower(input[i]))
            break;
        }
        if (i >= inputLen && i >= matchLen) {
          whichItem = static_cast< int >(it);
          break;
        }
      }
    }
  }

  return (whichItem);
#endif
}

/*
  Set current value for a keyword parameter using a string.
*/
int CoinParam::setKwdVal(const std::string newKwd, std::string *message,
                         ParamPushMode pMode)
{
  assert(type_ == paramKwd);

  int newMode = kwdToMode(newKwd);
  if (newMode >= 0) {
     if (message){
        std::ostringstream buffer;
        buffer << "Option for " << name_ << " changed from ";
        buffer << currentKwd_ << " to " << newKwd << std::endl;
        *message = buffer.str();
     }
     currentKwd_ = newKwd;
     currentMode_ = newMode;
     if (pMode == pushOn){
        pushFunc_(*this);
     }
     return 0;
  }else{
     if (message){
        std::ostringstream buffer;
        buffer << "Illegal keyword. " << std::endl;
        *message += buffer.str();
        printOptions(message);
     }
     return 1;
  }
}

int CoinParam::setKwdValDefault(const std::string newKwd, std::string *message)
{
  assert(type_ == paramKwd);

  int newMode = kwdToMode(newKwd);
  if (newMode >= 0) {
     if (message){
        std::ostringstream buffer;
        buffer << "Default option for " << name_ << " set to " << newKwd
               << std::endl;
        *message = buffer.str();
     }
     defaultKwd_ = newKwd;
     defaultMode_ = newMode;
     return 0;
  }else{
     if (message){
        std::ostringstream buffer;
        buffer << "Illegal keyword. Options are" << std::endl;
        *message = buffer.str();
     }
     return 1;
  }
}

/*
  Set current value for keyword parameter using an integer. Echo the new value
  to cout if requested.
*/
int CoinParam::setModeVal(int newMode, std::string *message, ParamPushMode pMode)
{
  assert(type_ == paramKwd);

  std::map<std::string, int>::const_iterator it;
  std::string newKwd;
  for (it = definedKwds_.begin(); it != definedKwds_.end(); it++) {
     if (it->second == newMode){
        newKwd = it->first;
	break;
     }
  }
  if (it == definedKwds_.end()){
     if (message){
        std::ostringstream buffer;  
        buffer << "Illegal keyword." << printKwds() << std::endl;
        *message = buffer.str();
     }
     return 1;
  }else{
     if (message){
        std::ostringstream buffer;  
        buffer << "Option for " << name_ << " changed from ";
        buffer << currentKwd_ << " to " << newKwd << std::endl;
        *message = buffer.str();
     }
     currentKwd_ = newKwd;
     currentMode_ = newMode;
     if (pMode == pushOn){
        pushFunc_(*this);
     }
     return 0;
  }
}

int CoinParam::setModeValDefault(int newMode, std::string *message)
{
  assert(type_ == paramKwd);

  std::map<std::string, int>::const_iterator it;
  std::string newKwd;
  for (it = definedKwds_.begin(); it != definedKwds_.end(); it++) {
     if (it->second == newMode){
        newKwd = it->first;
	break;
     }
  }
  if (it == definedKwds_.end()){
     if (message){
        std::ostringstream buffer;  
        buffer << "Illegal keyword." << printKwds() << std::endl;
        *message = buffer.str();
     }
     return 1;
  }else{
     if (message){
        std::ostringstream buffer;  
        buffer << "Default option for " << name_ << " set to " << newKwd
               << std::endl;
        *message = buffer.str();
     }
     defaultKwd_ = newKwd;
     defaultMode_ = newMode;
     return 0;
  }
}

/*
  Return the string corresponding to the current value.
*/
std::string CoinParam::kwdVal() const
{
  assert(type_ == paramKwd);

  std::string returnStr;
  
  std::string::size_type shriekPos = currentKwd_.find('!');
  if (shriekPos != std::string::npos) {
     //contains '!'
     returnStr = currentKwd_.substr(0, shriekPos) + "(" +
        currentKwd_.substr(shriekPos + 1) + ")";
  }

  return (returnStr);
}

/*
  Return the string corresponding to the current value.
*/
int CoinParam::modeVal() const
{
  assert(type_ == paramKwd);

  return (currentMode_);
}

class kwds_compare
{
  const std::map<std::string, int>& definedKwds_;
public:
  kwds_compare(
    const std::map<std::string, int>& definedKwds
  )
  : definedKwds_(definedKwds)
  { }

  bool operator()(
    const std::string& kwd1,
    const std::string& kwd2
  ) const
  {
    return definedKwds_.at(kwd1) < definedKwds_.at(kwd2);
  }
};

/// Valid value-keywords for a keyword parameter sorted by integer mode
std::vector<std::string> CoinParam::definedKwdsSorted() const
{
  // could be done simpler if one were assuming a bijection between keywords and modes being in {0...definedKwds_.size()-1}
  std::vector<std::string> kwds;
  kwds.reserve(definedKwds_.size());
  for( std::map<std::string, int>::const_iterator it(definedKwds_.begin()); it != definedKwds_.end(); ++it )
    kwds.push_back(it->first);

  std::sort(kwds.begin(), kwds.end(), kwds_compare(definedKwds_));

  return kwds;
}

/*
  Print the keywords for a keyword parameter, formatted to indicate how they'll
  be matched. (E.g., some!Name prints as some(Name).). Follow with current
  value.
*/
std::string CoinParam::printKwds() const
{
  assert(type_ == paramKwd);

  std::ostringstream buffer;
  buffer << "Possible options for " << name_ << " are:";
  int maxAcross = 5;
  std::vector<std::string> kwds = definedKwdsSorted();
  size_t i;
  for (i = 0; i < kwds.size(); ++i) {
    std::string kwd = kwds[i];
    std::string::size_type shriekPos = kwd.find('!');
    if (shriekPos != std::string::npos) {
      kwd = kwd.substr(0, shriekPos) + "(" + kwd.substr(shriekPos + 1) + ")";
    }
    if (i % maxAcross == 0) {
      buffer << std::endl;
    }
    buffer << "  " << kwd;
  }
  buffer << std::endl;

  assert(currentMode_ >= 0);

  std::string current = currentKwd_;
  std::string::size_type shriekPos = current.find('!');
  if (shriekPos != std::string::npos) {
    current = current.substr(0, shriekPos) + "(" + current.substr(shriekPos + 1) + ")";
  }
  buffer << "  <current: " << current << ">" << std::endl;

  return buffer.str();
}

/*
  Methods to manipulate the value of a string parameter.
*/

int CoinParam::setStrVal(std::string value, std::string *message,
                         ParamPushMode pMode)
{
  assert(type_ == paramStr);

  if (message){
     std::ostringstream buffer;  
     buffer << name_ << " was changed from ";
     buffer << strValue_ << " to " << value << std::endl;
     *message = buffer.str();
  }
  strValue_ = value;
  if (pMode == pushOn){
     pushFunc_(*this);
  }
  return 0;
}

int CoinParam::setStrValDefault(std::string value, std::string *message)
{
  assert(type_ == paramStr);

  if (message){
     std::ostringstream buffer;  
     buffer << "default value for " << name_ << " was set to " << value
            << std::endl;
     *message = buffer.str();
  }
  strDefaultValue_ = value;
  return 0;
}

std::string CoinParam::strVal() const
{
  assert(type_ == paramStr);

  return (strValue_);
}

/*
  Methods to manipulate the value of a double parameter.
*/

int CoinParam::setDblVal(double value, std::string *message, ParamPushMode pMode)
{
  assert(type_ == paramDbl);

  if (value < lowerDblValue_ || value > upperDblValue_) {
     if (message){
        std::ostringstream buffer;
        buffer << value << " was provided for " << name_;
        buffer << " - valid range is " << lowerDblValue_;
        buffer << " to " << upperDblValue_ << std::endl;
        *message = buffer.str();
     }
     return 1;
  } else {
     if (message){
        std::ostringstream buffer;
        buffer << name_ << " was changed from ";
        buffer << dblValue_ << " to " << value << std::endl;
        *message = buffer.str();
     }
     dblValue_ = value;
     if (pMode == pushOn){
        pushFunc_(*this);
     }
     return 0;
  }
}

int CoinParam::setDblValDefault(double value, std::string *message)
{
  assert(type_ == paramDbl);

  if (value < lowerDblValue_ || value > upperDblValue_) {
     if (message){
        std::ostringstream buffer;
        buffer << value << " was provided as default value for " << name_;
        buffer << " - valid range is " << lowerDblValue_;
        buffer << " to " << upperDblValue_ << std::endl;
        *message = buffer.str();
     }
     return 1;
  } else {
     if (message){
        std::ostringstream buffer;
        buffer << name_ << " default was set to " << value << std::endl;
        *message = buffer.str();
     }
     dblDefaultValue_ = value;
     return 0;
  }
}

double CoinParam::dblVal() const
{
  assert(type_ == paramDbl);

  return (dblValue_);
}

void CoinParam::setLowerDblVal(double value)
{
  assert(type_ == paramDbl);

  lowerDblValue_ = value;
}

double CoinParam::lowerDblVal() const
{
  assert(type_ == paramDbl);

  return(lowerDblValue_);
}

void CoinParam::setUpperDblVal(double value)
{
  assert(type_ == paramDbl);

  upperDblValue_ = value;
}

double CoinParam::upperDblVal() const
{
  assert(type_ == paramDbl);

  return(upperDblValue_);
}

/*
  Methods to manipulate the value of an integer parameter.
*/

int CoinParam::setIntVal(int value, std::string *message, ParamPushMode pMode)
{
  assert(type_ == paramInt);

  if (value < lowerIntValue_ || value > upperIntValue_) {
     if (message){
        std::ostringstream buffer;
        buffer << value << " was provided for " << name_;
        buffer << " - valid range is " << lowerIntValue_ << " to ";
        buffer << upperIntValue_ << std::endl;
        *message = buffer.str();
     }
     return 1;
  } else {
     if (message){
        std::ostringstream buffer;
        buffer << name_ << " was changed from " << intValue_;
        buffer << " to " << value << std::endl;
        *message = buffer.str();
     }
     intValue_ = value;
     if (pMode == pushOn){
        pushFunc_(*this);
     }
     return 0;
  }
}

int CoinParam::setIntValDefault(int value, std::string *message)
{
  assert(type_ == paramInt);

  if (value < lowerIntValue_ || value > upperIntValue_) {
     if (message){
        std::ostringstream buffer;
        buffer << value << " was provided as default value for " << name_;
        buffer << " - valid range is " << lowerIntValue_ << " to ";
        buffer << upperIntValue_ << std::endl;
        *message = buffer.str();
     }
     return 1;
  } else {
     if (message){
        std::ostringstream buffer;
        buffer << name_ << " default was set to " << value << std::endl;
        *message = buffer.str();
     }
     intDefaultValue_ = value;
     return 0;
  }
}

int CoinParam::intVal() const
{
  assert(type_ == paramInt);

  return (intValue_);
}

void CoinParam::setLowerIntVal(int value)
{
  assert(type_ == paramInt);

  lowerIntValue_ = value;
}

int CoinParam::lowerIntVal() const
{
  assert(type_ == paramInt);

  return(lowerIntValue_);
}

void CoinParam::setUpperIntVal(int value)
{
  assert(type_ == paramInt);

  upperIntValue_ = value;
}

int CoinParam::upperIntVal() const
{
  assert(type_ == paramInt);

  return(upperIntValue_);
}

// Prints parameter options
void CoinParam::printOptions(std::string *message)
{
   std::ostringstream buffer;
   buffer << "Possible options for " << name_ << " are:";
   std::map<std::string, int>::const_iterator it;
   for (it = definedKwds_.begin(); it != definedKwds_.end(); it++) {
      std::string thisOne = it->first;
      std::string::size_type shriekPos = thisOne.find('!');
      if (shriekPos != std::string::npos) {
         //contains '!'
         thisOne = thisOne.substr(0, shriekPos) + "(" +
            thisOne.substr(shriekPos + 1) + ")";
      }
      buffer << " " << thisOne;
   }
   assert(currentMode_ >= 0 &&
          currentMode_ < static_cast< int >(definedKwds_.size()));
   std::string::size_type shriekPos = currentKwd_.find('!');
   std::string current = currentKwd_;
   if (shriekPos != std::string::npos) {
      //contains '!'
      current = currentKwd_.substr(0, shriekPos) + "(" +
         currentKwd_.substr(shriekPos + 1) + ")";
   }
   buffer << "\nCurrent setting is " << current << std::endl;

   if (message == NULL){
      std::cout << buffer.str();
   }else{
      *message += buffer.str();
   }
}

/*
  A print function (friend of the class)
*/

std::ostream &operator<<(std::ostream &s, const CoinParam &param)
{
  switch (param.type()) {
  case CoinParam::paramDbl: {
    return (s << param.dblVal());
  }
  case CoinParam::paramInt: {
    return (s << param.intVal());
  }
  case CoinParam::paramKwd: {
    return (s << param.kwdVal());
  }
  case CoinParam::paramStr: {
    return (s << param.strVal());
  }
  case CoinParam::paramAct: {
    return (s << "<evokes action>");
  }
  default: {
    return (s << "!! invalid parameter type !!");
  }
  }
}

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/
