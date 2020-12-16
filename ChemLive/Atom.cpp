#include "pch.h"
#include "Atom.h"

namespace Simulation
{
	// create a neutral atom with unit mass located at (0,0,0) that is sitting still
	Atom::Atom() :
		m_position({ 0.0f, 0.0f, 0.0f }),
		m_velocity({ 0.0f, 0.0f, 0.0f }),
		m_mass(1.0f),
		m_numProtons(0),
		m_positionUnits(LENGTH_UNIT::NANOMETER),
		m_velocityUnits(VELOCITY_UNIT::NANOMETER_PER_SECOND),
		m_massUnits(MASS_UNIT::PROTON_MASSES)
	{
	}

	// Default units are in nano-X (except the mass units)
	Atom::Atom(XMFLOAT3 position, XMFLOAT3 velocity, float mass, int numProtons) :
		m_position(position),
		m_velocity(velocity),
		m_mass(mass),
		m_numProtons(numProtons),
		m_positionUnits(LENGTH_UNIT::NANOMETER),
		m_velocityUnits(VELOCITY_UNIT::NANOMETER_PER_SECOND),
		m_massUnits(MASS_UNIT::PROTON_MASSES)
	{
	}

	Atom::Atom(XMFLOAT3 position, LENGTH_UNIT positionUnits, XMFLOAT3 velocity, VELOCITY_UNIT velocityUnits, float mass, MASS_UNIT massUnits, int numProtons) :
		m_position(position),
		m_velocity(velocity),
		m_mass(mass),
		m_numProtons(numProtons),
		m_positionUnits(positionUnits),
		m_velocityUnits(velocityUnits),
		m_massUnits(massUnits)
	{
	}

}