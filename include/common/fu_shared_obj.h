#ifndef __FU_SHARED_OBJECT_H__
#define __FU_SHARED_OBJECT_H__

#include <memory>
#include <functional>
#include <string>

#define Declare_FUObject(Class,ParentClass)\
public:\
	virtual const char* ClassName()const{return #Class;}\
	virtual const char* ParentClassName()const{return #ParentClass;}\
	virtual const ClassInfo* GetClass()const{return StaticClass(ClassName());}\
	static const char* StaticClassName(){return #Class;}\
	static const char* StaticParentClassName(){return #ParentClass;}\
	static std::unique_ptr<ISharedObject> StaticConstruct(void);\
	static std::unique_ptr<ISharedObject> StaticCopyConstruct(std::shared_ptr<ISharedObject>);

// TODO memory manager
#define Implement_FUObject(Class)\
std::unique_ptr<ISharedObject> Class::StaticConstruct(void)\
{\
	return std::make_unique<Class>();\
}\
std::unique_ptr<ISharedObject> Class::StaticCopyConstruct(std::shared_ptr<ISharedObject> other)\
{\
	std::shared_ptr<Class> inst = DynamicCast<Class>(other);	 \
	return inst ? std::make_unique<Class>(*inst) : nullptr;\
}\
ClassInfo G##Class##ClassInfo(\
	Class::StaticClassName(),\
	Class::StaticParentClassName(),\
	&Class::StaticConstruct,\
	&Class::StaticCopyConstruct)

#define Implement_FUObject_NoCopy(Class)\
std::unique_ptr<ISharedObject> Class::StaticConstruct(void)\
{\
	return std::make_unique<Class>();\
}\
ClassInfo G##Class##ClassInfo(\
	Class::StaticClassName(),\
	Class::StaticParentClassName(),\
	&Class::StaticConstruct,\
	nullptr)

#define Implement_Abstract_FUObject(Class)\
ClassInfo G##Class##ClassInfo(\
	Class::StaticClassName(),\
	Class::StaticParentClassName(),\
	nullptr, nullptr)


class ISharedFromThis : public std::enable_shared_from_this<ISharedFromThis>
{
public:
	virtual ~ISharedFromThis()
	{
	}

	/**
	\brief cast to specified type
	\      if it's a null pointer, use DynamicCast
	*/
	template<typename T>
	std::shared_ptr<T> Cast()
	{
		return std::dynamic_pointer_cast<T>(shared_from_this());
	}
};

class ISharedObject;
typedef std::function<std::unique_ptr<ISharedObject>(void)> ConstructDelegate;
typedef std::function<std::unique_ptr<ISharedObject>(std::shared_ptr<ISharedObject>)> CopyConstructDelegate;

class ClassInfo
{
public:
	ClassInfo(const std::string& className,
		const std::string& parentClassName,
		ConstructDelegate cbConstruct,
		CopyConstructDelegate cbCopyConstruct);

	bool IsCastable(const ClassInfo* classInfo)const;

protected:
	std::string           mClassName;
	const ClassInfo*      mParentClass;
	ConstructDelegate     mConstruct;
	CopyConstructDelegate mCopyConstruct;
	uint32_t              mClassId;

	friend std::unique_ptr<ISharedObject> StaticNewObject(const ClassInfo*, std::shared_ptr<ISharedObject>);
};

extern const ClassInfo* StaticClass(const std::string& className);
extern std::unique_ptr<ISharedObject> StaticNewObject(const std::string& className, std::shared_ptr<ISharedObject> obj = nullptr);
extern std::unique_ptr<ISharedObject> StaticNewObject(const ClassInfo* classInfo, std::shared_ptr<ISharedObject> obj = nullptr);

class ISharedObject : public ISharedFromThis
{
	Declare_FUObject(ISharedObject, ISharedObject)
public:
	void SetName(const std::string& name)
	{
		mName = name;
	}

	const std::string& GetName()const
	{
		return mName;
	}

	virtual std::string GetDescriber()const
	{
		return ClassName();
	}

protected:
	std::string mName;
};

template<typename T>
std::shared_ptr<T> MakeSharedObject(const std::string& className, std::shared_ptr<ISharedObject> obj = nullptr)
{
	std::shared_ptr<ISharedObject> inst(std::move(StaticNewObject(className, obj)));
	return inst ? inst->Cast<T>() : nullptr;
}

template<typename T>
std::shared_ptr<T> MakeSharedObject(std::shared_ptr<ISharedObject> obj = nullptr)
{
	return MakeSharedObject<T>(T::StaticClassName(), obj);
}

template<typename T, typename S>
std::shared_ptr<T> DynamicCast(std::shared_ptr<S> obj)
{
	return std::dynamic_pointer_cast<T>(obj);
}

#endif
