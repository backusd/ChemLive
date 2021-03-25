#pragma once

#include "pch.h"
#include "Constants.h"
#include "Electron.h"
#include "Enums.h"
#include "SphereMesh.h"
#include <vector>

using DirectX::XMFLOAT3;

namespace Simulation
{
	class Atom
	{
	public:
		// Constructors
		Atom(const std::shared_ptr<DX::DeviceResources>& deviceResources,
			Simulation::Element element,
			XMFLOAT3 position, XMFLOAT3 velocity);

		Atom(const std::shared_ptr<DX::DeviceResources>& deviceResources,
			Simulation::Element element,
			XMFLOAT3 position, XMFLOAT3 velocity,
			int neutronCount, int electronCount);

		// If you want to explicitly set the radius
		Atom(const std::shared_ptr<DX::DeviceResources>& deviceResources,
			Simulation::Element element,
			XMFLOAT3 position, XMFLOAT3 velocity, 
			int neutronCount, int electronCount,
			float radius);

		// Update
		virtual void Update(double timeDelta, const std::vector<Atom*>& atoms, XMFLOAT3 boxDimensions) = 0;

		// Render
		void Render(XMMATRIX viewProjectionMatrix);
		XMMATRIX ModelMatrix() { return m_sphereMesh->ModelMatrix(); }
		XMMATRIX TranslationMatrix() { return XMMatrixTranslation(m_position.x, m_position.y, m_position.z); }

		// Get
		XMFLOAT3 Position() { return m_position; };
		XMFLOAT3 Velocity() { return m_velocity; };
		Simulation::Element Element() { return m_element; }
		float Mass() { return static_cast<float>(m_element + m_neutronCount); }
		int ProtonsCount() { return m_element; }
		int NeutronsCount() { return m_neutronCount; }
		int ElectronsCount() { return m_electrons.size(); }
		float Radius() { return m_radius; }
		int Charge() { return ProtonsCount() - ElectronsCount(); }

		// Set
		void Velocity(XMFLOAT3 velocity) { m_velocity = velocity; }

	protected:
		std::unique_ptr<SphereMesh> m_sphereMesh;

		XMFLOAT3		m_position;
		XMFLOAT3		m_velocity;

		Simulation::Element	                   m_element;
		std::vector<std::shared_ptr<Electron>> m_electrons;

		int				m_neutronCount;		

		float			m_radius;
	};
}
