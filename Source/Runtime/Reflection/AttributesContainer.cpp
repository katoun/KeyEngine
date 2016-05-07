#include <Reflection/AttributesContainer.h>
#include <Reflection/Attribute.h>

#include <cassert>

namespace reflection
{
	std::vector<std::pair<Type, const Attribute *>> AttibutesContainer::GetAttributes(void) const
	{
		std::vector<std::pair<Type, const Attribute *>> attributes;

		for (auto &it : m_Attributes)
		{
			attributes.emplace_back(std::make_pair(it.first, it.second));
		}

		return attributes;
	}

	void AttibutesContainer::SetAttributes(const Initializer &attributes)
	{
		for (auto &pair : attributes)
		{
			assert(pair.first.IsValid());

			auto search = m_Attributes.find(pair.first);

			if (search != m_Attributes.end())
			{
				delete search->second;
			}

			m_Attributes[pair.first] = pair.second;
		}
	}
}