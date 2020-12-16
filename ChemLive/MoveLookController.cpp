#include "pch.h"
#include "MoveLookController.h"



namespace ChemLive
{
	MoveLookController::MoveLookController(CoreWindow window) :
		m_pitch(0.0f),
		m_yaw(0.0f),
		m_state(MoveLookControllerState::None)
	{
		InitWindow(window);
	}

	void MoveLookController::InitWindow(CoreWindow window)
	{
		ResetState();

        // Assign Pointer Events
        window.PointerPressed({ this, &MoveLookController::OnPointerPressed });
        window.PointerMoved({ this, &MoveLookController::OnPointerMoved });
        window.PointerReleased({ this, &MoveLookController::OnPointerReleased });
	}

	void MoveLookController::ResetState()
	{
        m_mouseDown = false;

        // Reset the state of the MoveLookController.
        // Disable any active pointer IDs to stop all interaction.
        m_buttonPressed = false;
        m_pausePressed = false;
        m_buttonInUse = false;
        m_moveInUse = false;
        m_lookInUse = false;
        m_mouseInUse = false;
        m_mouseLeftInUse = false;
        m_mouseRightInUse = false;
        m_movePointerID = 0;
        m_lookPointerID = 0;
        m_mousePointerID = 0;
        m_velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        m_moveCommand = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        m_forward = false;
        m_back = false;
        m_left = false;
        m_right = false;
        m_up = false;
        m_down = false;
        m_pause = false;
	}

    void MoveLookController::OnPointerPressed(CoreWindow, PointerEventArgs const&)
    {
        // When the pointer is pressed begin tracking the pointer movement.
        m_mouseDown = true;
    }
    void MoveLookController::OnPointerMoved(CoreWindow, PointerEventArgs const& e)
    {
        // Update the pointer tracking code.
        if (m_mouseDown)
        {
            m_mousePositionX = e.CurrentPoint().Position().X;
        }
    }
    void MoveLookController::OnPointerReleased(CoreWindow, PointerEventArgs const&)
    {
        // Stop tracking pointer movement when the pointer is released.
        m_mouseDown = false;
    }
}