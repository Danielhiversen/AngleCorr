/*
 * ErrorHandler.hpp
 *
 *  Created on: Jun 10, 2015
 *      Author: dahoiv
 */

#ifndef ERRORHANDLER_HPP_
#define ERRORHANDLER_HPP_


#include <string>
#include <stdexcept>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>


class ErrorObserver : public vtkCommand
{
public:
  ErrorObserver():
    Error(false),
    Warning(false),
    ErrorMessage(""),
    WarningMessage("") {}
  static ErrorObserver *New()
  {
	  return new ErrorObserver;
  }

  bool GetError() const
  {
	  return this->Error;
  }

  bool GetWarning() const
  {
	  return this->Warning;
  }

  void Clear()
  {
	  this->Error = false;
	  this->Warning = false;
	  this->ErrorMessage = "";
	  this->WarningMessage = "";
  }
  virtual void Execute(vtkObject *vtkNotUsed(caller),
                       unsigned long event,
                       void *calldata)
  {
	  switch(event)
		{
		case vtkCommand::ErrorEvent:
		  ErrorMessage = static_cast<char *>(calldata);
		  this->Error = true;
		  break;
		case vtkCommand::WarningEvent:
		  WarningMessage = static_cast<char *>(calldata);
		  this->Warning = true;
		  break;
		}
  }
  std::string GetErrorMessage()
  {
	  return ErrorMessage;
  }
  std::string GetWarningMessage()
  {
	  return WarningMessage;
  }

private:
  bool        Error;
  bool        Warning;
  std::string ErrorMessage;
  std::string WarningMessage;
};

void reportError(std::string  errMsg){
	throw std::runtime_error(errMsg);
}





#endif /* ERRORHANDLER_HPP_ */

