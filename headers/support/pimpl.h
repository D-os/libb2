#ifndef PIMPL_IMPL_H
#define PIMPL_IMPL_H

#include <utility>

#include "SupportDefs.h"

template <typename T>
pimpl<T>::pimpl() : m{new T{}}
{
}

template <typename T>
template <typename... Args>
pimpl<T>::pimpl(Args&&... args)
	: m{new T{std::forward<Args>(args)...}}
{
}

template <typename T>
pimpl<T>::~pimpl()
{
}

template <typename T>
T* pimpl<T>::operator->() const
{
	return m.get();
}

template <typename T>
T& pimpl<T>::operator*() const
{
	return *m.get();
}

#endif
