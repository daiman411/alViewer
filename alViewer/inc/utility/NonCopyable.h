#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

class NonCopyable
{
protected:
    NonCopyable(){ }
    ~NonCopyable(){ }

private:
    NonCopyable(const NonCopyable& );
    const NonCopyable& operator=(const NonCopyable& );
};

#endif // _NONCOPYABLE_H_
