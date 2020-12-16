#pragma once

#include "pch.h"
#include "Atom.h"
#include "DeviceResources.h"
#include "Enums.h"
#include "Hydrogen.h"
#include "SimulationRenderer.h"


/* 
*	This class contains all information about the current simulation, as
*	well as the ability to import/export a simulation or specific molecule.
*/

using DirectX::XMFLOAT3;


namespace Simulation
{
	class Simulation
	{
	public:
		Simulation(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		void AddAtom();
		void RemoveAtom();

		void PlaySimulation() {  m_paused = false; }
		void PauseSimulation() { m_paused = true; }
		void StartRecording();
		void StopRecording();

		void LoadSimulationFromFile();
		void SaveSimulationToFile();

		void ClearSimulation();	// Completely delete the entire active simulation
		void ResetSimulation(); // Reset the simulation state to where it was before ever pressing Play
		
		void UpdateSimulation(DX::StepTimer const& timer);

		// GET
		XMFLOAT3	BoxDimensions() {		return m_boxDimensions; }
		LENGTH_UNIT BoxDimensionUnits() {	return m_boxDimensionUnits; }
		bool		BoxVisible() {			return m_boxVisible; }

		float		ElapsedTime() {			return m_elapsedTime; }
		TIME_UNIT	ElapsedTimeUnit() {		return m_elapsedTimeUnit; }

		// SET
		void BoxDimensions(XMFLOAT3 dimensions);
		void BoxDimensionUnits(LENGTH_UNIT unit) {	m_boxDimensionUnits = unit; }
		void BoxVisible(bool visible) {				m_boxVisible = visible; }

		void ElapsedTime(float time) {				m_elapsedTime = time; }
		void ElapsedTimeUnit(TIME_UNIT timeUnit) {	m_elapsedTimeUnit = timeUnit; }

		// Methods that get passed along to SimulationRenderer
		void CreateWindowSizeDependentResources() {  m_simulationRenderer->CreateWindowSizeDependentResources(); }
		void Render() {								 m_simulationRenderer->Render(m_atoms, m_boxDimensions); }
		void ReleaseDeviceDependentResources() {	 m_simulationRenderer->ReleaseDeviceDependentResources(); }
		void CreateDeviceDependentResourcesAsync() { m_simulationRenderer->CreateDeviceDependentResourcesAsync(m_boxDimensions); }
		

	private:
		// Rendering
		std::unique_ptr<SimulationRenderer> m_simulationRenderer;

		// Box
		XMFLOAT3	m_boxDimensions;		// 3 floats to hold the MAX x,y,z dimensions for the simulation box (ex. if x = 10, then x-axis = [-10, 10])
		LENGTH_UNIT m_boxDimensionUnits;	// The m_dimensions values will all be interpretted to be a specific unit
		bool		m_boxVisible;			// If true, the dimension box will be outlined

		// Time
		float		m_elapsedTime;
		TIME_UNIT	m_elapsedTimeUnit;

		// Atoms
		std::vector<Atom*> m_atoms;			// List of Atoms active in the simulation

		// State
		bool m_paused;
	};
}