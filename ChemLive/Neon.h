#pragma once

#include "pch.h"
#include "Atom.h"
#include "Enums.h"

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Neon : public Atom
	{
	public:
		// Constructors
		// Most common isotope = Neon-20
		// Most common charge  = 0
		Neon(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount = 10, int charge = 0);

		// Update
		void Update(double timeDelta, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions);
	};
}