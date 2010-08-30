/* 
 * File:   error.h
 * Author: cielak
 *
 * Created on 19 sierpień 2010, 14:42
 */

#ifndef ERROR_H
#define	ERROR_H
#include <cstdio>
#include <cstdlib>
class error {
public:
    error();
    error(const error& orig);
    void say(const char* message);
    virtual ~error();
private:

};

error & operator <<(error &err, const char* message);
error & operator <<(error &err, int number);
#endif	/* ERROR_H */

