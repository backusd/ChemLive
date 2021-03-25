#pragma once

#include "pch.h"

#include "DeviceResources.h"
#include "Enums.h"
#include "Elements.h"		// <-- includes header files for all elements

namespace Simulation
{
	class AtomGenerator
	{
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;


	public:
		AtomGenerator(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		Atom* CreateAtom(Element element, XMFLOAT3 position, XMFLOAT3 velocity);
		Atom* CreateAtom(Element element, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount, int charge);
	};
}