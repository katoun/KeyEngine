/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Attribute.h>
#include <Reflection/Any.h>
#include <Reflection/TypeInfo.h>

#include <functional>
#include <string>
#include <type_traits>

namespace attribute
{
	struct RUNTIME_API Getter : public reflection::Attribute
	{
		typedef std::function<reflection::Any(const reflection::Any &)> FuctionType;

		FuctionType Value;

		Getter(FuctionType value) : Value(value) {};
	};

	struct RUNTIME_API Setter : public reflection::Attribute
	{
		typedef std::function<void(reflection::Any &, const reflection::Any &)> FuctionType;

		FuctionType Value;

		Setter(FuctionType value) : Value(value) {};
	};

	struct RUNTIME_API DisplayName : public reflection::Attribute
	{
		static std::string Default;

		std::string Value;

		DisplayName(const std::string name) : Value(name) {};
	};

	struct RUNTIME_API Category : public reflection::Attribute
	{
		static std::string Default;

		std::string Value;

		Category(const std::string category) : Value(category) {};
	};

	struct RUNTIME_API Range : public reflection::Attribute
	{
		float A;
		float B;

		Range(const float a, const float b) : A(a), B(b) {};
	};
}

namespace reflection
{
	template<typename T>
	struct MemberFieldTraits;

	template<typename ClassType, typename FieldType>
	struct MemberFieldTraits<FieldType ClassType::*>
	{
		using Class = ClassType;
		using Field = FieldType;
	};

	template<auto Member>
	std::pair<Type, const Attribute*> FieldGetter()
	{
		using Traits = MemberFieldTraits<decltype(Member)>;
		using ClassType = typename Traits::Class;

		return AttributeInitializer<attribute::Getter>([](const Any &object)
		{
			if (object.GetType() == TypeOf<ClassType>())
			{
				auto &instance = object.GetValue<ClassType>();
				return Any{ instance.*Member };
			}

			auto &instance = object.GetValue<ClassType*>();
			return Any{ instance->*Member };
		});
	}

	template<auto Member>
	std::pair<Type, const Attribute*> FieldSetter()
	{
		using Traits = MemberFieldTraits<decltype(Member)>;
		using ClassType = typename Traits::Class;
		using FieldType = typename Traits::Field;

		static_assert(!std::is_const_v<FieldType>, "Const fields cannot be assigned through reflection.");

		return AttributeInitializer<attribute::Setter>([](Any &object, const Any &value)
		{
			if (object.GetType() == TypeOf<ClassType>())
			{
				auto &instance = object.GetValue<ClassType>();
				instance.*Member = value.GetValue<FieldType>();
				return;
			}

			auto &instance = object.GetValue<ClassType*>();
			instance->*Member = value.GetValue<FieldType>();
		});
	}
}
