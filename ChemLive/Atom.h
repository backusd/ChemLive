#pragma once

#include "pch.h"
#include "Electron.h"
#include "Enums.h"

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Atom
	{
	public:
		// Constructors
		Atom();
		Atom(XMFLOAT3 position, XMFLOAT3 velocity, float mass, int numProtons);
		Atom(XMFLOAT3 position, LENGTH_UNIT positionUnits, 
			 XMFLOAT3 velocity, VELOCITY_UNIT velocityUnits,
			 float mass,        MASS_UNIT massUnits,
			 int numProtons);

		// Update
		virtual void Update(double timeDelta, TIME_UNIT timeUnits, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions, LENGTH_UNIT lengthUnits) = 0;

		// GET
		XMFLOAT3 Position() {			return m_position; };
		LENGTH_UNIT PositionUnits() {	return m_positionUnits; };
		XMFLOAT3 Velocity() {			return m_velocity; };
		VELOCITY_UNIT VelocityUnits() {	return m_velocityUnits; };
		float Mass() {					return m_mass; };
		MASS_UNIT MassUnits() {			return m_massUnits; };
		int NumProtons() {				return m_numProtons; };

		// SET
		void Position(XMFLOAT3 position) {					m_position = position; };
		void PositionUnits(LENGTH_UNIT positionUnits) {		m_positionUnits = positionUnits; };
		void Velocity(XMFLOAT3 velocity) {					m_velocity = velocity; };
		void VelocityUnits(VELOCITY_UNIT velocityUnits) {	m_velocityUnits = velocityUnits; };
		void Mass(float mass) {								m_mass = mass; };
		void MassUnits(MASS_UNIT massUnits) {				m_massUnits = massUnits; };
		void NumProtons(int numProtons) {					m_numProtons = numProtons; };

	protected:


		XMFLOAT3		m_position;
		LENGTH_UNIT		m_positionUnits;

		XMFLOAT3		m_velocity;
		VELOCITY_UNIT	m_velocityUnits;

		float			m_mass;
		MASS_UNIT		m_massUnits;

		int				m_numProtons;
	};
}
