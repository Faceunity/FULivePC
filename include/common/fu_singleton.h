#ifndef _FL_SINGLETON_H_
#define _FL_SINGLETON_H_

#include "fu_uncopyable.h"
#include "fu_lock_mutex.h"

template <class T>
class SingleTon:public Uncopyable
{
public:

	static T & instance() 
	{
		if (!m_pInstance)
		{
			Locker lock(&m_mtx);
			if (!m_pInstance)
			{
				m_pInstance = new T();
				//atexit() ����������Ҫ����ɶ״̬����ʵ�أ����س����˳�����ڴ涼�ᱻ���յģ�ûɶ��Ҫע���������
			}
		}
		
		return *m_pInstance;
	};

protected:
	SingleTon() {};
	~SingleTon() {};

private:
	static T* volatile m_pInstance;
	static L_Mutex m_mtx;
};

template<class T>
T* volatile SingleTon<T>::m_pInstance = nullptr;

template<class T>
L_Mutex SingleTon<T>::m_mtx;

#endif