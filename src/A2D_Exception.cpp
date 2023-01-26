#pragma once

#include <A2D_Exception.h>

A2D::Exception::Exception(const char* data)
{
	this->m_Message = data;
}
