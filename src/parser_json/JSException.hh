#pragma once

#include <iostream>
#include <sstream>

/**
 * \author HONG Silien
 * \version 1.0
 * \date 03/04/2009
 * \brief Definition module of the JSON Exception application.
 * We define here after all error exception that's application can throw.
 * Note :
 * All Exception are attached to an interface Action for execute special thing
 * that's user wants if error have been throw.
 */

/**
 * Error Number List
 */
enum js_error_number{
   json_file_unknow
   ,json_syntax_error
};

/**
 * Pre-declaration of JSException
 */
class JSException;

/**
 * Action Class Handler, executing after the creation Exception
 */
class doJSAction{
  public:
    /**
     * The operation action to define (give the current exception which have triggered the action)
     */
    virtual void action(const JSException& exception) = 0;
    /**
     * Force the derived destruction class
     */
    virtual ~doJSAction() {
    }
};

/**
 * Definition JSON Exception
 */
class JSException{
  private:
    /// Error number
    const js_error_number error;
    //// Message Error
    std::string message;
    /// Which file have triggered the exception
    const std::string file;
    /// Which class have triggered the exception
    const std::string class_name;
    /// Which function have triggered the exception
    const std::string function_name;
    /// Which action to execute after the throwing Exception
    const doJSAction* action;

  public:
    /**
     * Constructor with initialization.
     * By default, action is not activated
     */
    JSException(const js_error_number error, const std::string& file, const std::string& class_name, const std::string& function_name,
        const std::string& message) :
      error(error), message(message), file(file), class_name(class_name), function_name(function_name), action(NULL) {
    }
    /**
     * Constructor with initialization.
     * By default, action is not activated
     */
    JSException(const js_error_number error, const char* file, const char* class_name, const char* function_name, const std::string& message) :
      error(error), message(message), file(file), class_name(class_name), function_name(function_name), action(NULL) {
    }
    /**
     * Constructor with initialization.
     * By default, action is not activated
     */
    JSException(const js_error_number error, const char* file, const char* class_name, const char* function_name, const char* message) :
      error(error), message(message), file(file), class_name(class_name), function_name(function_name) {
    }


    /**
     * Adding more message to exception
     */
    void addMessage(const std::string& mess){
      message += mess;
    }

    /**
     * Adding more message to exception
     */
    void addMessage(const char* mess){
      message += mess;
    }

    /**
     * Return the String description of the exception
     */
    std::string str() const{
      std::stringstream res;
      res << "Error Number     : " << error << std::endl;
      res << "File             : " << file << std::endl;
      res << "Class Name       : " << class_name << std::endl;
      res << "Function Name    : " << function_name << std::endl;
      res << "Message          : " << std::endl;
      res << message << std::endl;
      return res.str();
    }
};

/// Pretty print
std::ostream & operator << (std::ostream & os, const JSException& e);
