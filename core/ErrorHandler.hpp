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
  ErrorObserver();
  static ErrorObserver *New();
  bool GetError() const;
  bool GetWarning() const;
  void Clear();
  virtual void Execute(vtkObject *vtkNotUsed(caller),
                       unsigned long event,
                       void *calldata);
  std::string GetErrorMessage();
  std::string GetWarningMessage();
  
private:
  bool        Error;
  bool        Warning;
  std::string ErrorMessage;
  std::string WarningMessage;
};

void reportError(std::string  errMsg);




#endif /* ERRORHANDLER_HPP_ */


