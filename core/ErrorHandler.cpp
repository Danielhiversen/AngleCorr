#include "ErrorHandler.hpp"


ErrorObserver::ErrorObserver():
    mError(false),
    mWarning(false),
    mErrorMessage(""),
    mWarningMessage("") {}

  ErrorObserver* ErrorObserver::New()
  {
	  return new ErrorObserver;
  }

  bool ErrorObserver::GetError() const
  {
	  return mError;
  }

  bool ErrorObserver::GetWarning() const
  {
	  return mWarning;
  }

  void ErrorObserver::Clear()
  {
	    mError = false;
	    mWarning = false;
	    mErrorMessage = "";
	    mWarningMessage = "";
  }
  void ErrorObserver::Execute(vtkObject *vtkNotUsed(caller),
                       unsigned long event,
                       void *calldata)
  {
	  switch(event)
		{
		case vtkCommand::ErrorEvent:
		  mErrorMessage = static_cast<char *>(calldata);
		  mError = true;
		  break;
		case vtkCommand::WarningEvent:
		  mWarningMessage = static_cast<char *>(calldata);
		  mWarning = true;
		  break;
		}
  }
  std::string ErrorObserver::GetErrorMessage()
  {
	  return mErrorMessage;
  }
  std::string ErrorObserver::GetWarningMessage()
  {
	  return mWarningMessage;
  }

void reportError(std::string  errMsg){
	throw std::runtime_error(errMsg);
}



