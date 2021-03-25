#include "pch.h"
#include "MoveLookController.h"

using winrt::Windows::System::VirtualKey;

namespace ChemLive
{
	MoveLookController::MoveLookController(CoreWindow window, XMFLOAT3 boxDimensions) :
		m_pitch(0.0f),
		m_yaw(0.0f),
		m_state(MoveLookControllerState::None),
        m_elapsedTime(0.0),
        m_xRadians(0.0f),
        m_yRadians(0.0f),
        m_zRadians(0.0f)
	{
		InitWindow(window);

        m_eyeVec = { 0.0f, 0.0f, 2 * boxDimensions.z, 0.0f };
        m_atVec = { 0.0f, 0.0f, 0.0f, 0.0f };								// look at the origin
        m_upVec = { 0.0f, 1.0f, 0.0f, 0.0f };								// Up in the positive y-direction
	}

	void MoveLookController::InitWindow(CoreWindow window)
	{
		ResetState();

        // Assign Pointer Events
        /*
        window.PointerPressed({ this, &MoveLookController::OnPointerPressed });
        window.PointerMoved({ this, &MoveLookController::OnPointerMoved });
        window.PointerReleased({ this, &MoveLookController::OnPointerReleased });

        window.KeyDown({ this, &MoveLookController::OnKeyDown });
        window.KeyUp({ this, &MoveLookController::OnKeyUp });
        */
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
        m_shift = false;
        m_forward = false;
        m_back = false;
        m_left = false;
        m_right = false;
        m_up = false;
        m_down = false;
        m_pause = false;
	}

    void MoveLookController::OnPointerPressed(CoreWindow, PointerEventArgs const& e)
    {
        // When the pointer is pressed begin tracking the pointer movement.
        m_mouseDown = true;
        m_mousePositionX = m_mousePositionXNew = e.CurrentPoint().Position().X;
        m_mousePositionY = m_mousePositionYNew = e.CurrentPoint().Position().Y;
    }
    void MoveLookController::OnPointerMoved(CoreWindow, PointerEventArgs const& e)
    {
        m_mousePositionXNew = e.CurrentPoint().Position().X;
        m_mousePositionYNew = e.CurrentPoint().Position().Y;
    }
    void MoveLookController::OnPointerReleased(CoreWindow, PointerEventArgs const& e)
    {
        // Stop tracking pointer movement when the pointer is released.
        m_mouseDown = false;
        m_mousePositionX = m_mousePositionXNew = e.CurrentPoint().Position().X;
        m_mousePositionY = m_mousePositionYNew = e.CurrentPoint().Position().Y;
    }

    void MoveLookController::OnKeyDown(CoreWindow, KeyEventArgs const& args)
    {
        VirtualKey key = args.VirtualKey();

        switch (key)
        {
        case VirtualKey::Up:    m_up    = true; break;
        case VirtualKey::Down:  m_down  = true; break;
        case VirtualKey::Left:  m_left  = true; break;
        case VirtualKey::Right: m_right = true; break;
        case VirtualKey::Shift: m_shift = true; break;
        }
    }
    void MoveLookController::OnKeyUp(CoreWindow, KeyEventArgs const& args)
    {
        VirtualKey key = args.VirtualKey();

        switch (key)
        {
        case VirtualKey::Up:    m_up    = false; break;
        case VirtualKey::Down:  m_down  = false; break;
        case VirtualKey::Left:  m_left  = false; break;
        case VirtualKey::Right: m_right = false; break;
        case VirtualKey::Shift: m_shift = false; break;
        }

        // If no longer moving or rotating, reset the time to 0
        if (!(m_up || m_down || m_left || m_right))
            m_elapsedTime = 0.0f;
    }

    bool MoveLookController::IsMoving()
    {
        return m_up || m_down || m_left || m_right || m_mouseDown;
    }

    XMMATRIX MoveLookController::ViewMatrix()
    {
        return XMMatrixLookAtRH(m_eyeVec, m_atVec, m_upVec);
    }

