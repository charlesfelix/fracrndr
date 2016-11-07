//
//  Exception.hpp
//  FracRndr
//
//  Created by Charles-Felix on 11/6/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Exception_h
#define Exception_h

#include <stdexcept>

#define FR_DECL_GENERIC_EXCEPTION(name, base_class) \
class name : public base_class \
{ \
public: \
explicit name(const std::string &msg = "") throw() \
: base_class(msg) \
{ } \
~name() throw() \
{ } \
}; \

namespace Fr {
    

    class Exception : public std::exception
    {
    public:
        
        Exception(const std::string &msg) throw(): std::exception(), m_msg(msg)
        {
            
        }
        
        virtual ~Exception() throw
        {
            
        }
        
        virtual const char* what() const throw()
        {
            return m_msg.c_str();
        }
    private:
        
        std::string m_msg;
        
    }; // Exception
} // namespace Fr

FR_DECL_GENERIC_EXCEPTION(NoSuchFileException);
// ParserExceptions
FR_DECL_GENERIC_EXCEPTION(ParsingException);


#endif /* Exception_h */
