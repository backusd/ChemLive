#pragma once

#include "pch.h"
#include "Atom.h"

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Hydrogen : public Atom
	{
	public:
		// Constructors
		Hydrogen();
		Hydrogen(XMFLOAT3 position, XMFLOAT3 velocity);
		Hydrogen(XMFLOAT3 position, LENGTH_UNIT positionUnits, XMFLOAT3 velocity, VELOCITY_UNIT velocityUnits);

		// Update
		void Update(double timeDelta, TIME_UNIT timeUnits, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions, LENGTH_UNIT lengthUnits);

	};
}
