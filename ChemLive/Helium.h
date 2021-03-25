#pragma once

#include "pch.h"
#include "Atom.h"
#include "Enums.h"

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Helium : public Atom
	{
	public:
		// Constructors
		Helium(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount = 2, int charge = 0);

		// Update
		void Update(double timeDelta, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions);
	};
}