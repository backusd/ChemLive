#include "pch.h"
#include "Simulation.h"


namespace Simulation
{
	Simulation::Simulation(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_boxDimensions({ 2.0f, 2.0f, 2.0f }),		// These are the overall dimensions - so the x range is [-5, 5]
		m_boxVisible(true),
		m_elapsedTime(0.0f),
		m_paused(true),
		m_atomGenerator(AtomGenerator(deviceResources))
	{

		// TEMPORARY SETUP ===================================

		
		XMFLOAT3 initPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 initVelocity = XMFLOAT3(-1.0f, 0.0f, 0.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::HYDROGEN, initPos, initVelocity));		
		
		initPos = XMFLOAT3(0.0f, 0.75f, 0.0f);
		initVelocity = XMFLOAT3(1.0f, -1.0f, 0.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::HELIUM, initPos, initVelocity));		
		
		initPos = XMFLOAT3(0.5f, 0.0f, 0.0f);
		initVelocity = XMFLOAT3(-1.0f, 1.0f, 0.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::HYDROGEN, initPos, initVelocity));	

		initPos = XMFLOAT3(0.5f, 0.5f, 0.0f);
		initVelocity = XMFLOAT3(-1.0f, 1.0f, 0.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::LITHIUM, initPos, initVelocity));

		initPos = XMFLOAT3(0.5f, 0.5f, 0.5f);
		initVelocity = XMFLOAT3(-1.0f, 1.0f, 1.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::BERYLLIUM, initPos, initVelocity));

		initPos = XMFLOAT3(0.0f, 0.0f, 0.5f);
		initVelocity = XMFLOAT3(0.0f, 1.0f, 1.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::BORON, initPos, initVelocity));

		initPos = XMFLOAT3(0.5f, 0.0f, 0.5f);
		initVelocity = XMFLOAT3(1.0f, 1.0f, 0.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::CARBON, initPos, initVelocity));

		initPos = XMFLOAT3(0.5f, 0.5f, 0.8f);
		initVelocity = XMFLOAT3(1.0f, 0.5f, 1.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::NITROGEN, initPos, initVelocity));

		initPos = XMFLOAT3(0.7f, 0.3f, 0.1f);
		initVelocity = XMFLOAT3(0.0f, 0.5f, 1.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::OXYGEN, initPos, initVelocity));

		initPos = XMFLOAT3(0.2f, 0.6f, 0.7f);
		initVelocity = XMFLOAT3(0.0f, 0.5f, 1.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::FLOURINE, initPos, initVelocity));
		
		initPos = XMFLOAT3(-0.7f, 0.2f, 0.7f);
		initVelocity = XMFLOAT3(-1.0f, 0.5f, 1.0f);
		AddAtom(m_atomGenerator.CreateAtom(Element::NEON, initPos, initVelocity));

		m_paused = false;
	}

	void Simulation::AddAtom(Atom* atom)
	{
		// We need to make sure that the atoms are sorted by element type
		// So insert the new atom in the first spot after all of the elements with smaller
		// element numbers

		// Get the first index where the current element is greater than or equal to the new atom
		unsigned int index;
		for (index = 0; index < m_atoms.size(); ++index)
		{
			if (m_atoms[index]->Element() >= atom->Element())
				break;
		}

		// if we got to the end, just add the new atom
		// else, insert it at the appropriate spot in the vector
		if (index == m_atoms.size())
			m_atoms.push_back(atom);
		else
			m_atoms.insert(m_atoms.begin() + index, atom);

	}
	void Simulation::RemoveAtom()
	{

	}


	void Simulation::StartRecording()
	{

	}
	void Simulation::StopRecording()
	{

	}

	void Simulation::LoadSimulationFromFile()
	{

	}
	void Simulation::SaveSimulationToFile()
	{

	}

	void Simulation::ClearSimulation()
	{

	}
	void Simulation::ResetSimulation()
	{

	}

	void Simulation::Update(DX::StepTimer const& timer)
	{
		/* This function could be made HIGHLY parallel,
		* and should probably even execute on the GPU
		*/

		if (!m_paused)
		{
			// if the elapsed time is -1, then the simulation was just unpaused
			// and we need to refresh the time
			if (m_elapsedTime == -1.0f)
				m_elapsedTime = timer.GetTotalSeconds();

			double currentTime = timer.GetTotalSeconds();
			double timeDelta = currentTime - m_elapsedTime;

			// We probably don't want to simply run the update method without
			// passing along knowledge of the locations of other atoms
			// You probably want a read only buffer of all atom locations that
			// gets passed to the update call for each atom and is later updated
			// once all atoms have been updated

			for (Atom* atom : m_atoms)
				atom->Update(timeDelta, m_atoms, m_boxDimensions);

			// The update procedure currently only updates position and takes account of the simulation wall
			// Here, we need to make updates to account for elastic collisions with other atoms
			// This is temporary however, because we will need to move past elastic collisions to simulate
			// real physics
			// See here for math explanation: https://exploratoria.github.io/exhibits/mechanics/elastic-collisions-in-3d/

			XMFLOAT3 d; // distance between atoms
			float mag;  // magnitude of the distance vector
			XMFLOAT3 n; // normal vector between balls
			XMFLOAT3 vrel; // relative velocity between the atoms
			XMFLOAT3 vnorm; // relative velocity along the normal direction
			float vreldotnorm; // the dot product between vrel and vnorm
			XMFLOAT3 newV1, newV2; // new velocity vectors post-collision
			for (unsigned int iii = 0; iii < m_atoms.size(); ++iii)
			{
				for (unsigned int jjj = iii + 1; jjj < m_atoms.size(); ++jjj)
				{
					// check distance between the two atoms
					// currently assuming a radius of 1 and identical masses
					d.x = m_atoms[iii]->Position().x - m_atoms[jjj]->Position().x;
					d.y = m_atoms[iii]->Position().y - m_atoms[jjj]->Position().y;
					d.z = m_atoms[iii]->Position().z - m_atoms[jjj]->Position().z;

					mag = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
					if (mag < m_atoms[iii]->Radius() + m_atoms[jjj]->Radius()) // assume radius for each atom is 1
					{
						// compute a normalized normal vector between the atoms
						n.x = d.x / mag;
						n.y = d.y / mag;
						n.z = d.z / mag;

						// compute the relative velocity between the atoms
						vrel.x = m_atoms[iii]->Velocity().x - m_atoms[jjj]->Velocity().x;
						vrel.y = m_atoms[iii]->Velocity().y - m_atoms[jjj]->Velocity().y;
						vrel.z = m_atoms[iii]->Velocity().z - m_atoms[jjj]->Velocity().z;

						// compute the relative velocity along the normal direction;
						vreldotnorm = vrel.x * n.x + vrel.y * n.y + vrel.z * n.z;
						vnorm.x = vreldotnorm * n.x;
						vnorm.y = vreldotnorm * n.y;
						vnorm.z = vreldotnorm * n.z;

						// exchange normal velocities
						newV1.x = m_atoms[iii]->Velocity().x - vnorm.x;
						newV1.y = m_atoms[iii]->Velocity().y - vnorm.y;
						newV1.z = m_atoms[iii]->Velocity().z - vnorm.z;
						m_atoms[iii]->Velocity(newV1);

						newV2.x = m_atoms[jjj]->Velocity().x + vnorm.x;
						newV2.y = m_atoms[jjj]->Velocity().y + vnorm.y;
						newV2.z = m_atoms[jjj]->Velocity().z + vnorm.z;
						m_atoms[jjj]->Velocity(newV2);
					}
				}
			}

			m_elapsedTime = currentTime;

		}
	}
}