    void MoveLookController::Update(DX::StepTimer const& timer, D2D1_RECT_F renderPaneRect)
    {
        if (m_mouseDown)
        {
            // Compute the eye distance to center
            float radius = 0.0f;
            XMStoreFloat(&radius, XMVector3Length(m_eyeVec));

            // If the pointer were to move from the middle of the screen to the far right,
            // that should produce one full rotation. Therefore, set a rotationFactor = 2
            float rotationFactor = 2.0f;
            float renderPaneWidth = renderPaneRect.right - renderPaneRect.left;
            float renderPaneHeight = renderPaneRect.bottom - renderPaneRect.top;

            float radiansPerPixelX = (XM_2PI / renderPaneWidth) * rotationFactor;
            float radiansPerPixelY = (XM_2PI / renderPaneHeight) * rotationFactor;

            float thetaX = radiansPerPixelX * (m_mousePositionX - m_mousePositionXNew);
            float thetaY = radiansPerPixelY * (m_mousePositionYNew - m_mousePositionY);

            // Rotate
            RotateLeftRight(thetaX);
            RotateUpDown(thetaY);

            // reset the mouse position variables
            m_mousePositionX = m_mousePositionXNew;
            m_mousePositionY = m_mousePositionYNew;
        }
        else if (m_up || m_down || m_left || m_right)
        {
            // When a button is pressed, we must begin tracking the time before we can make an update
            if (m_elapsedTime < 0.01f)
            {
                m_elapsedTime = timer.GetTotalSeconds();
                return;
            }

            // Compute the time delta
            double currentTime = timer.GetTotalSeconds();
            double timeDelta = currentTime - m_elapsedTime;
            m_elapsedTime = currentTime;

            // Compute the rotation
            float radiansPerSecond = 0.5;
            float theta = timeDelta * radiansPerSecond;

            // If rotating up or right, make the angle negative so the rest of the math is the same
            if (m_up || m_right)
                theta *= -1;

            if (m_up || m_down)
                RotateUpDown(theta);

            if (m_left || m_right)
                RotateLeftRight(theta);
        }
    }

    void MoveLookController::RotateUpDown(float theta)
    {
        // Use Rodrigue's Rotation Formula
        //     See here: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
        //     v_rot : vector after rotation
        //     v     : vector before rotation
        //     theta : angle of rotation
        //     k     : unit vector representing axis of rotation
        //     v_rot = v*cos(theta) + (k x v)*sin(theta) + k*(k dot v)*(1-cos(theta))

        // The axis of rotation vector for up/down rotation will be the cross product 
        // between the eye-vector and the up-vector (must make it a unit vector)
        XMVECTOR v = m_eyeVec;
        XMVECTOR k = XMVector3Normalize(XMVector3Cross(m_eyeVec, m_upVec));
        m_eyeVec = v * cos(theta) + XMVector3Cross(k, v) * sin(theta) + k * XMVector3Dot(k, v) * (1 - cos(theta));

        // Now update the new up-vector should be the cross product between the k-vector and the new eye-vector
        m_upVec = XMVector3Normalize(XMVector3Cross(k, m_eyeVec));
    }

    void MoveLookController::RotateLeftRight(float theta)
    {
        // Use Rodrigue's Rotation Formula
        //     See here: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
        //     v_rot : vector after rotation
        //     v     : vector before rotation
        //     theta : angle of rotation
        //     k     : unit vector representing axis of rotation
        //     v_rot = v*cos(theta) + (k x v)*sin(theta) + k*(k dot v)*(1-cos(theta))
        
        XMVECTOR v = m_eyeVec;
        XMVECTOR k = m_upVec;
        m_eyeVec = v * cos(theta) + XMVector3Cross(k, v) * sin(theta) + k * XMVector3Dot(k, v) * (1 - cos(theta));
    
        // Do NOT change the up-vector
    }
}