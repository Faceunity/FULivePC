#include "fu_shared_obj.h"
#include <unordered_map>
#include <assert.h>


static std::unordered_map<std::string, const ClassInfo*>& GetClassMap()
{
	static std::unordered_map<std::string, const ClassInfo*> GGlobalClasses;
	return GGlobalClasses;
}

static std::unordered_map<std::string, const ClassInfo**>& UnmappedClassNames()
{
	static std::unordered_map<std::string, const ClassInfo**> GUnmapClasses;
	return GUnmapClasses;
}

static void SetUnmapped(const std::string& name, const ClassInfo** addr)
{
	UnmappedClassNames()[name] = addr;
}

static void MapClassName(const std::string& name, const ClassInfo* info)
{
	GetClassMap()[name] = info;

	auto it = UnmappedClassNames().find(name);
	if (it != UnmappedClassNames().end())
	{
		*it->second = info;
		UnmappedClassNames().erase(it);
	}
}

const ClassInfo* StaticClass(const std::string& className)
{
	auto it = GetClassMap().find(className);
	if (it == GetClassMap().end())
	{
		return nullptr;
	}
	return it->second;
}

std::unique_ptr<ISharedObject> StaticNewObject(const std::string& className, std::shared_ptr<ISharedObject> obj/* = nullptr*/)
{
	const ClassInfo* classInfo = StaticClass(className);
	return StaticNewObject(classInfo, obj);
}

std::unique_ptr<ISharedObject> StaticNewObject(const ClassInfo* classInfo, std::shared_ptr<ISharedObject> obj/* = nullptr*/)
{
	if (!classInfo || !classInfo->mConstruct)
	{
		return nullptr;
	}

	if (obj && classInfo->mCopyConstruct)
	{
		return std::move(classInfo->mCopyConstruct(obj));
	}
	return obj ? nullptr : std::move(classInfo->mConstruct());
}

Implement_Abstract_FUObject(ISharedObject);

ClassInfo::ClassInfo(
	const std::string& className,
	const std::string& parentClassName,
	ConstructDelegate cbConstruct,
	CopyConstructDelegate cbCopyConstruct)
	: mClassName(className)
	, mParentClass(nullptr)
	, mConstruct(cbConstruct)
	, mCopyConstruct(cbCopyConstruct)
{
	assert(!mClassName.empty());
	assert(!parentClassName.empty());
	assert(GetClassMap().find(mClassName) == GetClassMap().end());

	if (className != parentClassName)
	{
		mParentClass = StaticClass(parentClassName);
		if (!mParentClass)
		{
			SetUnmapped(parentClassName, &mParentClass);
		}
	}

	static uint32_t sClassIndex = 0;
	mClassId = ++sClassIndex;
	MapClassName(mClassName, this);
}

bool ClassInfo::IsCastable(const ClassInfo* classInfo)const
{
	if (!classInfo)
	{
		return false;
	}

	if (this == classInfo)
	{
		return true;
	}

	const ClassInfo* parent = nullptr;
	const ClassInfo* child = nullptr;
	if (mClassId > classInfo->mClassId)
	{
		parent = classInfo;
		child = this;
	}
	else
	{
		parent = this;
		child = classInfo;
	}

	while (child->mParentClass)
	{
		if (child->mParentClass == parent)
		{
			return true;
		}
		child = child->mParentClass;
	}
	return false;
}