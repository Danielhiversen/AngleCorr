#include "ErrorHandler.hpp"


ErrorObserver::ErrorObserver():
    Error(false),
    Warning(false),
    ErrorMessage(""),
    WarningMessage("") {}

  ErrorObserver* ErrorObserver::New()
  {
	  return new ErrorObserver;
  }

  bool ErrorObserver::GetError() const
  {
	  return this->Error;
  }

  bool ErrorObserver::GetWarning() const
  {
	  return this->Warning;
  }

  void ErrorObserver::Clear()
  {
	  this->Error = false;
	  this->Warning = false;
	  this->ErrorMessage = "";
	  this->WarningMessage = "";
  }
  void ErrorObserver::Execute(vtkObject *vtkNotUsed(caller),
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
  std::string ErrorObserver::GetErrorMessage()
  {
	  return ErrorMessage;
  }
  std::string ErrorObserver::GetWarningMessage()
  {
	  return WarningMessage;
  }

void reportError(std::string  errMsg){
	throw std::runtime_error(errMsg);
}



