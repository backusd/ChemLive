#pragma once

#include "pch.h"

#include "Atom.h"
#include "DeviceResources.h"
#include "Enums.h"

#include "Hydrogen.h"
#include "Helium.h"

namespace Simulation
{
	class AtomGenerator
	{
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;


	public:
		AtomGenerator(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		Atom* CreateAtom(Element element, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount, int charge = 0);
	};
}