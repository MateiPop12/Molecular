#include "Molecular.h"

class Sandbox : public Molecular::App
{
public:
	Sandbox() 
	{
	}
	~Sandbox() 
	{
	}
};

Molecular::App* Molecular::CreateApplication()
{
	return new Sandbox();
}