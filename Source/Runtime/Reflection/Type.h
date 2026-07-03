/* Copyright (c) 2016 Catalin-Alexandru Nastase
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, 
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
* THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/TypeConfig.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <set>

namespace core
{
	class Object;
}

namespace reflection
{
	class Any;
	class Field;
	struct Attribute;

	class RUNTIME_API Type
	{
	public:
		typedef std::vector<Type> List;
		typedef std::set<Type> Set;

		static const TypeID Invalid = 0;

		Type(void);
		Type(const Type &rhs);
		Type(TypeID id);

		operator bool(void) const;

		Type &operator=(const Type &rhs);

		bool operator<(const Type &rhs) const;
		bool operator>(const Type &rhs) const;
		bool operator<=(const Type &rhs) const;
		bool operator>=(const Type &rhs) const;
		bool operator==(const Type &rhs) const;
		bool operator!=(const Type &rhs) const;

		TypeID GetID(void) const;

		const std::string &GetName(void) const;

		bool IsValid(void) const;
		bool IsPrimitive(void) const;
		bool IsEnum(void) const;
		bool IsPointer(void) const;
		bool IsClass(void) const;

		Any Create() const;
		Any CreateDynamic() const;
		std::shared_ptr<core::Object> CreateDynamicObject() const;
		Any CreateDynamicPointer(core::Object& object) const;
		void Destroy(Any &instance) const;

		template<typename AttributeType>
		std::shared_ptr<const AttributeType> GetAttribute(void) const;

		std::vector<std::pair<Type, std::shared_ptr<const Attribute>>> GetAttributes(void) const;

		const Field &GetField(const std::string &name) const;
		std::vector<Field> GetFields(void) const;

		bool IsDerivedFrom(const Type &other) const;
		template<typename T>
		bool IsDerivedFrom(void) const;

		bool IsTypeOf(const Type &other) const;
		template<typename T>
		bool IsTypeOf(void) const;

		const Set &GetBaseClasses(void) const;

	private:

		friend class std::allocator<Type>;

		friend class TypeData;
		friend class Any;
		friend class Field;

		TypeID m_ID;
	};

}
