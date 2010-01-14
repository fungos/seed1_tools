#ifndef __GROUP_H__
#define __GROUP_H__

#include <map>

#include "defines.h"
#include "iobject.h"
#include "iresource.h"
//#include "package.h"

using namespace std;

typedef map<const char *, IObject *, StringComparator>::iterator ObjectMapIterator;
//typedef ResourceMap::iterator ResourceMapIterator;

class Group
{
	friend class Package;

	private:
		char *sName;
		map<const char *, IObject *, StringComparator> pObjects;
		//map<const char *, IResource *, StringComparator> pResources;
		ResourceMap pResources;

		Group(const Group &g);
		Group &operator=(const Group &g);

	public:
		Group(const char *name);
		~Group();

		void AddObject(IObject *o);
		void AddResource(IResource *r);
		IObject *GetObject(const char *name);
		IResource *GetResource(const char *fileName);

		ObjectMapIterator GetFirstObject();
		ObjectMapIterator GetLastObject();
		ResourceMapIterator GetFirstResource();
		ResourceMapIterator GetLastResource();

		const char *GetName() const;
		u32 GetObjectsAmount() const;
		u32 GetResourcesAmount() const;
};

#endif
