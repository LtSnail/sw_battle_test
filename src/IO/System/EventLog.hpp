#pragma once

#include "details/PrintFieldVisitor.hpp"

#include <iostream>
#include <type_traits>

namespace sw
{
	class EventLog
	{
	public:
		template <class TEvent>
		void log(uint32_t turn, TEvent&& event)
		{
			using EventType = std::decay_t<TEvent>;
			std::cout << turn << " " << EventType::Name << " ";
			PrintFieldVisitor visitor(std::cout);
			event.visit(visitor);
			std::cout << std::endl;
		}
	};
}
