#pragma once

#include <vector>
#include <memory> // For std::shared_ptr (optional, but good practice for memory management)


class IObserver;


class ISubject
{
public:
    virtual ~ISubject() = default; 
    
    virtual void Attach(IObserver* observer) = 0;
    
    virtual void Detach(IObserver* observer) = 0;
    
    virtual void Notify() = 0;
};



class IObserver
{
public:
    virtual ~IObserver() = default; 
    
    virtual void Update(ISubject* subject) = 0; 
};

class IMatchHandler 
{
public:
    ~IMatchHandler() = default;

    virtual void Attach(IObserver* observer) = 0;
    
    virtual void Detach(IObserver* observer) = 0;
    
    virtual void Notify(float Power) = 0;
};