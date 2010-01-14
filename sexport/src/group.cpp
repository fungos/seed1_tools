#include "group.h"
#include "exporter.h"

Group::Group(const Group &g) : sName(NULL), pObjects(), pResources()
{
}

Group &Group::operator=(const Group &g)
{
	return *this;
}

Group::Group(const char *name) : sName(NULL), pObjects(), pResources()
{
		sName = strdup(name);
		pObjects.empty();
		pResources.empty();
}

Group::~Group()
{
		if (sName)
			free (sName);
}

void Group::AddObject(IObject *o)
{
	pObjects[o->GetName()] = o;
}

void Group::AddResource(IResource *r)
{
	if (!e->IsPackageResourcesEnabled())
	{
//#if COMPILE_RESOURCE == 1
		fprintf(stderr, "\nNAO DEVERIA TER RES SENDO ADICIONADO: %s\n", r->GetName());
		//exit(EXIT_FAILURE);
//#endif
	}
	else
	{

		pResources[r->GetName()] = r;
	}
}

IObject *Group::GetObject(const char *name)
{
	IObject *ret = NULL;
	if (name)
	{
		if (pObjects[name])
			ret = pObjects[name];
	}
	return ret;
}

IResource *Group::GetResource(const char *fileName)
{
	IResource *ret = NULL;
	if (fileName)
	{
		if (pResources[fileName])
			ret = pResources[fileName];
	}
	return ret;
}

ObjectMapIterator Group::GetFirstObject()
{
	return pObjects.begin();
}

ObjectMapIterator Group::GetLastObject()
{
	return pObjects.end();
}

ResourceMapIterator Group::GetFirstResource()
{
	return pResources.begin();
}

ResourceMapIterator Group::GetLastResource()
{
	return pResources.end();
}

const char *Group::GetName() const
{
	return sName;
}

u32 Group::GetObjectsAmount() const
{
	return pObjects.size();
}

u32 Group::GetResourcesAmount() const
{
	return pResources.size();
}
