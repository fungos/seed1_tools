#include "groupmanager.h"

GroupManager::GroupManager() : pGroups()
{
	pGroups.empty();
}

GroupManager::~GroupManager()
{
	pGroups.empty();
}

void GroupManager::Add(Group *g)
{
	pGroups[g->GetName()] = g;
}

GroupMapIterator GroupManager::GetFirst()
{
	return pGroups.begin();
}

GroupMapIterator GroupManager::GetLast()
{
	return pGroups.end();
}

Group *GroupManager::Get(const char *groupName)
{
	Group *ret = NULL;
	if (groupName)
	{
		if (pGroups[groupName])
			ret = pGroups[groupName];
	}
	return ret;
}
