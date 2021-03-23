#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

template<class T>
class Singleton
{
protected:
	Singleton(){}
	virtual ~Singleton(){}

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

public:
	static T* GetInstance()
	{
		static T m_Instance;
		return &m_Instance;
	}
};

#endif // !_SINGLETON_HPP_
