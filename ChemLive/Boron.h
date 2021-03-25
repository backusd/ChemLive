#pragma once

#include "pch.h"
#include "Atom.h"
#include "Enums.h"

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Boron : public Atom
	{
	public:
		// Constructors
		// Most common isotope = Boron-11
		// Most common charge  = 0 (3+ and 3- are common)
		Boron(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount = 6, int charge = 0);

		// Update
		void Update(double timeDelta, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions);
	};
}