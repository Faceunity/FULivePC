#ifndef _FL_UNCOPYABLE_H_
#define _FL_UNCOPYABLE_H_

class Uncopyable
{
protected:
	Uncopyable() {}
	~Uncopyable() {}
private:
	Uncopyable(const Uncopyable &c);
	Uncopyable& operator= (const Uncopyable &c);
};

#endif
