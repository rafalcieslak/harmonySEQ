/* 
 * File:   error.cpp
 * Author: cielak
 * 
 * Created on 19 sierpień 2010, 14:42
 */

#include "error.h"

error::error() {

}

error::error(const error& orig) {
}

error::~error() {
}

void error::say(const char* message){
    //fputs("ERROR - ",stdout);
    fputs(message,stdout);

}
error & operator <<(error &err, const char* message){
    err.say(message);
    return err;
}

error & operator <<(error &err, int number){
    char temp[20];
    sprintf(temp,"%d",number);
    err.say(temp);
    return err;
}
