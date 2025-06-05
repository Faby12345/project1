#ifndef API_H
#define API_H
#include <iostream>
class API
{
private :
    const std::string token = "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";
public:
    API();
    const std::string getToken(){
        return token;
    }
};

#endif // API_H
