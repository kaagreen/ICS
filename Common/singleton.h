#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
class Singleton {
public:
    static T* instance() {
      if (m_pinstance == 0)
          m_pinstance = new T();
      return m_pinstance;
    }
    static void dropinstance() {
        if (m_pinstance != 0)
		{
			delete m_pinstance;
            m_pinstance = 0;
		}
    }

private:
  static T* m_pinstance;
private:
  Singleton();
  ~Singleton();
  Singleton(const Singleton &);
  Singleton& operator=(const Singleton &);
};

template <class T> T* Singleton<T>::m_pinstance = 0;

#endif // SINGLETON_H
