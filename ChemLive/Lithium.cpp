#include "pch.h"
#include "Lithium.h"

namespace Simulation
{

	Lithium::Lithium(const std::shared_ptr<DX::DeviceResources>& deviceResources,
		XMFLOAT3 position, XMFLOAT3 velocity, int neutronCount, int charge) :
		Atom(deviceResources, Element::LITHIUM, position, velocity, neutronCount, Element::LITHIUM - charge)
	{
	}

	void Lithium::Update(double timeDelta, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions)
	{
		// I will really want to create new data types: scientific_double and scientific_int
		// This will allow me to get rid of TIME_UNIT, LENGTH_UNIT, and such
		// In the mean time, all of the units are set up correctly, so just ignore the units for now

		// Move the position
		m_position.x += (timeDelta * m_velocity.x);
		m_position.y += (timeDelta * m_velocity.y);
		m_position.z += (timeDelta * m_velocity.z);

		// Bounce off the simulation wall - We can't just flip th velocity because when an atom is small enough and the velocity
		// large enough, it is possible for the center of the atom to find itself outside the box

		// Positive X Wall
		float delta = (m_position.x + m_radius) - (boxDimensions.x / 2.0f);
		if (delta > 0)
		{
			m_position.x -= delta;
			m_velocity.x *= -1;
		}
		else
		{
			// Negative X Wall
			delta = (m_position.x - m_radius) + (boxDimensions.x / 2.0f);
			if (delta < 0)
			{
				m_position.x -= delta;
				m_velocity.x *= -1;
			}
		}

		// Positive Y Wall
		delta = (m_position.y + m_radius) - (boxDimensions.y / 2.0f);
		if (delta > 0)
		{
			m_position.y -= delta;
			m_velocity.y *= -1;
		}
		else
		{
			// Negative Y Wall
			delta = (m_position.y - m_radius) + (boxDimensions.y / 2.0f);
			if (delta < 0)
			{
				m_position.y -= delta;
				m_velocity.y *= -1;
			}
		}

		// Positive Z Wall
		delta = (m_position.z + m_radius) - (boxDimensions.z / 2.0f);
		if (delta > 0)
		{
			m_position.z -= delta;
			m_velocity.z *= -1;
		}
		else
		{
			// Negative Z Wall
			delta = (m_position.z - m_radius) + (boxDimensions.z / 2.0f);
			if (delta < 0)
			{
				m_position.z -= delta;
				m_velocity.z *= -1;
			}
		}

		/*
		if ((m_position.x + m_radius) > (boxDimensions.x / 2.0f) || (m_position.x - m_radius) < -1 * (boxDimensions.x / 2.0f))
			m_velocity.x *= -1;

		if ((m_position.y + m_radius) > (boxDimensions.y / 2.0f) || (m_position.y - m_radius) < -1 * (boxDimensions.y / 2.0f))
			m_velocity.y *= -1;

		if ((m_position.z + m_radius) > (boxDimensions.z / 2.0f) || (m_position.z - m_radius) < -1 * (boxDimensions.z / 2.0f))
			m_velocity.z *= -1;

		*/


		// Eventually, you need to implement real physics here
		// ...
		// ...
	}


}