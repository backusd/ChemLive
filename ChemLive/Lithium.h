#pragma once

#include "pch.h"
#include "Atom.h"
#include "Enums.h"

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Lithium : public Atom
	{
	public:
		// Constructors
		// Most common isotope = Lithium-7
		// Most common charge  = +1
		Lithium(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount = 4, int charge = 1);

		// Update
		void Update(double timeDelta, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions);
	};
}