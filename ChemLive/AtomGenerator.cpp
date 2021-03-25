#include "pch.h"
#include "AtomGenerator.h"


namespace Simulation
{
	AtomGenerator::AtomGenerator(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_deviceResources(deviceResources)
	{
	}

	Atom* AtomGenerator::CreateAtom(Element element, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount, int charge)
	{
		switch (element)
		{
		case Element::HYDROGEN: return new Hydrogen(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::HELIUM:   return new Helium(m_deviceResources, position, velocity, neutronCount, charge);
		default:
			return nullptr;
		}
	}
}