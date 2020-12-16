#include "pch.h"
#include "Hydrogen.h"

namespace Simulation
{
	Hydrogen::Hydrogen() : Atom()
	{
		// Set the mass to a single proton
		this->Mass(1.0f);
		this->MassUnits(MASS_UNIT::PROTON_MASSES);
	}

	Hydrogen::Hydrogen(XMFLOAT3 position, XMFLOAT3 velocity) :
		Atom(position, velocity, 1.0f, 1)
	{
		// Make sure mass units are correct (even though it should be this by default)
		this->MassUnits(MASS_UNIT::PROTON_MASSES);
	}

	Hydrogen::Hydrogen(XMFLOAT3 position, LENGTH_UNIT positionUnits, XMFLOAT3 velocity, VELOCITY_UNIT velocityUnits) :
		Atom(position, positionUnits, velocity, velocityUnits, 1.0f, MASS_UNIT::PROTON_MASSES, 1)
	{
	}

	void Hydrogen::Update(double timeDelta, TIME_UNIT timeUnits, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions, LENGTH_UNIT lengthUnits)
	{
		// I will really want to create new data types: scientific_double and scientific_int
		// This will allow me to get rid of TIME_UNIT, LENGTH_UNIT, and such
		// In the mean time, all of the units are set up correctly, so just ignore the units for now

		// Move the position
		m_position.x += (timeDelta * m_velocity.x);
		m_position.y += (timeDelta * m_velocity.y);
		m_position.z += (timeDelta * m_velocity.z);

		// Bounce off the simulation wall (don't worry about relocation, just flip the velocity)
		if (m_position.x > (boxDimensions.x / 2.0f) || m_position.x < -1 * (boxDimensions.x / 2.0f))
			m_velocity.x *= -1;

		if (m_position.y > (boxDimensions.y / 2.0f) || m_position.y < -1 * (boxDimensions.y / 2.0f))
			m_velocity.y *= -1;

		if (m_position.z > (boxDimensions.z / 2.0f) || m_position.z < -1 * (boxDimensions.z / 2.0f))
			m_velocity.z *= -1;

		// Eventually, you need to implement real physics here
		// ...
		// ...
	}

}