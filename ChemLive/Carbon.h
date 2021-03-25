#pragma once

#include "pch.h"
#include "Atom.h"
#include "Enums.h"

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Carbon : public Atom
	{
	public:
		// Constructors
		// Most common isotope = Carbon-12
		// Most common charge  = 0
		Carbon(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount = 6, int charge = 0);

		// Update
		void Update(double timeDelta, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions);
	};
}