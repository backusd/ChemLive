#include "pch.h"
#include "AtomGenerator.h"

namespace Simulation
{
	AtomGenerator::AtomGenerator(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_deviceResources(deviceResources)
	{
	}

	Atom* AtomGenerator::CreateAtom(Element element, XMFLOAT3 position, XMFLOAT3 velocity)
	{
		switch (element)
		{
		case Element::HYDROGEN:		return new Hydrogen(m_deviceResources, position, velocity);
		case Element::HELIUM:		return new Helium(m_deviceResources, position, velocity);
		case Element::LITHIUM:		return new Lithium(m_deviceResources, position, velocity);
		case Element::BERYLLIUM:	return new Beryllium(m_deviceResources, position, velocity); 
		case Element::BORON:		return new Boron(m_deviceResources, position, velocity);
		case Element::CARBON:		return new Carbon(m_deviceResources, position, velocity);
		case Element::NITROGEN:		return new Nitrogen(m_deviceResources, position, velocity);
		case Element::OXYGEN:		return new Oxygen(m_deviceResources, position, velocity);
		case Element::FLOURINE:		return new Flourine(m_deviceResources, position, velocity);
		case Element::NEON:			return new Neon(m_deviceResources, position, velocity);
		default:
			return nullptr;
		}
	}

	Atom* AtomGenerator::CreateAtom(Element element, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount, int charge)
	{
		switch (element)
		{
		case Element::HYDROGEN:		return new Hydrogen(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::HELIUM:		return new Helium(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::LITHIUM:		return new Lithium(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::BERYLLIUM:	return new Beryllium(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::BORON:		return new Boron(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::CARBON:		return new Carbon(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::NITROGEN:		return new Nitrogen(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::OXYGEN:		return new Oxygen(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::FLOURINE:		return new Flourine(m_deviceResources, position, velocity, neutronCount, charge);
		case Element::NEON:			return new Neon(m_deviceResources, position, velocity, neutronCount, charge);
		default:
			return nullptr;
		}
	}
}