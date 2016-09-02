/**
	Common.h
	Provides common constants and helper methods

	@author Paul Wang
	@version 9/8/16
*/

#pragma once

#include <BWAPI.h>

namespace ProBot{
	// common constants
	namespace Constants{
		const int FRAME_TO_SECONDS = 24;
		const bool DEBUG_ENABLED = true;
	}

	// alternate initialization format for vectors.
	template <typename T>
	class make_vector {
	public:
		typedef make_vector<T> my_type;
		my_type& operator<< (const T& val) {
			data_.push_back(val);
			return *this;
		}
		operator std::vector<T>() const {
			return data_;
		}
	private:
		std::vector<T> data_;
	};
}