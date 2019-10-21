#pragma once

template<typename T>
T&& move(T& arg)
{
	return static_cast<T&&>(arg);
}